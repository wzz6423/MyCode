package service

import (
	"testing"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"

	"github.com/alicebob/miniredis/v2"
	"github.com/redis/go-redis/v9"
	"github.com/stretchr/testify/assert"
)

func setupUserService(t *testing.T) (*UserService, *mockUserRepo, *miniredis.Miniredis) {
	t.Helper()
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	cfg := &config.Config{
		JWT: config.JWTConfig{
			Secret:      "test-secret-key",
			ExpireHours: 24,
		},
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 10,
		},
	}

	repo := newMockUserRepo()
	verifySvc := NewVerifyService(rdb, cfg)
	userSvc := NewUserService(repo, verifySvc, cfg)

	return userSvc, repo, mr
}

// Helper to set a verify code in miniredis and return codeID + code
func setVerifyCode(mr *miniredis.Miniredis, codeID, code string) {
	mr.Set("verify:"+codeID, code)
}

func TestRegister_Phone_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	codeID := "reg-phone-code-id"
	code := "123456"
	setVerifyCode(mr, codeID, code)

	err := svc.Register("phone", "13800138000", "password123", "TestUser", codeID, code)
	assert.NoError(t, err)

	// Verify user was created in the repo
	user, err := repo.GetByPhone("13800138000")
	assert.NoError(t, err)
	assert.Equal(t, "TestUser", user.Nickname)
	assert.NotEmpty(t, user.UserID)
	assert.NotNil(t, user.Phone)
	assert.Equal(t, "13800138000", *user.Phone)
	// Password should be hashed
	assert.True(t, pkg.CheckPassword("password123", user.Password))
}

func TestRegister_Email_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	codeID := "reg-email-code-id"
	code := "654321"
	setVerifyCode(mr, codeID, code)

	err := svc.Register("email", "test@example.com", "password123", "EmailUser", codeID, code)
	assert.NoError(t, err)

	user, err := repo.GetByEmail("test@example.com")
	assert.NoError(t, err)
	assert.Equal(t, "EmailUser", user.Nickname)
	assert.NotNil(t, user.Email)
	assert.Equal(t, "test@example.com", *user.Email)
}

func TestRegister_InvalidCode(t *testing.T) {
	svc, _, mr := setupUserService(t)
	defer mr.Close()

	codeID := "bad-code-id"
	setVerifyCode(mr, codeID, "123456")

	// Use wrong code
	err := svc.Register("phone", "13800138000", "password123", "TestUser", codeID, "999999")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "invalid or expired verify code")
}

func TestRegister_ExpiredCode(t *testing.T) {
	svc, _, mr := setupUserService(t)
	defer mr.Close()

	// Don't set any code - simulates expired
	err := svc.Register("phone", "13800138000", "password123", "TestUser", "expired-id", "123456")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "invalid or expired verify code")
}

func TestRegister_DuplicatePhone(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	// Pre-populate a user with the same phone
	phone := "13800138000"
	repo.Create(&model.User{
		UserID:   "existing-user-id",
		Nickname: "ExistingUser",
		Phone:    &phone,
		Password: "hashed",
	})

	codeID := "dup-phone-code-id"
	code := "123456"
	setVerifyCode(mr, codeID, code)

	err := svc.Register("phone", "13800138000", "password123", "NewUser", codeID, code)
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "phone already registered")
}

func TestRegister_DuplicateEmail(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	email := "test@example.com"
	repo.Create(&model.User{
		UserID:   "existing-user-id",
		Nickname: "ExistingUser",
		Email:    &email,
		Password: "hashed",
	})

	codeID := "dup-email-code-id"
	code := "123456"
	setVerifyCode(mr, codeID, code)

	err := svc.Register("email", "test@example.com", "password123", "NewUser", codeID, code)
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "email already registered")
}

func TestRegister_UnsupportedType(t *testing.T) {
	svc, _, mr := setupUserService(t)
	defer mr.Close()

	codeID := "type-code-id"
	code := "123456"
	setVerifyCode(mr, codeID, code)

	err := svc.Register("fax", "12345", "password123", "TestUser", codeID, code)
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "unsupported register type")
}

func TestLogin_Phone_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	// Create a user with hashed password
	hashed, _ := pkg.HashPassword("password123")
	phone := "13800138000"
	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "TestUser",
		Phone:    &phone,
		Password: hashed,
	})

	token, user, err := svc.Login("phone", "13800138000", "password123")
	assert.NoError(t, err)
	assert.NotEmpty(t, token)
	assert.NotNil(t, user)
	assert.Equal(t, "user-123", user.UserID)
	assert.Equal(t, "TestUser", user.Nickname)

	// Verify the token is valid
	claims, err := pkg.ParseToken(token, "test-secret-key")
	assert.NoError(t, err)
	assert.Equal(t, "user-123", claims.UserID)
}

