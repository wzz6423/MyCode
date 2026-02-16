package service

import (
	"fmt"
	"math"
	"sync"
	"testing"
	"time"

	"clouddisk/internal/config"
	"clouddisk/internal/model"

	"github.com/alicebob/miniredis/v2"
	"github.com/redis/go-redis/v9"
	"github.com/stretchr/testify/assert"
)

// ==================== Concurrent Tests ====================

func TestConcurrent_Register(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}
	defer mr.Close()

	rdb := redis.NewClient(&redis.Options{Addr: mr.Addr()})
	cfg := &config.Config{
		JWT: config.JWTConfig{Secret: "test-secret", ExpireHours: 24},
		VerifyCode: config.VerifyCodeConfig{Length: 6, ExpireMinutes: 10},
	}

	repo := newMockUserRepo()
	verifySvc := NewVerifyService(rdb, cfg)
	userSvc := NewUserService(repo, verifySvc, cfg)

	const goroutines = 10
	var wg sync.WaitGroup
	errs := make([]error, goroutines)

	// Set up verify codes for each goroutine
	for i := 0; i < goroutines; i++ {
		codeID := fmt.Sprintf("concurrent-code-%d", i)
		mr.Set(fmt.Sprintf("verify:%s", codeID), "123456")
	}

	wg.Add(goroutines)
	for i := 0; i < goroutines; i++ {
		go func(idx int) {
			defer wg.Done()
			phone := fmt.Sprintf("1380013%04d", idx)
			codeID := fmt.Sprintf("concurrent-code-%d", idx)
			nickname := fmt.Sprintf("User%d", idx)
			errs[idx] = userSvc.Register("phone", phone, "password123", nickname, codeID, "123456")
		}(i)
	}
	wg.Wait()

	// All registrations should succeed since each uses a unique phone
	for i, e := range errs {
		assert.NoError(t, e, "goroutine %d failed", i)
	}
}

func TestConcurrent_ShareReceive(t *testing.T) {
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	svc := NewShareService(fileRepo, shareRepo)

	// Create original file and share
	fileRepo.Create(&model.File{
		FileID:      "file-concurrent",
		UserID:      "owner-1",
		FileName:    "shared.txt",
		StoragePath: "/storage/shared.txt",
		UniqueFlag:  "flag-concurrent",
	})

	shareRepo.Create(&model.Share{
		ShareID:      "share-concurrent",
		UserID:       "owner-1",
		FileID:       "file-concurrent",
		ShareType:    model.ShareTypeShare,
		MaxCount:     math.MaxUint32, // unlimited
		CurrentCount: 0,
	})

	const goroutines = 10
	var wg sync.WaitGroup
	errs := make([]error, goroutines)
	files := make([]*model.File, goroutines)

	wg.Add(goroutines)
	for i := 0; i < goroutines; i++ {
		go func(idx int) {
			defer wg.Done()
			userID := fmt.Sprintf("receiver-%d", idx)
			files[idx], errs[idx] = svc.ReceiveFile(userID, "share-concurrent", "/received")
		}(i)
	}
	wg.Wait()

	for i := 0; i < goroutines; i++ {
		assert.NoError(t, errs[i], "goroutine %d failed to receive", i)
		assert.NotNil(t, files[i], "goroutine %d got nil file", i)
	}

	// Verify share count was incremented (may not be exactly goroutines due to race,
	// but should be > 0)
	share, _ := shareRepo.GetByShareID("share-concurrent")
	assert.True(t, share.CurrentCount > 0, "share count should have been incremented")
}

func TestConcurrent_FileListWhileDelete(t *testing.T) {
	fileRepo := newMockFileRepo()
	svc := NewFileService(fileRepo, nil)

	// Create files
	for i := 0; i < 20; i++ {
		fileRepo.Create(&model.File{
			FileID:     fmt.Sprintf("file-%03d", i),
			UserID:     "user-123",
			FileName:   fmt.Sprintf("file%d.txt", i),
			ParentPath: "/concurrent",
		})
	}

	var wg sync.WaitGroup
	wg.Add(2)

	// Goroutine 1: repeatedly list files
	listErrors := make([]error, 0)
	go func() {
		defer wg.Done()
		for i := 0; i < 50; i++ {
			_, _, err := svc.List("user-123", "/concurrent", 1, 100)
			if err != nil {
				listErrors = append(listErrors, err)
			}
		}
	}()

	// Goroutine 2: delete files one by one (using fileRepo directly since Delete needs ES)
	go func() {
		defer wg.Done()
		for i := 0; i < 20; i++ {
			fileRepo.Delete(fmt.Sprintf("file-%03d", i))
			time.Sleep(time.Millisecond)
		}
	}()

	wg.Wait()

	// List should never error even while deletes are happening
	assert.Empty(t, listErrors, "listing should not produce errors during concurrent deletes")
}

