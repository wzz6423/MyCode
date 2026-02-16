package service

import (
	"errors"
	"fmt"
	"math"
	"path/filepath"
	"strings"
	"time"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"
	"clouddisk/internal/repository"

	"gorm.io/gorm"
)

type ShareService struct {
	fileRepo  repository.FileRepository
	shareRepo repository.ShareRepository
	cfg       *config.Config
}

func NewShareService(fileRepo repository.FileRepository, shareRepo repository.ShareRepository, cfg *config.Config) *ShareService {
	return &ShareService{fileRepo: fileRepo, shareRepo: shareRepo, cfg: cfg}
}

func (s *ShareService) ShareFile(userID, fileID, message string, expireHours, maxCount int) (*model.Share, error) {
	// Verify file ownership
	file, err := s.fileRepo.GetByFileID(fileID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return nil, errors.New("file not found")
		}
		return nil, fmt.Errorf("failed to get file: %w", err)
	}
	if file.UserID != userID {
		return nil, errors.New("access denied")
	}

	// Create share record
	mc := uint32(maxCount)
	if maxCount < 0 {
		mc = math.MaxUint32
	}
	share := &model.Share{
		ShareID:   pkg.NewUUID(),
		UserID:    userID,
		FileID:    fileID,
		ShareType: model.ShareTypeShare,
		Message:   message,
		MaxCount:  mc,
	}

	if expireHours > 0 {
		expireAt := time.Now().Add(time.Duration(expireHours) * time.Hour)
		share.ExpireAt = &expireAt
	}

	if err := s.shareRepo.Create(share); err != nil {
		return nil, fmt.Errorf("failed to create share record: %w", err)
	}

	// Update file shared status
	file.IsShared = true
	if err := s.fileRepo.Update(file); err != nil {
		return nil, fmt.Errorf("failed to update file: %w", err)
	}

	// If folder, also share all children
	if file.FileType == model.FileTypeFolder {
		s.setChildrenShared(userID, file, true)
	}

	return share, nil
}

func (s *ShareService) UnshareFile(userID, fileID string) error {
	// Verify file ownership
	file, err := s.fileRepo.GetByFileID(fileID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return errors.New("file not found")
		}
		return fmt.Errorf("failed to get file: %w", err)
	}
	if file.UserID != userID {
		return errors.New("access denied")
	}

	// Delete share record
	if err := s.shareRepo.DeleteByFileIDAndType(fileID, model.ShareTypeShare); err != nil {
		return fmt.Errorf("failed to delete share record: %w", err)
	}

	// Update file shared status
	file.IsShared = false
	if err := s.fileRepo.Update(file); err != nil {
		return fmt.Errorf("failed to update file: %w", err)
	}

	// If folder, also unshare all children
	if file.FileType == model.FileTypeFolder {
		s.setChildrenShared(userID, file, false)
	}

	return nil
}

func (s *ShareService) PublicFile(userID, fileID string, expireHours, maxCount int) (*model.Share, error) {
	// Verify file ownership
	file, err := s.fileRepo.GetByFileID(fileID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return nil, errors.New("file not found")
		}
		return nil, fmt.Errorf("failed to get file: %w", err)
	}
	if file.UserID != userID {
		return nil, errors.New("access denied")
	}

	// Create public share record
	mc := uint32(maxCount)
	if maxCount < 0 {
		mc = math.MaxUint32
	}
	share := &model.Share{
		ShareID:   pkg.NewUUID(),
		UserID:    userID,
		FileID:    fileID,
		ShareType: model.ShareTypePublic,
		MaxCount:  mc,
	}

	if expireHours > 0 {
		expireAt := time.Now().Add(time.Duration(expireHours) * time.Hour)
		share.ExpireAt = &expireAt
	}

	if err := s.shareRepo.Create(share); err != nil {
		return nil, fmt.Errorf("failed to create public share record: %w", err)
	}

	// Update file public status
	file.IsPublic = true
	if err := s.fileRepo.Update(file); err != nil {
		return nil, fmt.Errorf("failed to update file: %w", err)
	}

	return share, nil
}

func (s *ShareService) UnpublicFile(userID, fileID string) error {
	// Verify file ownership
	file, err := s.fileRepo.GetByFileID(fileID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return errors.New("file not found")
		}
		return fmt.Errorf("failed to get file: %w", err)
	}
	if file.UserID != userID {
		return errors.New("access denied")
	}

	// Delete public share record
	if err := s.shareRepo.DeleteByFileIDAndType(fileID, model.ShareTypePublic); err != nil {
		return fmt.Errorf("failed to delete public share record: %w", err)
	}

	// Update file public status
	file.IsPublic = false
	if err := s.fileRepo.Update(file); err != nil {
		return fmt.Errorf("failed to update file: %w", err)
	}

	return nil
}

// folderChildPath returns the parent_path prefix for children of a folder.
func folderChildPath(file *model.File) string {
	p := file.ParentPath
	if p == "/" {
		return "/" + file.FileName + "/"
	}
	return p + file.FileName + "/"
}

func (s *ShareService) setChildrenShared(userID string, folder *model.File, shared bool) {
	children, err := s.fileRepo.ListByUserAndPathPrefix(userID, folderChildPath(folder))
	if err != nil || len(children) == 0 {
		return
	}
	for i := range children {
		children[i].IsShared = shared
		_ = s.fileRepo.Update(&children[i])
		if !shared {
			_ = s.shareRepo.DeleteByFileIDAndType(children[i].FileID, model.ShareTypeShare)
		}
	}
}