func TestLogin_Email_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("password123")
	email := "test@example.com"
	repo.Create(&model.User{
		UserID:   "user-456",
		Nickname: "EmailUser",
		Email:    &email,
		Password: hashed,
	})

	token, user, err := svc.Login("email", "test@example.com", "password123")
	assert.NoError(t, err)
	assert.NotEmpty(t, token)
	assert.NotNil(t, user)
	assert.Equal(t, "user-456", user.UserID)
}

func TestLogin_WrongPassword(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("password123")
	phone := "13800138000"
	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "TestUser",
		Phone:    &phone,
		Password: hashed,
	})

	_, _, err := svc.Login("phone", "13800138000", "wrongpassword")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "incorrect password")
}

func TestLogin_UserNotFound(t *testing.T) {
	svc, _, mr := setupUserService(t)
	defer mr.Close()

	_, _, err := svc.Login("phone", "99999999999", "password123")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "user not found")
}

func TestLogin_UnsupportedType(t *testing.T) {
	svc, _, mr := setupUserService(t)
	defer mr.Close()

	_, _, err := svc.Login("fax", "12345", "password123")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "unsupported login type")
}

func TestGetProfile_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	repo.Create(&model.User{
		UserID:      "user-123",
		Nickname:    "TestUser",
		Description: "Hello world",
	})

	user, err := svc.GetProfile("user-123")
	assert.NoError(t, err)
	assert.Equal(t, "TestUser", user.Nickname)
	assert.Equal(t, "Hello world", user.Description)
}

func TestGetProfile_NotFound(t *testing.T) {
	svc, _, mr := setupUserService(t)
	defer mr.Close()

	_, err := svc.GetProfile("nonexistent")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "user not found")
}

func TestUpdatePassword_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("oldpassword")
	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "TestUser",
		Password: hashed,
	})

	err := svc.UpdatePassword("user-123", "oldpassword", "newpassword")
	assert.NoError(t, err)

	// Verify the password was updated
	user, _ := repo.GetByUserID("user-123")
	assert.True(t, pkg.CheckPassword("newpassword", user.Password))
	assert.False(t, pkg.CheckPassword("oldpassword", user.Password))
}

func TestUpdatePassword_WrongOld(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("oldpassword")
	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "TestUser",
		Password: hashed,
	})

	err := svc.UpdatePassword("user-123", "wrongold", "newpassword")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "incorrect old password")
}

func TestUpdateNickname_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "OldName",
	})

	err := svc.UpdateNickname("user-123", "NewName")
	assert.NoError(t, err)

	user, _ := repo.GetByUserID("user-123")
	assert.Equal(t, "NewName", user.Nickname)
}

func TestUpdateAvatar_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	repo.Create(&model.User{
		UserID:    "user-123",
		Nickname:  "TestUser",
		AvatarURL: "/old-avatar.png",
	})

	err := svc.UpdateAvatar("user-123", "/new-avatar.png")
	assert.NoError(t, err)

	user, _ := repo.GetByUserID("user-123")
	assert.Equal(t, "/new-avatar.png", user.AvatarURL)
}

func TestUpdateDescription_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	repo.Create(&model.User{
		UserID:      "user-123",
		Nickname:    "TestUser",
		Description: "Old desc",
	})

	err := svc.UpdateDescription("user-123", "New description")
	assert.NoError(t, err)

	user, _ := repo.GetByUserID("user-123")
	assert.Equal(t, "New description", user.Description)
}

func TestUpdatePhone_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("password123")
	oldPhone := "13800138000"
	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "TestUser",
		Phone:    &oldPhone,
		Password: hashed,
	})

	codeID := "phone-update-code"
	code := "123456"
	setVerifyCode(mr, codeID, code)

	err := svc.UpdatePhone("user-123", "13900139000", codeID, code, "password123")
	assert.NoError(t, err)

	user, _ := repo.GetByUserID("user-123")
	assert.NotNil(t, user.Phone)
	assert.Equal(t, "13900139000", *user.Phone)
}

func TestUpdateEmail_Success(t *testing.T) {
	svc, repo, mr := setupUserService(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("password123")
	oldEmail := "old@example.com"
	repo.Create(&model.User{
		UserID:   "user-123",
		Nickname: "TestUser",
		Email:    &oldEmail,
		Password: hashed,
	})

	codeID := "email-update-code"
	code := "654321"
	setVerifyCode(mr, codeID, code)

	err := svc.UpdateEmail("user-123", "new@example.com", codeID, code, "password123")
	assert.NoError(t, err)

	user, _ := repo.GetByUserID("user-123")
	assert.NotNil(t, user.Email)
	assert.Equal(t, "new@example.com", *user.Email)
}
