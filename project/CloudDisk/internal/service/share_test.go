package service

import (
	"math"
	"testing"
	"time"

	"clouddisk/internal/model"

	"github.com/stretchr/testify/assert"
)

func setupShareService() (*ShareService, *mockFileRepo, *mockShareRepo) {
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	svc := NewShareService(fileRepo, shareRepo)
	return svc, fileRepo, shareRepo
}

func TestShareFile_Success(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	// Create a file owned by the user
	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	share, err := svc.ShareFile("user-123", "file-001", "Check this out", 24, 10)
	assert.NoError(t, err)
	assert.NotNil(t, share)
	assert.NotEmpty(t, share.ShareID)
	assert.Equal(t, "user-123", share.UserID)
	assert.Equal(t, "file-001", share.FileID)
	assert.Equal(t, model.ShareTypeShare, share.ShareType)
	assert.Equal(t, "Check this out", share.Message)
	assert.Equal(t, uint32(10), share.MaxCount)
	assert.NotNil(t, share.ExpireAt)

	// Verify file is marked as shared
	file, _ := fileRepo.GetByFileID("file-001")
	assert.True(t, file.IsShared)

	// Verify share record exists in repo
	stored, err := shareRepo.GetByShareID(share.ShareID)
	assert.NoError(t, err)
	assert.Equal(t, share.ShareID, stored.ShareID)
}

func TestShareFile_NoExpiry(t *testing.T) {
	svc, fileRepo, _ := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	share, err := svc.ShareFile("user-123", "file-001", "No expiry", 0, -1)
	assert.NoError(t, err)
	assert.Nil(t, share.ExpireAt)
	assert.Equal(t, uint32(math.MaxUint32), share.MaxCount)
}

func TestShareFile_NotOwner(t *testing.T) {
	svc, fileRepo, _ := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	// Different user tries to share
	_, err := svc.ShareFile("user-999", "file-001", "Steal this", 24, 10)
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "access denied")
}

func TestShareFile_FileNotFound(t *testing.T) {
	svc, _, _ := setupShareService()

	_, err := svc.ShareFile("user-123", "nonexistent-file", "msg", 24, 10)
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "file not found")
}

func TestUnshareFile_Success(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
		IsShared: true,
	})

	shareRepo.Create(&model.Share{
		ShareID:   "share-001",
		UserID:    "user-123",
		FileID:    "file-001",
		ShareType: model.ShareTypeShare,
	})

	err := svc.UnshareFile("user-123", "file-001")
	assert.NoError(t, err)

	// Verify file is no longer shared
	file, _ := fileRepo.GetByFileID("file-001")
	assert.False(t, file.IsShared)

	// Verify share record is deleted
	_, err = shareRepo.GetByFileIDAndType("file-001", model.ShareTypeShare)
	assert.Error(t, err)
}

func TestUnshareFile_NotOwner(t *testing.T) {
	svc, fileRepo, _ := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
		IsShared: true,
	})

	err := svc.UnshareFile("user-999", "file-001")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "access denied")
}

func TestPublicFile_Success(t *testing.T) {
	svc, fileRepo, _ := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	share, err := svc.PublicFile("user-123", "file-001", 48, 100)
	assert.NoError(t, err)
	assert.NotNil(t, share)
	assert.Equal(t, model.ShareTypePublic, share.ShareType)
	assert.NotNil(t, share.ExpireAt)
	assert.Equal(t, uint32(100), share.MaxCount)

	// Verify file is marked as public
	file, _ := fileRepo.GetByFileID("file-001")
	assert.True(t, file.IsPublic)
}

func TestUnpublicFile_Success(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
		IsPublic: true,
	})

	shareRepo.Create(&model.Share{
		ShareID:   "share-pub-001",
		UserID:    "user-123",
		FileID:    "file-001",
		ShareType: model.ShareTypePublic,
	})

	err := svc.UnpublicFile("user-123", "file-001")
	assert.NoError(t, err)

	file, _ := fileRepo.GetByFileID("file-001")
	assert.False(t, file.IsPublic)
}

func TestReceiveFile_Success(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	// Create original file
	fileRepo.Create(&model.File{
		FileID:      "file-001",
		UserID:      "user-123",
		FileName:    "shared-doc.pdf",
		FileType:    model.FileTypeFile,
		FileSize:    1024,
		StoragePath: "/storage/user-123/doc.pdf",
		ParentPath:  "/",
		UniqueFlag:  "abc123",
	})

	// Create share record
	shareRepo.Create(&model.Share{
		ShareID:      "share-001",
		UserID:       "user-123",
		FileID:       "file-001",
		ShareType:    model.ShareTypeShare,
		MaxCount:     10,
		CurrentCount: 0,
	})

	// Another user receives the file
	newFile, err := svc.ReceiveFile("user-456", "share-001", "/received")
	assert.NoError(t, err)
	assert.NotNil(t, newFile)
	assert.NotEqual(t, "file-001", newFile.FileID) // New file ID
	assert.Equal(t, "user-456", newFile.UserID)
	assert.Equal(t, "shared-doc.pdf", newFile.FileName)
	assert.Equal(t, "/storage/user-123/doc.pdf", newFile.StoragePath) // Same storage
	assert.Equal(t, "/received", newFile.ParentPath)
	assert.Equal(t, model.SourceOthers, newFile.Source)
	assert.Equal(t, "user-123", newFile.SourceUserID)

	// Verify share count incremented
	share, _ := shareRepo.GetByShareID("share-001")
	assert.Equal(t, uint32(1), share.CurrentCount)
}

func TestReceiveFile_Expired(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	// Create an expired share
	expiredTime := time.Now().Add(-1 * time.Hour)
	shareRepo.Create(&model.Share{
		ShareID:   "share-expired",
		UserID:    "user-123",
		FileID:    "file-001",
		ShareType: model.ShareTypeShare,
		ExpireAt:  &expiredTime,
	})

	_, err := svc.ReceiveFile("user-456", "share-expired", "/")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "share has expired")
}

func TestReceiveFile_MaxCountReached(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "test.txt",
	})

	// Create a share that has reached max count
	shareRepo.Create(&model.Share{
		ShareID:      "share-maxed",
		UserID:       "user-123",
		FileID:       "file-001",
		ShareType:    model.ShareTypeShare,
		MaxCount:     5,
		CurrentCount: 5,
	})

	_, err := svc.ReceiveFile("user-456", "share-maxed", "/")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "share has reached maximum receive count")
}

func TestReceiveFile_ShareNotFound(t *testing.T) {
	svc, _, _ := setupShareService()

	_, err := svc.ReceiveFile("user-456", "nonexistent-share", "/")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "share not found")
}

func TestReceiveFile_UnlimitedCount(t *testing.T) {
	svc, fileRepo, shareRepo := setupShareService()

	fileRepo.Create(&model.File{
		FileID:      "file-001",
		UserID:      "user-123",
		FileName:    "test.txt",
		StoragePath: "/storage/test.txt",
	})

	// MaxCount = MaxUint32 means unlimited
	shareRepo.Create(&model.Share{
		ShareID:      "share-unlimited",
		UserID:       "user-123",
		FileID:       "file-001",
		ShareType:    model.ShareTypeShare,
		MaxCount:     math.MaxUint32,
		CurrentCount: 999,
	})

	newFile, err := svc.ReceiveFile("user-456", "share-unlimited", "/")
	assert.NoError(t, err)
	assert.NotNil(t, newFile)
}
