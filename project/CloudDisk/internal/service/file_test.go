package service

import (
	"os"
	"path/filepath"
	"testing"

	"clouddisk/internal/model"
	"clouddisk/internal/repository"

	"github.com/stretchr/testify/assert"
)

func setupFileService() (*FileService, *mockFileRepo) {
	fileRepo := newMockFileRepo()
	// esRepo is nil -- only use for methods that don't call esRepo (Download, List, PublicList)
	svc := NewFileService(fileRepo, nil)
	return svc, fileRepo
}

// setupFileServiceWithES creates a FileService with a dummy ES repo so Delete won't panic.
// The ES client connects lazily; errors from unreachable ES are discarded by the caller.
func setupFileServiceWithES(t *testing.T) (*FileService, *mockFileRepo) {
	t.Helper()
	fileRepo := newMockFileRepo()
	esRepo, err := repository.NewESRepository([]string{"http://127.0.0.1:19876"}, "", "", "test-index")
	if err != nil {
		t.Fatalf("failed to create dummy ES repo: %v", err)
	}
	svc := NewFileService(fileRepo, esRepo)
	return svc, fileRepo
}

// ==================== Download Tests ====================

func TestDownload_Success(t *testing.T) {
	svc, fileRepo := setupFileService()

	fileRepo.Create(&model.File{
		FileID:      "file-001",
		UserID:      "user-123",
		FileName:    "document.pdf",
		StoragePath: "/storage/user-123/document.pdf",
	})

	file, err := svc.Download("user-123", "file-001")
	assert.NoError(t, err)
	assert.NotNil(t, file)
	assert.Equal(t, "file-001", file.FileID)
	assert.Equal(t, "user-123", file.UserID)
	assert.Equal(t, "document.pdf", file.FileName)
	assert.Equal(t, "/storage/user-123/document.pdf", file.StoragePath)
}

func TestDownload_FileNotFound(t *testing.T) {
	svc, _ := setupFileService()

	file, err := svc.Download("user-123", "nonexistent-file")
	assert.Error(t, err)
	assert.Nil(t, file)
	assert.Contains(t, err.Error(), "file not found")
}

func TestDownload_AccessDenied(t *testing.T) {
	svc, fileRepo := setupFileService()

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "private.txt",
	})

	// Different user tries to download
	file, err := svc.Download("user-999", "file-001")
	assert.Error(t, err)
	assert.Nil(t, file)
	assert.Contains(t, err.Error(), "access denied")
}

// ==================== Delete Tests ====================

func TestDelete_Success(t *testing.T) {
	svc, fileRepo := setupFileServiceWithES(t)

	// Create a temp file on disk so Delete can find it
	tmpDir := filepath.Join(os.TempDir(), "clouddisk_test_delete")
	os.MkdirAll(tmpDir, 0755)
	defer os.RemoveAll(tmpDir)

	tmpFile := filepath.Join(tmpDir, "testfile.txt")
	os.WriteFile(tmpFile, []byte("content"), 0644)

	fileRepo.Create(&model.File{
		FileID:      "file-001",
		UserID:      "user-123",
		FileName:    "testfile.txt",
		StoragePath: tmpFile,
	})

	err := svc.Delete("user-123", "file-001")
	assert.NoError(t, err)

	// Verify file is removed from repo
	_, err = fileRepo.GetByFileID("file-001")
	assert.Error(t, err)
}

func TestDelete_FileNotFound(t *testing.T) {
	svc, _ := setupFileServiceWithES(t)

	err := svc.Delete("user-123", "nonexistent-file")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "file not found")
}

func TestDelete_AccessDenied(t *testing.T) {
	svc, fileRepo := setupFileServiceWithES(t)

	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "private.txt",
	})

	// Different user tries to delete
	err := svc.Delete("user-999", "file-001")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "access denied")

	// Verify file still exists in repo
	file, err := fileRepo.GetByFileID("file-001")
	assert.NoError(t, err)
	assert.NotNil(t, file)
}

// ==================== List Tests ====================

