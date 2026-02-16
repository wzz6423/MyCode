package service

import (
	"encoding/json"
	"testing"
	"time"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"

	"github.com/alicebob/miniredis/v2"
	"github.com/redis/go-redis/v9"
	"github.com/stretchr/testify/assert"
)

// setupRegressionUserService creates a UserService with fresh mocks for regression tests.
func setupRegressionUserService(t *testing.T) (*UserService, *mockUserRepo, *miniredis.Miniredis) {
	t.Helper()
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	cfg := &config.Config{
		JWT: config.JWTConfig{
			Secret:      "regression-test-secret",
			ExpireHours: 24,
		},
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 10,
		},
	}

	repo := newMockUserRepo()
	verifySvc := NewVerifyService(rdb, cfg)
	userSvc := NewUserService(repo, verifySvc, cfg)

	return userSvc, repo, mr
}

// setupRegressionShareService creates a ShareService with fresh mocks for regression tests.
func setupRegressionShareService() (*ShareService, *mockFileRepo, *mockShareRepo) {
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	svc := NewShareService(fileRepo, shareRepo)
	return svc, fileRepo, shareRepo
}

// setupRegressionFileService creates a FileService with fresh mocks for regression tests.
func setupRegressionFileService() (*FileService, *mockFileRepo) {
	fileRepo := newMockFileRepo()
	svc := NewFileService(fileRepo, nil)
	return svc, fileRepo
}

func TestRegression_RegisterThenLoginImmediately(t *testing.T) {
	userSvc, _, mr := setupRegressionUserService(t)
	defer mr.Close()

	// Register
	codeID := "reg-imm-code"
	code := "111111"
	mr.Set("verify:"+codeID, code)

	err := userSvc.Register("phone", "13700001111", "pass123", "ImmUser", codeID, code)
	assert.NoError(t, err)

	// Login immediately with same credentials
	token, user, err := userSvc.Login("phone", "13700001111", "pass123")
	assert.NoError(t, err)
	assert.NotEmpty(t, token)
	assert.Equal(t, "ImmUser", user.Nickname)
	assert.NotNil(t, user.Phone)
	assert.Equal(t, "13700001111", *user.Phone)
}

func TestRegression_UpdatePhoneToExistingPhone(t *testing.T) {
	userSvc, repo, mr := setupRegressionUserService(t)
	defer mr.Close()

	// User A has phone X
	hashedA, _ := pkg.HashPassword("passA")
	phoneA := "13700002222"
	repo.Create(&model.User{
		UserID:   "user-A",
		Nickname: "UserA",
		Phone:    &phoneA,
		Password: hashedA,
	})

	// User B has a different phone
	hashedB, _ := pkg.HashPassword("passB")
	phoneB := "13700003333"
	repo.Create(&model.User{
		UserID:   "user-B",
		Nickname: "UserB",
		Phone:    &phoneB,
		Password: hashedB,
	})

	// User B tries to update to User A's phone
	codeID := "phone-conflict-code"
	code := "222222"
	mr.Set("verify:"+codeID, code)

	err := userSvc.UpdatePhone("user-B", "13700002222", codeID, code, "passB")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "phone already in use")
}

func TestRegression_UpdateEmailToExistingEmail(t *testing.T) {
	userSvc, repo, mr := setupRegressionUserService(t)
	defer mr.Close()

	// User A has email X
	hashedA, _ := pkg.HashPassword("passA")
	emailA := "usera@example.com"
	repo.Create(&model.User{
		UserID:   "user-A",
		Nickname: "UserA",
		Email:    &emailA,
		Password: hashedA,
	})

	// User B has a different email
	hashedB, _ := pkg.HashPassword("passB")
	emailB := "userb@example.com"
	repo.Create(&model.User{
		UserID:   "user-B",
		Nickname: "UserB",
		Email:    &emailB,
		Password: hashedB,
	})

	// User B tries to update to User A's email
	codeID := "email-conflict-code"
	code := "333333"
	mr.Set("verify:"+codeID, code)

	err := userSvc.UpdateEmail("user-B", "usera@example.com", codeID, code, "passB")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "email already in use")
}

