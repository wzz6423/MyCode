package handler

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/middleware"
	"clouddisk/internal/pkg"
	"clouddisk/internal/repository"
	"clouddisk/internal/service"

	"github.com/alicebob/miniredis/v2"
	"github.com/gin-gonic/gin"
	"github.com/redis/go-redis/v9"
	"github.com/stretchr/testify/assert"
	"gorm.io/gorm"
)

// --- Mock User Repository for handler tests ---

type mockUserRepo struct {
	users  map[string]*model.User
	phones map[string]*model.User
	emails map[string]*model.User
}

func newMockUserRepo() *mockUserRepo {
	return &mockUserRepo{
		users:  make(map[string]*model.User),
		phones: make(map[string]*model.User),
		emails: make(map[string]*model.User),
	}
}

func (m *mockUserRepo) Create(user *model.User) error {
	m.users[user.UserID] = user
	if user.Phone != nil {
		m.phones[*user.Phone] = user
	}
	if user.Email != nil {
		m.emails[*user.Email] = user
	}
	return nil
}

func (m *mockUserRepo) GetByUserID(userID string) (*model.User, error) {
	u, ok := m.users[userID]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return u, nil
}

func (m *mockUserRepo) GetByPhone(phone string) (*model.User, error) {
	u, ok := m.phones[phone]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return u, nil
}

func (m *mockUserRepo) GetByEmail(email string) (*model.User, error) {
	u, ok := m.emails[email]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return u, nil
}

func (m *mockUserRepo) Update(user *model.User) error {
	m.users[user.UserID] = user
	if user.Phone != nil {
		m.phones[*user.Phone] = user
	}
	if user.Email != nil {
		m.emails[*user.Email] = user
	}
	return nil
}

// Compile-time check that mockUserRepo implements repository.UserRepository
var _ repository.UserRepository = (*mockUserRepo)(nil)

func setupUserHandler(t *testing.T) (*UserHandler, *mockUserRepo, *config.Config, *miniredis.Miniredis) {
	t.Helper()
	gin.SetMode(gin.TestMode)

	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	cfg := &config.Config{
		JWT: config.JWTConfig{
			Secret:      "test-handler-secret",
			ExpireHours: 24,
		},
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 10,
		},
	}

	repo := newMockUserRepo()
	verifySvc := service.NewVerifyService(rdb, cfg)
	userSvc := service.NewUserService(repo, verifySvc, cfg)
	handler := NewUserHandler(userSvc, cfg)

	return handler, repo, cfg, mr
}

type apiResponse struct {
	Code    int             `json:"code"`
	Message string          `json:"message"`
	Data    json.RawMessage `json:"data"`
}

func TestLogin_Handler_Success(t *testing.T) {
	handler, repo, cfg, mr := setupUserHandler(t)
	defer mr.Close()

	// Create a user
	hashed, _ := pkg.HashPassword("password123")
	phone := "13800138000"
	repo.Create(&model.User{
		UserID:   "user-handler-001",
		Nickname: "HandlerUser",
		Phone:    &phone,
		Password: hashed,
	})

	// Setup router
	router := gin.New()
	router.POST("/api/v1/user/login", handler.Login)

	// Create request
	body := map[string]string{
		"type":     "phone",
		"target":   "13800138000",
		"password": "password123",
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/api/v1/user/login", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, "success", resp.Message)

	// Parse the data to verify token and user are present
	var data map[string]json.RawMessage
	err = json.Unmarshal(resp.Data, &data)
	assert.NoError(t, err)
	assert.Contains(t, string(data["token"]), "")

	// Verify the token is valid
	var token string
	json.Unmarshal(data["token"], &token)
	assert.NotEmpty(t, token)
	claims, err := pkg.ParseToken(token, cfg.JWT.Secret)
	assert.NoError(t, err)
	assert.Equal(t, "user-handler-001", claims.UserID)
}

func TestLogin_Handler_BadRequest(t *testing.T) {
	handler, _, _, mr := setupUserHandler(t)
	defer mr.Close()

	router := gin.New()
	router.POST("/api/v1/user/login", handler.Login)

	// Missing required fields
	body := map[string]string{
		"type": "phone",
		// missing target and password
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/api/v1/user/login", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusBadRequest, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, -1, resp.Code)
	assert.Contains(t, resp.Message, "invalid request")
}

func TestLogin_Handler_WrongPassword(t *testing.T) {
	handler, repo, _, mr := setupUserHandler(t)
	defer mr.Close()

	hashed, _ := pkg.HashPassword("password123")
	phone := "13800138000"
	repo.Create(&model.User{
		UserID:   "user-handler-001",
		Nickname: "HandlerUser",
		Phone:    &phone,
		Password: hashed,
	})

	router := gin.New()
	router.POST("/api/v1/user/login", handler.Login)

	body := map[string]string{
		"type":     "phone",
		"target":   "13800138000",
		"password": "wrongpassword",
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/api/v1/user/login", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, -1, resp.Code)
	assert.Contains(t, resp.Message, "incorrect password")
}

func TestGetProfile_Handler_Success(t *testing.T) {
	handler, repo, cfg, mr := setupUserHandler(t)
	defer mr.Close()

	repo.Create(&model.User{
		UserID:      "user-handler-001",
		Nickname:    "ProfileUser",
		Description: "Test description",
	})

	// Generate a valid token
	token, _ := pkg.GenerateToken("user-handler-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/user/profile", handler.GetProfile)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/user/profile", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)

	// Parse user data
	var user model.User
	json.Unmarshal(resp.Data, &user)
	assert.Equal(t, "ProfileUser", user.Nickname)
	assert.Equal(t, "Test description", user.Description)
}

func TestGetProfile_Handler_Unauthorized(t *testing.T) {
	handler, _, cfg, mr := setupUserHandler(t)
	defer mr.Close()

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/user/profile", handler.GetProfile)

	// No Authorization header
	req := httptest.NewRequest(http.MethodGet, "/api/v1/user/profile", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, -1, resp.Code)
}

func TestGetProfile_Handler_InvalidToken(t *testing.T) {
	handler, _, cfg, mr := setupUserHandler(t)
	defer mr.Close()

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/user/profile", handler.GetProfile)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/user/profile", nil)
	req.Header.Set("Authorization", "Bearer invalid-token-here")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

func TestLogin_Handler_EmptyBody(t *testing.T) {
	handler, _, _, mr := setupUserHandler(t)
	defer mr.Close()

	router := gin.New()
	router.POST("/api/v1/user/login", handler.Login)

	req := httptest.NewRequest(http.MethodPost, "/api/v1/user/login", bytes.NewBuffer([]byte("{}")))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusBadRequest, w.Code)
}

func TestLogin_Handler_InvalidJSON(t *testing.T) {
	handler, _, _, mr := setupUserHandler(t)
	defer mr.Close()

	router := gin.New()
	router.POST("/api/v1/user/login", handler.Login)

	req := httptest.NewRequest(http.MethodPost, "/api/v1/user/login", bytes.NewBuffer([]byte("not json")))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusBadRequest, w.Code)
}
