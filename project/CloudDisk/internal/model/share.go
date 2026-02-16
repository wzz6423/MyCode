package model

import "time"

// ShareType constants
const (
	ShareTypeShare  uint8 = 0
	ShareTypePublic uint8 = 2
)

type Share struct {
	ID           uint64     `gorm:"primaryKey;autoIncrement" json:"-"`
	ShareID      string     `gorm:"column:share_id;uniqueIndex;size:64" json:"share_id"`
	UserID       string     `gorm:"column:user_id;index;size:64" json:"user_id"`
	FileID       string     `gorm:"column:file_id;index;size:64" json:"file_id"`
	ShareType    uint8      `gorm:"column:share_type" json:"share_type"`
	Message      string     `gorm:"size:512" json:"message"`
	ExpireAt     *time.Time `gorm:"column:expire_at" json:"expire_at"`
	MaxCount     uint32     `gorm:"column:max_count" json:"max_count"`
	CurrentCount uint32     `gorm:"column:current_count" json:"current_count"`
	CreatedAt    time.Time  `json:"created_at"`
}

func (Share) TableName() string {
	return "shares"
}