func TestRegression_ShareThenUnshare_FileStatusReset(t *testing.T) {
	svc, fileRepo, _ := setupRegressionShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-reg-001",
		UserID:   "user-reg-001",
		FileName: "doc.pdf",
	})

	// Share the file
	share, err := svc.ShareFile("user-reg-001", "file-reg-001", "msg", 24, 10)
	assert.NoError(t, err)
	assert.NotNil(t, share)

	// Verify IsShared is true
	file, _ := fileRepo.GetByFileID("file-reg-001")
	assert.True(t, file.IsShared)

	// Unshare the file
	err = svc.UnshareFile("user-reg-001", "file-reg-001")
	assert.NoError(t, err)

	// Verify IsShared is false
	file, _ = fileRepo.GetByFileID("file-reg-001")
	assert.False(t, file.IsShared)
}

func TestRegression_PublicThenUnpublic_FileStatusReset(t *testing.T) {
	svc, fileRepo, _ := setupRegressionShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-reg-002",
		UserID:   "user-reg-001",
		FileName: "public-doc.pdf",
	})

	// Make public
	share, err := svc.PublicFile("user-reg-001", "file-reg-002", 48, 100)
	assert.NoError(t, err)
	assert.NotNil(t, share)

	// Verify IsPublic is true
	file, _ := fileRepo.GetByFileID("file-reg-002")
	assert.True(t, file.IsPublic)

	// Unpublic
	err = svc.UnpublicFile("user-reg-001", "file-reg-002")
	assert.NoError(t, err)

	// Verify IsPublic is false
	file, _ = fileRepo.GetByFileID("file-reg-002")
	assert.False(t, file.IsPublic)
}

func TestRegression_ReceiveOwnShare(t *testing.T) {
	svc, fileRepo, shareRepo := setupRegressionShareService()

	fileRepo.Create(&model.File{
		FileID:      "file-own-001",
		UserID:      "user-own-001",
		FileName:    "my-file.pdf",
		FileType:    model.FileTypeFile,
		FileSize:    512,
		StoragePath: "/storage/my-file.pdf",
		ParentPath:  "/",
		UniqueFlag:  "unique-own-001",
	})

	shareRepo.Create(&model.Share{
		ShareID:   "share-own-001",
		UserID:    "user-own-001",
		FileID:    "file-own-001",
		ShareType: model.ShareTypeShare,
		MaxCount:  10,
	})

	// User receives their own share - should still work (no restriction)
	newFile, err := svc.ReceiveFile("user-own-001", "share-own-001", "/self-received")
	assert.NoError(t, err)
	assert.NotNil(t, newFile)
	assert.Equal(t, "user-own-001", newFile.UserID)
	assert.Equal(t, "/self-received", newFile.ParentPath)
	assert.Equal(t, "my-file.pdf", newFile.FileName)
}

func TestRegression_ReceiveIncrements_CountCorrectly(t *testing.T) {
	svc, fileRepo, shareRepo := setupRegressionShareService()

	fileRepo.Create(&model.File{
		FileID:      "file-count-001",
		UserID:      "user-count-001",
		FileName:    "counted-file.pdf",
		FileType:    model.FileTypeFile,
		FileSize:    256,
		StoragePath: "/storage/counted-file.pdf",
		ParentPath:  "/",
		UniqueFlag:  "unique-count-001",
	})

	shareRepo.Create(&model.Share{
		ShareID:      "share-count-001",
		UserID:       "user-count-001",
		FileID:       "file-count-001",
		ShareType:    model.ShareTypeShare,
		MaxCount:     100,
		CurrentCount: 0,
	})

	// Receive 3 times
	for i := 0; i < 3; i++ {
		_, err := svc.ReceiveFile("user-recv-001", "share-count-001", "/")
		assert.NoError(t, err)
	}

	// Verify count is 3
	share, err := shareRepo.GetByShareID("share-count-001")
	assert.NoError(t, err)
	assert.Equal(t, uint32(3), share.CurrentCount)
}

func TestRegression_ExpireAtBoundary(t *testing.T) {
	svc, fileRepo, shareRepo := setupRegressionShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-boundary-001",
		UserID:   "user-boundary-001",
		FileName: "boundary-file.pdf",
	})

	// ExpireAt is exactly at current time minus a tiny amount (should be expired)
	expireAt := time.Now().Add(-1 * time.Millisecond)
	shareRepo.Create(&model.Share{
		ShareID:   "share-boundary-001",
		UserID:    "user-boundary-001",
		FileID:    "file-boundary-001",
		ShareType: model.ShareTypeShare,
		ExpireAt:  &expireAt,
	})

	_, err := svc.ReceiveFile("user-recv-001", "share-boundary-001", "/")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "share has expired")
}

