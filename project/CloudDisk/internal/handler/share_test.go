package handler

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"
	"time"

	"clouddisk/internal/config"
	"clouddisk/internal/middleware"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"
	"clouddisk/internal/repository"
	"clouddisk/internal/service"

	"github.com/gin-gonic/gin"
	"github.com/stretchr/testify/assert"
	"gorm.io/gorm"
)

// --- Mock Share Repository for handler tests ---

type mockShareRepo struct {
	shares map[string]*model.Share
}

func newMockShareRepo() *mockShareRepo {
	return &mockShareRepo{
		shares: make(map[string]*model.Share),
	}
}

func (m *mockShareRepo) Create(share *model.Share) error {
	m.shares[share.ShareID] = share
	return nil
}

func (m *mockShareRepo) GetByShareID(shareID string) (*model.Share, error) {
	s, ok := m.shares[shareID]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return s, nil
}

func (m *mockShareRepo) GetByFileIDAndType(fileID string, shareType uint8) (*model.Share, error) {
	for _, s := range m.shares {
		if s.FileID == fileID && s.ShareType == shareType {
			return s, nil
		}
	}
	return nil, gorm.ErrRecordNotFound
}

func (m *mockShareRepo) DeleteByFileIDAndType(fileID string, shareType uint8) error {
	for id, s := range m.shares {
		if s.FileID == fileID && s.ShareType == shareType {
			delete(m.shares, id)
			return nil
		}
	}
	return nil
}

func (m *mockShareRepo) IncrementCount(shareID string) error {
	s, ok := m.shares[shareID]
	if !ok {
		return gorm.ErrRecordNotFound
	}
	s.CurrentCount++
	return nil
}

// Compile-time check
var _ repository.ShareRepository = (*mockShareRepo)(nil)

func setupShareHandler(t *testing.T) (*ShareHandler, *mockFileRepo, *mockShareRepo, *config.Config) {
	t.Helper()
	gin.SetMode(gin.TestMode)

	cfg := &config.Config{
		JWT: config.JWTConfig{
			Secret:      "test-share-handler-secret",
			ExpireHours: 24,
		},
	}

	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()
	shareSvc := service.NewShareService(fileRepo, shareRepo)
	handler := NewShareHandler(shareSvc)

	return handler, fileRepo, shareRepo, cfg
}

// --- Share tests ---

