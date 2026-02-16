package service

import (
	"errors"
	"fmt"
	"os"
	"path/filepath"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"
	"clouddisk/internal/repository"

	"gorm.io/gorm"
)

type UserService struct {
	repo      repository.UserRepository
	fileRepo  repository.FileRepository
	shareRepo repository.ShareRepository
	verifySvc *VerifyService
	cfg       *config.Config
}

func NewUserService(repo repository.UserRepository, fileRepo repository.FileRepository, shareRepo repository.ShareRepository, verifySvc *VerifyService, cfg *config.Config) *UserService {
	return &UserService{repo: repo, fileRepo: fileRepo, shareRepo: shareRepo, verifySvc: verifySvc, cfg: cfg}
}

func (s *UserService) SendVerifyCode(codeType, target string) (string, error) {
	return s.verifySvc.SendCode(codeType, target)
}

func (s *UserService) Register(regType, target, password, nickname, codeID, code string) error {
	switch regType {
	case "nickname":
		if _, err := s.repo.GetByNickname(nickname); err == nil {
			return errors.New("nickname already taken")
		}
	case "phone":
		if !s.verifySvc.ValidateCode(codeID, code) {
			return errors.New("invalid or expired verify code")
		}
		if _, err := s.repo.GetByPhone(target); err == nil {
			return errors.New("phone already registered")
		}
	case "email":
		if !s.verifySvc.ValidateCode(codeID, code) {
			return errors.New("invalid or expired verify code")
		}
		if _, err := s.repo.GetByEmail(target); err == nil {
			return errors.New("email already registered")
		}
	default:
		return fmt.Errorf("unsupported register type: %s", regType)
	}

	hashed, err := pkg.HashPassword(password)
	if err != nil {
		return fmt.Errorf("failed to hash password: %w", err)
	}

	user := &model.User{
		UserID:   pkg.NewUUID(),
		Nickname: nickname,
		Password: hashed,
	}

	switch regType {
	case "phone":
		user.Phone = &target
	case "email":
		user.Email = &target
	}

	if err := s.repo.Create(user); err != nil {
		return fmt.Errorf("failed to create user: %w", err)
	}

	return nil
}

func (s *UserService) Login(loginType, target, password string) (string, *model.User, error) {
	var user *model.User
	var err error

	switch loginType {
	case "nickname":
		user, err = s.repo.GetByNickname(target)
	case "phone":
		user, err = s.repo.GetByPhone(target)
	case "email":
		user, err = s.repo.GetByEmail(target)
	default:
		return "", nil, fmt.Errorf("unsupported login type: %s", loginType)
	}

	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return "", nil, errors.New("user not found")
		}
		return "", nil, fmt.Errorf("failed to find user: %w", err)
	}

	if !pkg.CheckPassword(password, user.Password) {
		return "", nil, errors.New("incorrect password")
	}

	token, err := pkg.GenerateToken(user.UserID, s.cfg.JWT.Secret, s.cfg.JWT.ExpireHours)
	if err != nil {
		return "", nil, fmt.Errorf("failed to generate token: %w", err)
	}

	return token, user, nil
}

func (s *UserService) GetProfile(userID string) (*model.User, error) {
	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			return nil, errors.New("user not found")
		}
		return nil, fmt.Errorf("failed to get user: %w", err)
	}
	return user, nil
}

func (s *UserService) UpdateAvatar(userID, avatarURL string) error {
	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}
	user.AvatarURL = avatarURL
	return s.repo.Update(user)
}

func (s *UserService) UpdateNickname(userID, nickname string) error {
	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}
	user.Nickname = nickname
	return s.repo.Update(user)
}

func (s *UserService) UpdateDescription(userID, desc string) error {
	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}
	user.Description = desc
	return s.repo.Update(user)
}

func (s *UserService) UpdatePassword(userID, oldPwd, newPwd string) error {
	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}

	if !pkg.CheckPassword(oldPwd, user.Password) {
		return errors.New("incorrect old password")
	}

	hashed, err := pkg.HashPassword(newPwd)
	if err != nil {
		return fmt.Errorf("failed to hash password: %w", err)
	}

	user.Password = hashed
	return s.repo.Update(user)
}

func (s *UserService) UpdatePhone(userID, newPhone, codeID, code, password string) error {
	if !s.verifySvc.ValidateCode(codeID, code) {
		return errors.New("invalid or expired verify code")
	}

	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}

	if !pkg.CheckPassword(password, user.Password) {
		return errors.New("incorrect password")
	}

	if existing, err := s.repo.GetByPhone(newPhone); err == nil && existing.UserID != userID {
		return errors.New("phone already in use")
	}

	user.Phone = &newPhone
	return s.repo.Update(user)
}

func (s *UserService) UpdateEmail(userID, newEmail, codeID, code, password string) error {
	if !s.verifySvc.ValidateCode(codeID, code) {
		return errors.New("invalid or expired verify code")
	}

	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}

	if !pkg.CheckPassword(password, user.Password) {
		return errors.New("incorrect password")
	}

	if existing, err := s.repo.GetByEmail(newEmail); err == nil && existing.UserID != userID {
		return errors.New("email already in use")
	}

	user.Email = &newEmail
	return s.repo.Update(user)
}

func (s *UserService) DeleteAccount(userID, password string) error {
	user, err := s.repo.GetByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to find user: %w", err)
	}

	if !pkg.CheckPassword(password, user.Password) {
		return errors.New("incorrect password")
	}

	// 1. Get all user files to collect IDs and delete from disk
	files, err := s.fileRepo.ListByUserID(userID)
	if err != nil {
		return fmt.Errorf("failed to list user files: %w", err)
	}

	// 2. Delete shares associated with user's files
	if len(files) > 0 {
		fileIDs := make([]string, len(files))
		for i, f := range files {
			fileIDs[i] = f.FileID
		}
		if err := s.shareRepo.DeleteByFileIDs(fileIDs); err != nil {
			return fmt.Errorf("failed to delete shares: %w", err)
		}
	}

	// 3. Delete file records from DB
	if err := s.fileRepo.DeleteByUserID(userID); err != nil {
		return fmt.Errorf("failed to delete files: %w", err)
	}

	// 4. Delete user's entire upload directory (includes all files and folders)
	userDir := filepath.Join(s.cfg.Server.UploadDir, userID)
	_ = os.RemoveAll(userDir)

	// 5. Delete avatar file from disk
	if user.AvatarURL != "" {
		avatarPath := filepath.Join(s.cfg.Server.UploadDir, user.AvatarURL)
		_ = os.Remove(avatarPath)
	}

	// 6. Delete user record
	if err := s.repo.Delete(userID); err != nil {
		return fmt.Errorf("failed to delete user: %w", err)
	}

	return nil
}