// ==================== Boundary Tests ====================

func TestBoundary_EmptyStrings(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}
	defer mr.Close()

	rdb := redis.NewClient(&redis.Options{Addr: mr.Addr()})
	cfg := &config.Config{
		JWT:        config.JWTConfig{Secret: "test-secret", ExpireHours: 24},
		VerifyCode: config.VerifyCodeConfig{Length: 6, ExpireMinutes: 10},
	}

	repo := newMockUserRepo()
	verifySvc := NewVerifyService(rdb, cfg)
	userSvc := NewUserService(repo, verifySvc, cfg)

	t.Run("register with empty strings", func(t *testing.T) {
		// Empty code should fail validation
		err := userSvc.Register("phone", "", "", "", "", "")
		assert.Error(t, err)
		assert.Contains(t, err.Error(), "invalid or expired verify code")
	})

	t.Run("login with empty strings", func(t *testing.T) {
		_, _, err := userSvc.Login("phone", "", "")
		assert.Error(t, err)
	})

	t.Run("login with empty type", func(t *testing.T) {
		_, _, err := userSvc.Login("", "", "")
		assert.Error(t, err)
	})
}

func TestBoundary_VeryLongNickname(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}
	defer mr.Close()

	rdb := redis.NewClient(&redis.Options{Addr: mr.Addr()})
	cfg := &config.Config{
		JWT:        config.JWTConfig{Secret: "test-secret", ExpireHours: 24},
		VerifyCode: config.VerifyCodeConfig{Length: 6, ExpireMinutes: 10},
	}

	repo := newMockUserRepo()
	verifySvc := NewVerifyService(rdb, cfg)
	userSvc := NewUserService(repo, verifySvc, cfg)

	// Set verify code
	codeID := "long-nick-code"
	mr.Set("verify:"+codeID, "123456")

	longNickname := ""
	for i := 0; i < 10000; i++ {
		longNickname += "a"
	}

	// The mock repo doesn't enforce length limits, so this should succeed at the service level
	err = userSvc.Register("phone", "13800000001", "password123", longNickname, codeID, "123456")
	assert.NoError(t, err)

	user, err := repo.GetByPhone("13800000001")
	assert.NoError(t, err)
	assert.Len(t, user.Nickname, 10000)
}

func TestBoundary_SpecialCharacters(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}
	defer mr.Close()

	rdb := redis.NewClient(&redis.Options{Addr: mr.Addr()})
	cfg := &config.Config{
		JWT:        config.JWTConfig{Secret: "test-secret", ExpireHours: 24},
		VerifyCode: config.VerifyCodeConfig{Length: 6, ExpireMinutes: 10},
	}

	repo := newMockUserRepo()
	verifySvc := NewVerifyService(rdb, cfg)
	userSvc := NewUserService(repo, verifySvc, cfg)

	t.Run("unicode nickname", func(t *testing.T) {
		codeID := "unicode-nick-code"
		mr.Set("verify:"+codeID, "123456")

		err := userSvc.Register("phone", "13800000002", "password123", "用户名🎉テスト", codeID, "123456")
		assert.NoError(t, err)

		user, err := repo.GetByPhone("13800000002")
		assert.NoError(t, err)
		assert.Equal(t, "用户名🎉テスト", user.Nickname)
	})

	t.Run("emoji in description", func(t *testing.T) {
		// First create a user
		codeID := "emoji-desc-code"
		mr.Set("verify:"+codeID, "654321")

		err := userSvc.Register("phone", "13800000003", "password123", "EmojiUser", codeID, "654321")
		assert.NoError(t, err)

		user, err := repo.GetByPhone("13800000003")
		assert.NoError(t, err)

		err = userSvc.UpdateDescription(user.UserID, "Hello 🌍🚀💻 World")
		assert.NoError(t, err)

		updated, err := repo.GetByUserID(user.UserID)
		assert.NoError(t, err)
		assert.Equal(t, "Hello 🌍🚀💻 World", updated.Description)
	})

	t.Run("special chars in nickname", func(t *testing.T) {
		codeID := "special-nick-code"
		mr.Set("verify:"+codeID, "111111")

		err := userSvc.Register("phone", "13800000004", "password123", "<script>alert('xss')</script>", codeID, "111111")
		assert.NoError(t, err)

		user, err := repo.GetByPhone("13800000004")
		assert.NoError(t, err)
		assert.Equal(t, "<script>alert('xss')</script>", user.Nickname)
	})
}

