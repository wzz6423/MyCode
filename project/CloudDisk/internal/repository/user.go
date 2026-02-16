package repository

import (
	"clouddisk/internal/model"

	"gorm.io/gorm"
)

type UserRepository interface {
	Create(user *model.User) error
	GetByUserID(userID string) (*model.User, error)
	GetByPhone(phone string) (*model.User, error)
	GetByEmail(email string) (*model.User, error)
	GetByNickname(nickname string) (*model.User, error)
	GetByUserIDs(userIDs []string) ([]model.User, error)
	Update(user *model.User) error
	Delete(userID string) error
}

type userRepository struct {
	db *gorm.DB
}

func NewUserRepository(db *gorm.DB) UserRepository {
	return &userRepository{db: db}
}

func (r *userRepository) Create(user *model.User) error {
	return r.db.Create(user).Error
}

func (r *userRepository) GetByUserID(userID string) (*model.User, error) {
	var user model.User
	if err := r.db.Where("user_id = ?", userID).First(&user).Error; err != nil {
		return nil, err
	}
	return &user, nil
}

func (r *userRepository) GetByPhone(phone string) (*model.User, error) {
	var user model.User
	if err := r.db.Where("phone = ?", phone).First(&user).Error; err != nil {
		return nil, err
	}
	return &user, nil
}

func (r *userRepository) GetByEmail(email string) (*model.User, error) {
	var user model.User
	if err := r.db.Where("email = ?", email).First(&user).Error; err != nil {
		return nil, err
	}
	return &user, nil
}

func (r *userRepository) GetByNickname(nickname string) (*model.User, error) {
	var user model.User
	if err := r.db.Where("nickname = ?", nickname).First(&user).Error; err != nil {
		return nil, err
	}
	return &user, nil
}

func (r *userRepository) Update(user *model.User) error {
	return r.db.Save(user).Error
}

func (r *userRepository) Delete(userID string) error {
	return r.db.Where("user_id = ?", userID).Delete(&model.User{}).Error
}

func (r *userRepository) GetByUserIDs(userIDs []string) ([]model.User, error) {
	var users []model.User
	if len(userIDs) == 0 {
		return users, nil
	}
	if err := r.db.Where("user_id IN ?", userIDs).Find(&users).Error; err != nil {
		return nil, err
	}
	return users, nil
}
