package model

import "time"

type User struct {
	ID          uint64    `gorm:"primaryKey;autoIncrement" json:"-"`
	UserID      string    `gorm:"column:user_id;uniqueIndex;size:64" json:"user_id"`
	Nickname    string    `gorm:"size:128" json:"nickname"`
	Description string    `gorm:"size:512" json:"description"`
	Phone       *string   `gorm:"uniqueIndex;size:20" json:"phone"`
	Email       *string   `gorm:"uniqueIndex;size:128" json:"email"`
	Password    string    `gorm:"size:256" json:"-"`
	AvatarURL   string    `gorm:"column:avatar_url;size:512" json:"avatar_url"`
	CreatedAt   time.Time `json:"created_at"`
	UpdatedAt   time.Time `json:"updated_at"`
}

func (User) TableName() string {
	return "users"
}