func TestList_Success(t *testing.T) {
	svc, fileRepo := setupFileService()

	// Create multiple files for the same user and path
	for i := 0; i < 5; i++ {
		fileRepo.Create(&model.File{
			FileID:     "file-" + string(rune('A'+i)),
			UserID:     "user-123",
			FileName:   "file" + string(rune('A'+i)) + ".txt",
			ParentPath: "/docs",
		})
	}

	files, total, err := svc.List("user-123", "/docs", 1, 10)
	assert.NoError(t, err)
	assert.Equal(t, int64(5), total)
	assert.Len(t, files, 5)
}

func TestList_Pagination(t *testing.T) {
	svc, fileRepo := setupFileService()

	// Create 5 files
	for i := 0; i < 5; i++ {
		fileRepo.Create(&model.File{
			FileID:     "file-" + string(rune('A'+i)),
			UserID:     "user-123",
			FileName:   "file.txt",
			ParentPath: "/",
		})
	}

	// Page 1, size 2
	files, total, err := svc.List("user-123", "/", 1, 2)
	assert.NoError(t, err)
	assert.Equal(t, int64(5), total)
	assert.Len(t, files, 2)

	// Page 2, size 2
	files, total, err = svc.List("user-123", "/", 2, 2)
	assert.NoError(t, err)
	assert.Equal(t, int64(5), total)
	assert.Len(t, files, 2)

	// Page 3, size 2 (only 1 remaining)
	files, total, err = svc.List("user-123", "/", 3, 2)
	assert.NoError(t, err)
	assert.Equal(t, int64(5), total)
	assert.Len(t, files, 1)
}

func TestList_EmptyResult(t *testing.T) {
	svc, _ := setupFileService()

	files, total, err := svc.List("user-123", "/empty", 1, 10)
	assert.NoError(t, err)
	assert.Equal(t, int64(0), total)
	assert.Empty(t, files)
}

func TestList_PageBeyondTotal(t *testing.T) {
	svc, fileRepo := setupFileService()

	fileRepo.Create(&model.File{
		FileID:     "file-001",
		UserID:     "user-123",
		FileName:   "only.txt",
		ParentPath: "/",
	})

	// Page 100 with only 1 file
	files, total, err := svc.List("user-123", "/", 100, 10)
	assert.NoError(t, err)
	assert.Equal(t, int64(1), total)
	assert.Empty(t, files)
}

// ==================== PublicList Tests ====================

func TestPublicList_Success(t *testing.T) {
	svc, fileRepo := setupFileService()

	// Create public and non-public files
	fileRepo.Create(&model.File{
		FileID:   "file-pub-1",
		UserID:   "user-123",
		FileName: "public1.txt",
		IsPublic: true,
	})
	fileRepo.Create(&model.File{
		FileID:   "file-pub-2",
		UserID:   "user-456",
		FileName: "public2.txt",
		IsPublic: true,
	})
	fileRepo.Create(&model.File{
		FileID:   "file-priv-1",
		UserID:   "user-123",
		FileName: "private.txt",
		IsPublic: false,
	})

	files, total, err := svc.PublicList(1, 10)
	assert.NoError(t, err)
	assert.Equal(t, int64(2), total)
	assert.Len(t, files, 2)
}

func TestPublicList_Empty(t *testing.T) {
	svc, fileRepo := setupFileService()

	// Only non-public files
	fileRepo.Create(&model.File{
		FileID:   "file-001",
		UserID:   "user-123",
		FileName: "private.txt",
		IsPublic: false,
	})

	files, total, err := svc.PublicList(1, 10)
	assert.NoError(t, err)
	assert.Equal(t, int64(0), total)
	assert.Empty(t, files)
}

func TestPublicList_Pagination(t *testing.T) {
	svc, fileRepo := setupFileService()

	for i := 0; i < 5; i++ {
		fileRepo.Create(&model.File{
			FileID:   "file-pub-" + string(rune('A'+i)),
			UserID:   "user-123",
			FileName: "public.txt",
			IsPublic: true,
		})
	}

	// Page 1, size 3
	files, total, err := svc.PublicList(1, 3)
	assert.NoError(t, err)
	assert.Equal(t, int64(5), total)
	assert.Len(t, files, 3)

	// Page 2, size 3
	files, total, err = svc.PublicList(2, 3)
	assert.NoError(t, err)
	assert.Equal(t, int64(5), total)
	assert.Len(t, files, 2)
}
