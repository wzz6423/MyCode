package repository

import (
	"strings"
	"time"

	"clouddisk/internal/model"

	"gorm.io/gorm"
)

type FileFilter struct {
	Status    string     // "shared", "public", "" (all)
	StartTime *time.Time
	EndTime   *time.Time
}

type FileRepository interface {
	Create(file *model.File) error
	GetByFileID(fileID string) (*model.File, error)
	GetByUniqueFlag(uniqueFlag string) (*model.File, error)
	ListByUserAndPath(userID, parentPath string, offset, limit int, filter *FileFilter) ([]model.File, int64, error)
	ListByUserAndPathPrefix(userID, pathPrefix string) ([]model.File, error)
	ListPublic(offset, limit int) ([]model.File, int64, error)
	Update(file *model.File) error
	Delete(fileID string) error
	ListByUserID(userID string) ([]model.File, error)
	DeleteByUserID(userID string) error
	GetByFileIDs(fileIDs []string) ([]model.File, error)
}

type fileRepository struct {
	db *gorm.DB
}

func NewFileRepository(db *gorm.DB) FileRepository {
	return &fileRepository{db: db}
}

func (r *fileRepository) Create(file *model.File) error {
	return r.db.Create(file).Error
}

func (r *fileRepository) GetByFileID(fileID string) (*model.File, error) {
	var file model.File
	if err := r.db.Where("file_id = ?", fileID).First(&file).Error; err != nil {
		return nil, err
	}
	return &file, nil
}

func (r *fileRepository) GetByUniqueFlag(uniqueFlag string) (*model.File, error) {
	var file model.File
	if err := r.db.Where("unique_flag = ?", uniqueFlag).First(&file).Error; err != nil {
		return nil, err
	}
	return &file, nil
}

func (r *fileRepository) ListByUserAndPath(userID, parentPath string, offset, limit int, filter *FileFilter) ([]model.File, int64, error) {
	var files []model.File
	var total int64

	query := r.db.Where("user_id = ? AND parent_path = ?", userID, parentPath)

	if filter != nil {
		if filter.Status == "shared" {
			query = query.Where("is_shared = ?", true)
		} else if filter.Status == "public" {
			query = query.Where("is_public = ?", true)
		}
		if filter.StartTime != nil {
			query = query.Where("created_at >= ?", *filter.StartTime)
		}
		if filter.EndTime != nil {
			query = query.Where("created_at <= ?", *filter.EndTime)
		}
	}

	if err := query.Model(&model.File{}).Count(&total).Error; err != nil {
		return nil, 0, err
	}

	if err := query.Offset(offset).Limit(limit).Order("created_at DESC").Find(&files).Error; err != nil {
		return nil, 0, err
	}

	return files, total, nil
}

func (r *fileRepository) ListPublic(offset, limit int) ([]model.File, int64, error) {
	var files []model.File
	var total int64

	query := r.db.Where("is_public = ?", true)

	if err := query.Model(&model.File{}).Count(&total).Error; err != nil {
		return nil, 0, err
	}

	if err := query.Offset(offset).Limit(limit).Order("created_at DESC").Find(&files).Error; err != nil {
		return nil, 0, err
	}

	return files, total, nil
}

func (r *fileRepository) Update(file *model.File) error {
	return r.db.Save(file).Error
}

func (r *fileRepository) Delete(fileID string) error {
	return r.db.Where("file_id = ?", fileID).Delete(&model.File{}).Error
}

func (r *fileRepository) ListByUserID(userID string) ([]model.File, error) {
	var files []model.File
	if err := r.db.Where("user_id = ?", userID).Find(&files).Error; err != nil {
		return nil, err
	}
	return files, nil
}

func (r *fileRepository) DeleteByUserID(userID string) error {
	return r.db.Where("user_id = ?", userID).Delete(&model.File{}).Error
}

func (r *fileRepository) GetByFileIDs(fileIDs []string) ([]model.File, error) {
	var files []model.File
	if len(fileIDs) == 0 {
		return files, nil
	}
	if err := r.db.Where("file_id IN ?", fileIDs).Find(&files).Error; err != nil {
		return nil, err
	}
	return files, nil
}

func (r *fileRepository) ListByUserAndPathPrefix(userID, pathPrefix string) ([]model.File, error) {
	var files []model.File
	escaped := strings.ReplaceAll(pathPrefix, "\\", "\\\\")
	escaped = strings.ReplaceAll(escaped, "%", "\\%")
	escaped = strings.ReplaceAll(escaped, "_", "\\_")
	if err := r.db.Where("user_id = ? AND parent_path LIKE ?", userID, escaped+"%").Find(&files).Error; err != nil {
		return nil, err
	}
	return files, nil
}