func (s *ShareService) GetShareInfo(shareID string) (map[string]interface{}, error) {
	share, err := s.shareRepo.GetByShareID(shareID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return nil, errors.New("share not found")
		}
		return nil, fmt.Errorf("failed to get share: %w", err)
	}

	// Check expiry
	if share.ExpireAt != nil && share.ExpireAt.Before(time.Now()) {
		return nil, errors.New("share has expired")
	}

	// Check count limit
	if share.MaxCount != math.MaxUint32 && share.CurrentCount >= share.MaxCount {
		return nil, errors.New("share has reached maximum receive count")
	}

	// Get file info
	file, err := s.fileRepo.GetByFileID(share.FileID)
	if err != nil {
		return nil, fmt.Errorf("failed to get file: %w", err)
	}

	return map[string]interface{}{
		"share_id":   share.ShareID,
		"share_type": share.ShareType,
		"user_id":    share.UserID,
		"message":    share.Message,
		"file_name":  file.FileName,
		"file_size":  file.FileSize,
		"expire_at":  share.ExpireAt,
		"created_at": share.CreatedAt,
	}, nil
}

func (s *ShareService) ReceiveFile(userID, shareID, savePath, fileName string) (*model.File, error) {
	// Validate save path
	if err := pkg.ValidatePath(savePath); err != nil {
		return nil, fmt.Errorf("invalid save path: %w", err)
	}
	// Sanitize filename if provided
	if fileName != "" {
		fileName = pkg.SanitizeFileName(fileName)
		if pkg.IsBlockedExtension(fileName) {
			return nil, errors.New("file type not allowed")
		}
	}

	// Get share record
	share, err := s.shareRepo.GetByShareID(shareID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return nil, errors.New("share not found")
		}
		return nil, fmt.Errorf("failed to get share: %w", err)
	}

	// Check expiry
	if share.ExpireAt != nil && share.ExpireAt.Before(time.Now()) {
		return nil, errors.New("share has expired")
	}

	// Check count limit (MaxUint32 means unlimited)
	if share.MaxCount != math.MaxUint32 && share.CurrentCount >= share.MaxCount {
		return nil, errors.New("share has reached maximum receive count")
	}

	// Get original file
	originalFile, err := s.fileRepo.GetByFileID(share.FileID)
	if err != nil {
		return nil, fmt.Errorf("failed to get original file: %w", err)
	}

	// Copy file record for receiving user
	saveName := originalFile.FileName
	if fileName != "" {
		saveName = fileName
	}

	// Copy physical file to receiver's storage directory
	newStoragePath := ""
	if originalFile.FileType != model.FileTypeFolder && originalFile.StoragePath != "" {
		ext := filepath.Ext(originalFile.StoragePath)
		newStorageName := pkg.NewUUID() + ext
		newStoragePath = filepath.Join(s.cfg.Server.UploadDir, userID, newStorageName)
		if err := pkg.CopyFile(originalFile.StoragePath, newStoragePath); err != nil {
			return nil, fmt.Errorf("failed to copy file: %w", err)
		}
	}

	newFile := &model.File{
		FileID:       pkg.NewUUID(),
		UserID:       userID,
		FileName:     saveName,
		FileType:     originalFile.FileType,
		FileSize:     originalFile.FileSize,
		StoragePath:  newStoragePath,
		ParentPath:   savePath,
		Source:       model.SourceOthers,
		SourceUserID: originalFile.UserID,
		UniqueFlag:   originalFile.UniqueFlag,
	}

	if err := s.fileRepo.Create(newFile); err != nil {
		if newStoragePath != "" {
			_ = pkg.DeleteFile(newStoragePath)
		}
		return nil, fmt.Errorf("failed to create file record: %w", err)
	}

	// If folder, also copy all children with remapped paths and copied files
	if originalFile.FileType == model.FileTypeFolder {
		oldPrefix := folderChildPath(originalFile)
		newPrefix := savePath
		if newPrefix == "/" {
			newPrefix = "/" + saveName + "/"
		} else {
			newPrefix = newPrefix + saveName + "/"
		}
		children, _ := s.fileRepo.ListByUserAndPathPrefix(originalFile.UserID, oldPrefix)
		for _, child := range children {
			newParent := strings.Replace(child.ParentPath, oldPrefix, newPrefix, 1)
			childStoragePath := ""
			if child.FileType != model.FileTypeFolder && child.StoragePath != "" {
				ext := filepath.Ext(child.StoragePath)
				childStoragePath = filepath.Join(s.cfg.Server.UploadDir, userID, pkg.NewUUID()+ext)
				if err := pkg.CopyFile(child.StoragePath, childStoragePath); err != nil {
					continue
				}
			}
			childCopy := &model.File{
				FileID:       pkg.NewUUID(),
				UserID:       userID,
				FileName:     child.FileName,
				FileType:     child.FileType,
				FileSize:     child.FileSize,
				StoragePath:  childStoragePath,
				ParentPath:   newParent,
				Source:       model.SourceOthers,
				SourceUserID: originalFile.UserID,
				UniqueFlag:   child.UniqueFlag,
			}
			_ = s.fileRepo.Create(childCopy)
		}
	}

	// Increment share count
	if err := s.shareRepo.IncrementCount(shareID); err != nil {
		return nil, fmt.Errorf("failed to increment share count: %w", err)
	}

	return newFile, nil
}
