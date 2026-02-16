package service

import (
	"errors"
	"fmt"
	"mime/multipart"
	"path/filepath"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"
	"clouddisk/internal/repository"

	"gorm.io/gorm"
)

type FileService struct {
	fileRepo repository.FileRepository
	esRepo   *repository.ESRepository
}

func NewFileService(fileRepo repository.FileRepository, esRepo *repository.ESRepository) *FileService {
	return &FileService{fileRepo: fileRepo, esRepo: esRepo}
}

func (s *FileService) Upload(userID string, fileHeader *multipart.FileHeader, parentPath, uniqueFlag string, cfg *config.Config) (*model.File, error) {
	// Validate path safety
	if err := pkg.ValidatePath(parentPath); err != nil {
		return nil, fmt.Errorf("invalid parent path: %w", err)
	}

	// Sanitize filename and check blocked extensions
	fileHeader.Filename = pkg.SanitizeFileName(fileHeader.Filename)
	if pkg.IsBlockedExtension(fileHeader.Filename) {
		return nil, errors.New("file type not allowed")
	}

	// Check uniqueFlag for dedup (秒传)
	if uniqueFlag != "" {
		existing, err := s.fileRepo.GetByUniqueFlag(uniqueFlag)
		if err == nil && existing != nil {
			// Same file already exists, create new DB record pointing to same storage
			newFile := &model.File{
				FileID:      pkg.NewUUID(),
				UserID:      userID,
				FileName:    fileHeader.Filename,
				FileType:    pkg.DetectFileType(fileHeader.Filename),
				FileSize:    uint64(fileHeader.Size),
				StoragePath: existing.StoragePath,
				ParentPath:  parentPath,
				Source:      model.SourceOwn,
				UniqueFlag:  uniqueFlag,
			}
			if err := s.fileRepo.Create(newFile); err != nil {
				return nil, fmt.Errorf("failed to create file record: %w", err)
			}
			// Index in ES
			_ = s.esRepo.IndexFile(newFile)
			return newFile, nil
		}
	}

	// Save file to disk
	ext := filepath.Ext(fileHeader.Filename)
	storageName := pkg.NewUUID() + ext
	storagePath := filepath.Join(cfg.Server.UploadDir, userID, storageName)

	if err := pkg.SaveUploadedFile(fileHeader, storagePath); err != nil {
		return nil, fmt.Errorf("failed to save file: %w", err)
	}

	// Create DB record
	file := &model.File{
		FileID:      pkg.NewUUID(),
		UserID:      userID,
		FileName:    fileHeader.Filename,
		FileType:    pkg.DetectFileType(fileHeader.Filename),
		FileSize:    uint64(fileHeader.Size),
		StoragePath: storagePath,
		ParentPath:  parentPath,
		Source:      model.SourceOwn,
		UniqueFlag:  uniqueFlag,
	}

	if err := s.fileRepo.Create(file); err != nil {
		// Clean up saved file on DB error
		_ = pkg.DeleteFile(storagePath)
		return nil, fmt.Errorf("failed to create file record: %w", err)
	}

	// Index in ES
	_ = s.esRepo.IndexFile(file)

	return file, nil
}

func (s *FileService) Download(userID, fileID string) (*model.File, error) {
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

	return file, nil
}

func (s *FileService) Delete(userID, fileID string) error {
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

	// If folder, delete all children first
	if file.FileType == model.FileTypeFolder {
		childPrefix := file.ParentPath
		if childPrefix == "/" {
			childPrefix = "/" + file.FileName + "/"
		} else {
			childPrefix = childPrefix + file.FileName + "/"
		}
		children, _ := s.fileRepo.ListByUserAndPathPrefix(userID, childPrefix)
		for _, child := range children {
			if pkg.FileExists(child.StoragePath) {
				_ = pkg.DeleteFile(child.StoragePath)
			}
			_ = s.fileRepo.Delete(child.FileID)
			_ = s.esRepo.DeleteFile(child.FileID)
		}
	}

	// Delete from disk
	if pkg.FileExists(file.StoragePath) {
		_ = pkg.DeleteFile(file.StoragePath)
	}

	// Delete from DB
	if err := s.fileRepo.Delete(fileID); err != nil {
		return fmt.Errorf("failed to delete file record: %w", err)
	}

	// Delete from ES
	_ = s.esRepo.DeleteFile(fileID)

	return nil
}

func (s *FileService) List(userID, parentPath string, page, size int, filter *repository.FileFilter) ([]model.File, int64, error) {
	offset := (page - 1) * size
	return s.fileRepo.ListByUserAndPath(userID, parentPath, offset, size, filter)
}

func (s *FileService) PublicList(page, size int) ([]model.File, int64, error) {
	offset := (page - 1) * size
	return s.fileRepo.ListPublic(offset, size)
}

func (s *FileService) Search(keyword string, page, size int) ([]model.File, int64, error) {
	offset := (page - 1) * size

	fileIDs, total, err := s.esRepo.SearchFiles(keyword, offset, size)
	if err != nil {
		return nil, 0, fmt.Errorf("failed to search files: %w", err)
	}

	if len(fileIDs) == 0 {
		return []model.File{}, 0, nil
	}

	var files []model.File
	for _, fid := range fileIDs {
		file, err := s.fileRepo.GetByFileID(fid)
		if err != nil {
			continue
		}
		files = append(files, *file)
	}

	return files, int64(total), nil
}

func (s *FileService) CreateFolder(userID, folderName, parentPath string) (*model.File, error) {
	// Validate path safety
	if err := pkg.ValidatePath(parentPath); err != nil {
		return nil, fmt.Errorf("invalid parent path: %w", err)
	}
	// Sanitize folder name
	folderName = pkg.SanitizeFileName(folderName)

	folder := &model.File{
		FileID:     pkg.NewUUID(),
		UserID:     userID,
		FileName:   folderName,
		FileType:   model.FileTypeFolder,
		ParentPath: parentPath,
		Source:     model.SourceOwn,
	}
	if err := s.fileRepo.Create(folder); err != nil {
		return nil, fmt.Errorf("failed to create folder: %w", err)
	}
	return folder, nil
}

func (s *FileService) GetFolderChildren(userID string, folder *model.File) ([]model.File, string, error) {
	prefix := folder.ParentPath
	if prefix == "/" {
		prefix = "/" + folder.FileName + "/"
	} else {
		prefix = prefix + folder.FileName + "/"
	}
	children, err := s.fileRepo.ListByUserAndPathPrefix(userID, prefix)
	if err != nil {
		return nil, prefix, err
	}
	return children, prefix, nil
}

func (s *FileService) GetFilesByIDs(userID string, fileIDs []string) ([]model.File, error) {
	files, err := s.fileRepo.GetByFileIDs(fileIDs)
	if err != nil {
		return nil, err
	}
	// Filter to only user's own files
	var result []model.File
	for _, f := range files {
		if f.UserID == userID {
			result = append(result, f)
		}
	}
	return result, nil
}