func TestShare_Handler_Success(t *testing.T) {
	handler, fileRepo, _, cfg := setupShareHandler(t)

	fileRepo.Create(&model.File{
		FileID:     "file-share-001",
		UserID:     "user-share-001",
		FileName:   "shared-doc.pdf",
		ParentPath: "/",
	})

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/share", handler.Share)

	body := map[string]interface{}{
		"message":      "Check this out",
		"expire_hours": 24,
		"max_count":    10,
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/file-share-001/share", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, "success", resp.Message)

	var share model.Share
	json.Unmarshal(resp.Data, &share)
	assert.NotEmpty(t, share.ShareID)
	assert.Equal(t, "file-share-001", share.FileID)
	assert.Equal(t, "Check this out", share.Message)
}

func TestShare_Handler_Unauthorized(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/share", handler.Share)

	body := map[string]interface{}{
		"message":      "test",
		"expire_hours": 24,
		"max_count":    10,
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/file-001/share", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

func TestShare_Handler_FileNotFound(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/share", handler.Share)

	body := map[string]interface{}{
		"message":      "test",
		"expire_hours": 24,
		"max_count":    10,
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/nonexistent-file/share", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusNotFound, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, -1, resp.Code)
	assert.Contains(t, resp.Message, "file not found")
}

func TestShare_Handler_AccessDenied(t *testing.T) {
	handler, fileRepo, _, cfg := setupShareHandler(t)

	// File belongs to a different user
	fileRepo.Create(&model.File{
		FileID:     "file-other-001",
		UserID:     "user-other-001",
		FileName:   "other-doc.pdf",
		ParentPath: "/",
	})

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/share", handler.Share)

	body := map[string]interface{}{
		"message":      "test",
		"expire_hours": 24,
		"max_count":    10,
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/file-other-001/share", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusForbidden, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Contains(t, resp.Message, "access denied")
}

func TestShare_Handler_InvalidBody(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/share", handler.Share)

	req := httptest.NewRequest(http.MethodPost, "/file-001/share", bytes.NewBuffer([]byte("not json")))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusBadRequest, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Contains(t, resp.Message, "invalid request body")
}

// --- Unshare tests ---

func TestUnshare_Handler_Success(t *testing.T) {
	handler, fileRepo, shareRepo, cfg := setupShareHandler(t)

	fileRepo.Create(&model.File{
		FileID:     "file-unshare-001",
		UserID:     "user-share-001",
		FileName:   "shared-doc.pdf",
		ParentPath: "/",
		IsShared:   true,
	})

	shareRepo.Create(&model.Share{
		ShareID:   "share-to-delete",
		UserID:    "user-share-001",
		FileID:    "file-unshare-001",
		ShareType: model.ShareTypeShare,
	})

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.DELETE("/:file_id/share", handler.Unshare)

	req := httptest.NewRequest(http.MethodDelete, "/file-unshare-001/share", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)

	// Verify file is no longer shared
	file, _ := fileRepo.GetByFileID("file-unshare-001")
	assert.False(t, file.IsShared)
}

func TestUnshare_Handler_Unauthorized(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.DELETE("/:file_id/share", handler.Unshare)

	req := httptest.NewRequest(http.MethodDelete, "/file-001/share", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

func TestUnshare_Handler_FileNotFound(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.DELETE("/:file_id/share", handler.Unshare)

	req := httptest.NewRequest(http.MethodDelete, "/nonexistent-file/share", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusNotFound, w.Code)
}

// --- Public tests ---

func TestPublic_Handler_Success(t *testing.T) {
	handler, fileRepo, _, cfg := setupShareHandler(t)

	fileRepo.Create(&model.File{
		FileID:     "file-public-001",
		UserID:     "user-share-001",
		FileName:   "public-doc.pdf",
		ParentPath: "/",
	})

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/public", handler.Public)

	body := map[string]interface{}{
		"expire_hours": 48,
		"max_count":    100,
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/file-public-001/public", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)

	var share model.Share
	json.Unmarshal(resp.Data, &share)
	assert.NotEmpty(t, share.ShareID)
	assert.Equal(t, model.ShareTypePublic, share.ShareType)
}

func TestPublic_Handler_Unauthorized(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/:file_id/public", handler.Public)

	body := map[string]interface{}{
		"expire_hours": 48,
		"max_count":    100,
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/file-001/public", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

// --- Unpublic tests ---

func TestUnpublic_Handler_Success(t *testing.T) {
	handler, fileRepo, shareRepo, cfg := setupShareHandler(t)

	fileRepo.Create(&model.File{
		FileID:     "file-unpublic-001",
		UserID:     "user-share-001",
		FileName:   "public-doc.pdf",
		ParentPath: "/",
		IsPublic:   true,
	})

	shareRepo.Create(&model.Share{
		ShareID:   "public-to-delete",
		UserID:    "user-share-001",
		FileID:    "file-unpublic-001",
		ShareType: model.ShareTypePublic,
	})

	token, _ := pkg.GenerateToken("user-share-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.DELETE("/:file_id/public", handler.Unpublic)

	req := httptest.NewRequest(http.MethodDelete, "/file-unpublic-001/public", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)

	// Verify file is no longer public
	file, _ := fileRepo.GetByFileID("file-unpublic-001")
	assert.False(t, file.IsPublic)
}

func TestUnpublic_Handler_Unauthorized(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.DELETE("/:file_id/public", handler.Unpublic)

	req := httptest.NewRequest(http.MethodDelete, "/file-001/public", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

// --- Receive tests ---

func TestReceive_Handler_Success(t *testing.T) {
	handler, fileRepo, shareRepo, cfg := setupShareHandler(t)

	// Original file
	fileRepo.Create(&model.File{
		FileID:      "file-recv-001",
		UserID:      "user-owner-001",
		FileName:    "shared-file.pdf",
		FileType:    model.FileTypeFile,
		FileSize:    1024,
		StoragePath: "/storage/shared-file.pdf",
		ParentPath:  "/",
		UniqueFlag:  "unique-001",
	})

	// Share record
	expireAt := time.Now().Add(24 * time.Hour)
	shareRepo.Create(&model.Share{
		ShareID:   "share-recv-001",
		UserID:    "user-owner-001",
		FileID:    "file-recv-001",
		ShareType: model.ShareTypeShare,
		ExpireAt:  &expireAt,
		MaxCount:  10,
	})

	token, _ := pkg.GenerateToken("user-receiver-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/receive/:share_id", handler.Receive)

	body := map[string]string{
		"save_path": "/received",
	}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/receive/share-recv-001", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp apiResponse
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)

	var file model.File
	json.Unmarshal(resp.Data, &file)
	assert.Equal(t, "shared-file.pdf", file.FileName)
	assert.Equal(t, "/received", file.ParentPath)
	assert.Equal(t, model.SourceOthers, file.Source)
	assert.Equal(t, "user-owner-001", file.SourceUserID)
}

func TestReceive_Handler_Unauthorized(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/receive/:share_id", handler.Receive)

	body := map[string]string{"save_path": "/"}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/receive/share-001", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

func TestReceive_Handler_ShareNotFound(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	token, _ := pkg.GenerateToken("user-receiver-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/receive/:share_id", handler.Receive)

	body := map[string]string{"save_path": "/"}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/receive/nonexistent-share", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusNotFound, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Contains(t, resp.Message, "share not found")
}

func TestReceive_Handler_ExpiredShare(t *testing.T) {
	handler, fileRepo, shareRepo, cfg := setupShareHandler(t)

	fileRepo.Create(&model.File{
		FileID:     "file-expired-001",
		UserID:     "user-owner-001",
		FileName:   "expired-file.pdf",
		ParentPath: "/",
	})

	// Expired share
	expiredAt := time.Now().Add(-1 * time.Hour)
	shareRepo.Create(&model.Share{
		ShareID:   "share-expired-001",
		UserID:    "user-owner-001",
		FileID:    "file-expired-001",
		ShareType: model.ShareTypeShare,
		ExpireAt:  &expiredAt,
	})

	token, _ := pkg.GenerateToken("user-receiver-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/receive/:share_id", handler.Receive)

	body := map[string]string{"save_path": "/"}
	jsonBody, _ := json.Marshal(body)
	req := httptest.NewRequest(http.MethodPost, "/receive/share-expired-001", bytes.NewBuffer(jsonBody))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusForbidden, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Contains(t, resp.Message, "share has expired")
}

func TestReceive_Handler_InvalidBody(t *testing.T) {
	handler, _, _, cfg := setupShareHandler(t)

	token, _ := pkg.GenerateToken("user-receiver-001", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.POST("/receive/:share_id", handler.Receive)

	req := httptest.NewRequest(http.MethodPost, "/receive/share-001", bytes.NewBuffer([]byte("not json")))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusBadRequest, w.Code)

	var resp apiResponse
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Contains(t, resp.Message, "invalid request body")
}