func TestRegression_PasswordNotLeakedInProfile(t *testing.T) {
	userSvc, repo, mr := setupRegressionUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("secretpassword")
	repo.Create(&model.User{
		UserID:   "user-leak-001",
		Nickname: "LeakTestUser",
		Password: hashed,
	})

	user, err := userSvc.GetProfile("user-leak-001")
	assert.NoError(t, err)
	assert.Equal(t, "LeakTestUser", user.Nickname)

	// Serialize to JSON and verify password is not present
	// The User struct has `json:"-"` on Password field
	jsonBytes, err := json.Marshal(user)
	assert.NoError(t, err)

	var jsonMap map[string]interface{}
	json.Unmarshal(jsonBytes, &jsonMap)

	_, hasPassword := jsonMap["password"]
	assert.False(t, hasPassword, "password field should not be present in JSON output")

	// Also verify the raw JSON string doesn't contain the hashed password
	assert.NotContains(t, string(jsonBytes), hashed)
}

func TestRegression_NilPhoneEmail_NoConflict(t *testing.T) {
	userSvc, _, mr := setupRegressionUserService(t)
	defer mr.Close()

	// Register user 1 with phone (no email)
	codeID1 := "nil-conflict-code-1"
	code1 := "444444"
	mr.Set("verify:"+codeID1, code1)

	err := userSvc.Register("phone", "13700006666", "pass1", "User1", codeID1, code1)
	assert.NoError(t, err)

	// Register user 2 with a different phone (no email either)
	codeID2 := "nil-conflict-code-2"
	code2 := "555555"
	mr.Set("verify:"+codeID2, code2)

	err = userSvc.Register("phone", "13700007777", "pass2", "User2", codeID2, code2)
	assert.NoError(t, err)

	// Both users should exist and have nil emails without conflict
	_, user1, err := userSvc.Login("phone", "13700006666", "pass1")
	assert.NoError(t, err)
	assert.Nil(t, user1.Email)

	_, user2, err := userSvc.Login("phone", "13700007777", "pass2")
	assert.NoError(t, err)
	assert.Nil(t, user2.Email)

	// They should be different users
	assert.NotEqual(t, user1.UserID, user2.UserID)
}

func TestRegression_UpdatePassword_OldPasswordStillWorks_UntilChanged(t *testing.T) {
	userSvc, _, mr := setupRegressionUserService(t)
	defer mr.Close()

	// Register a user
	codeID := "pwd-change-code"
	code := "666666"
	mr.Set("verify:"+codeID, code)

	err := userSvc.Register("phone", "13700008888", "oldpass", "PwdUser", codeID, code)
	assert.NoError(t, err)

	// Verify old password works
	token, _, err := userSvc.Login("phone", "13700008888", "oldpass")
	assert.NoError(t, err)
	assert.NotEmpty(t, token)

	// Get user ID for password update
	_, user, _ := userSvc.Login("phone", "13700008888", "oldpass")
	userID := user.UserID

	// Change password
	err = userSvc.UpdatePassword(userID, "oldpass", "newpass")
	assert.NoError(t, err)

	// Verify old password no longer works
	_, _, err = userSvc.Login("phone", "13700008888", "oldpass")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "incorrect password")

	// Verify new password works
	token, _, err = userSvc.Login("phone", "13700008888", "newpass")
	assert.NoError(t, err)
	assert.NotEmpty(t, token)
}

func TestRegression_DeleteNonexistentFile(t *testing.T) {
	fileSvc, _ := setupRegressionFileService()

	err := fileSvc.Delete("user-del-001", "nonexistent-file-id")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "file not found")
}

func TestRegression_DownloadOtherUsersFile(t *testing.T) {
	fileSvc, fileRepo := setupRegressionFileService()

	// User A's file
	fileRepo.Create(&model.File{
		FileID:      "file-access-001",
		UserID:      "user-A",
		FileName:    "private.pdf",
		StoragePath: "/storage/private.pdf",
		ParentPath:  "/",
	})

	// User B tries to download
	_, err := fileSvc.Download("user-B", "file-access-001")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "access denied")
}
