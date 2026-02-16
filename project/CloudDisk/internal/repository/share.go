package repository

import (
	"clouddisk/internal/model"

	"gorm.io/gorm"
)

type ShareRepository interface {
	Create(share *model.Share) error
	GetByShareID(shareID string) (*model.Share, error)
	GetByFileIDAndType(fileID string, shareType uint8) (*model.Share, error)
	GetByFileIDs(fileIDs []string) ([]model.Share, error)
	DeleteByFileIDAndType(fileID string, shareType uint8) error
	IncrementCount(shareID string) error
	DeleteByFileIDs(fileIDs []string) error
}

type shareRepository struct {
	db *gorm.DB
}

func NewShareRepository(db *gorm.DB) ShareRepository {
	return &shareRepository{db: db}
}

func (r *shareRepository) Create(share *model.Share) error {
	return r.db.Create(share).Error
}

func (r *shareRepository) GetByShareID(shareID string) (*model.Share, error) {
	var share model.Share
	if err := r.db.Where("share_id = ?", shareID).First(&share).Error; err != nil {
		return nil, err
	}
	return &share, nil
}

func (r *shareRepository) GetByFileIDAndType(fileID string, shareType uint8) (*model.Share, error) {
	var share model.Share
	if err := r.db.Where("file_id = ? AND share_type = ?", fileID, shareType).First(&share).Error; err != nil {
		return nil, err
	}
	return &share, nil
}

func (r *shareRepository) GetByFileIDs(fileIDs []string) ([]model.Share, error) {
	var shares []model.Share
	if len(fileIDs) == 0 {
		return shares, nil
	}
	if err := r.db.Where("file_id IN ?", fileIDs).Find(&shares).Error; err != nil {
		return nil, err
	}
	return shares, nil
}

func (r *shareRepository) DeleteByFileIDAndType(fileID string, shareType uint8) error {
	return r.db.Where("file_id = ? AND share_type = ?", fileID, shareType).Delete(&model.Share{}).Error
}

func (r *shareRepository) IncrementCount(shareID string) error {
	return r.db.Model(&model.Share{}).Where("share_id = ?", shareID).
		UpdateColumn("current_count", gorm.Expr("current_count + 1")).Error
}

func (r *shareRepository) DeleteByFileIDs(fileIDs []string) error {
	if len(fileIDs) == 0 {
		return nil
	}
	return r.db.Where("file_id IN ?", fileIDs).Delete(&model.Share{}).Error
}