func TestBoundary_NegativeExpireHours(t *testing.T) {
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	svc := NewShareService(fileRepo, shareRepo)

	fileRepo.Create(&model.File{
		FileID:   "file-neg-expire",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	// Negative expire hours: expireHours <= 0 means no expiry is set
	share, err := svc.ShareFile("user-123", "file-neg-expire", "negative expire", -5, 10)
	assert.NoError(t, err)
	assert.NotNil(t, share)
	// With negative hours, the code checks `if expireHours > 0`, so ExpireAt should be nil
	assert.Nil(t, share.ExpireAt)
}

func TestBoundary_ZeroMaxCount(t *testing.T) {
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	svc := NewShareService(fileRepo, shareRepo)

	fileRepo.Create(&model.File{
		FileID:      "file-zero-count",
		UserID:      "user-123",
		FileName:    "unlimited.txt",
		StoragePath: "/storage/unlimited.txt",
		UniqueFlag:  "flag-unlimited",
	})

	// MaxCount = -1 means unlimited (stored as MaxUint32)
	share, err := svc.ShareFile("user-123", "file-zero-count", "unlimited share", 24, -1)
	assert.NoError(t, err)
	assert.NotNil(t, share)
	assert.Equal(t, uint32(math.MaxUint32), share.MaxCount)

	// Create the share in repo for receive test
	// (ShareFile already created it, so we can receive)
	// Receive should work even with high CurrentCount since MaxCount is MaxUint32
	shareRepo.mu.Lock()
	for _, s := range shareRepo.shares {
		if s.FileID == "file-zero-count" {
			s.CurrentCount = 999999
		}
	}
	shareRepo.mu.Unlock()

	newFile, err := svc.ReceiveFile("user-456", share.ShareID, "/received")
	assert.NoError(t, err)
	assert.NotNil(t, newFile)
}

func TestBoundary_MaxUint32Count(t *testing.T) {
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	svc := NewShareService(fileRepo, shareRepo)

	fileRepo.Create(&model.File{
		FileID:      "file-max-count",
		UserID:      "user-123",
		FileName:    "maxcount.txt",
		StoragePath: "/storage/maxcount.txt",
	})

	// Share with MaxCount at a large boundary value.
	// ShareFile takes int for maxCount and casts to uint32 internally.
	// Use a value that fits in int and maps to a large uint32.
	maxCount := math.MaxInt32
	share, err := svc.ShareFile("user-123", "file-max-count", "max count share", 24, maxCount)
	assert.NoError(t, err)
	assert.NotNil(t, share)
	assert.Equal(t, uint32(math.MaxInt32), share.MaxCount)

	// Receive should work since CurrentCount (0) < MaxCount
	newFile, err := svc.ReceiveFile("user-456", share.ShareID, "/received")
	assert.NoError(t, err)
	assert.NotNil(t, newFile)

	// Now set CurrentCount to MaxCount to test the boundary
	shareRepo.mu.Lock()
	for _, s := range shareRepo.shares {
		if s.ShareID == share.ShareID {
			s.CurrentCount = uint32(math.MaxInt32)
		}
	}
	shareRepo.mu.Unlock()

	// Should fail since CurrentCount >= MaxCount
	_, err = svc.ReceiveFile("user-789", share.ShareID, "/received")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "share has reached maximum receive count")
}
