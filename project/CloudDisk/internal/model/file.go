package model

import "time"

// FileType constants
const (
	FileTypeFile     uint8 = 0
	FileTypeFolder   uint8 = 1
	FileTypeImage    uint8 = 2
	FileTypeAudio    uint8 = 3
	FileTypeVideo    uint8 = 4
	FileTypeCompress uint8 = 5
	FileTypeOther    uint8 = 6
)

// Source constants
const (
	SourceOwn    uint8 = 0
	SourceOthers uint8 = 1
)

type File struct {
	ID           uint64    `gorm:"primaryKey;autoIncrement" json:"-"`
	FileID       string    `gorm:"column:file_id;uniqueIndex;size:64" json:"file_id"`
	UserID       string    `gorm:"column:user_id;index;size:64" json:"user_id"`
	FileName     string    `gorm:"column:file_name;size:512" json:"file_name"`
	FileType     uint8     `gorm:"column:file_type" json:"file_type"`
	FileSize     uint64    `gorm:"column:file_size" json:"file_size"`
	StoragePath  string    `gorm:"column:storage_path;size:1024" json:"-"`
	ParentPath   string    `gorm:"column:parent_path;size:1024" json:"parent_path"`
	Source       uint8     `json:"source"`
	SourceUserID string    `gorm:"column:source_user_id;size:64" json:"source_user_id"`
	UniqueFlag   string    `gorm:"column:unique_flag;index;size:128" json:"-"`
	IsShared     bool      `gorm:"column:is_shared" json:"is_shared"`
	IsPublic     bool      `gorm:"column:is_public;index" json:"is_public"`
	CreatedAt    time.Time `json:"created_at"`
	UpdatedAt    time.Time `json:"updated_at"`
}

func (File) TableName() string {
	return "files"
}
