package handler

import (
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"
	"clouddisk/internal/middleware"
	"clouddisk/internal/repository"
	"clouddisk/internal/service"

	"github.com/gin-gonic/gin"
	"github.com/stretchr/testify/assert"
	"gorm.io/gorm"
)

// --- Mock File Repository for handler tests ---

type mockFileRepo struct {
	files map[string]*model.File
}

func newMockFileRepo() *mockFileRepo {
	return &mockFileRepo{
		files: make(map[string]*model.File),
	}
}

func (m *mockFileRepo) Create(file *model.File) error {
	m.files[file.FileID] = file
	return nil
}

func (m *mockFileRepo) GetByFileID(fileID string) (*model.File, error) {
	f, ok := m.files[fileID]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return f, nil
}

func (m *mockFileRepo) GetByUniqueFlag(uniqueFlag string) (*model.File, error) {
	for _, f := range m.files {
		if f.UniqueFlag == uniqueFlag {
			return f, nil
		}
	}
	return nil, gorm.ErrRecordNotFound
}

func (m *mockFileRepo) ListByUserAndPath(userID, parentPath string, offset, limit int) ([]model.File, int64, error) {
	var result []model.File
	for _, f := range m.files {
		if f.UserID == userID && f.ParentPath == parentPath {
			result = append(result, *f)
		}
	}
	total := int64(len(result))
	if offset >= len(result) {
		return []model.File{}, total, nil
	}
	end := offset + limit
	if end > len(result) {
		end = len(result)
	}
	return result[offset:end], total, nil
}

func (m *mockFileRepo) ListPublic(offset, limit int) ([]model.File, int64, error) {
	var result []model.File
	for _, f := range m.files {
		if f.IsPublic {
			result = append(result, *f)
		}
	}
	total := int64(len(result))
	if offset >= len(result) {
		return []model.File{}, total, nil
	}
	end := offset + limit
	if end > len(result) {
		end = len(result)
	}
	return result[offset:end], total, nil
}

func (m *mockFileRepo) Update(file *model.File) error {
	m.files[file.FileID] = file
	return nil
}

func (m *mockFileRepo) Delete(fileID string) error {
	if _, ok := m.files[fileID]; !ok {
		return gorm.ErrRecordNotFound
	}
	delete(m.files, fileID)
	return nil
}

// Compile-time check
var _ repository.FileRepository = (*mockFileRepo)(nil)

func setupFileHandler(t *testing.T) (*FileHandler, *mockFileRepo, *config.Config) {
	t.Helper()
	gin.SetMode(gin.TestMode)

	cfg := &config.Config{
		JWT: config.JWTConfig{
			Secret:      "test-file-handler-secret",
			ExpireHours: 24,
		},
		Server: config.ServerConfig{
			UploadDir: "/tmp/test-uploads",
		},
	}

	fileRepo := newMockFileRepo()
	// Pass nil for esRepo since List and PublicList don't use it
	fileSvc := service.NewFileService(fileRepo, nil)
	handler := NewFileHandler(fileSvc, cfg)

	return handler, fileRepo, cfg
}

func TestList_Handler_Success(t *testing.T) {
	handler, fileRepo, cfg := setupFileHandler(t)

	// Seed some files
	fileRepo.Create(&model.File{
		FileID:     "file-001",
		UserID:     "user-123",
		FileName:   "document.pdf",
		FileType:   model.FileTypeFile,
		FileSize:   2048,
		ParentPath: "/",
	})
	fileRepo.Create(&model.File{
		FileID:     "file-002",
		UserID:     "user-123",
		FileName:   "photo.jpg",
		FileType:   model.FileTypeImage,
		FileSize:   4096,
		ParentPath: "/",
	})

	token, _ := pkg.GenerateToken("user-123", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/file/list", handler.List)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/list?parent_path=/&page=1&size=20", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp struct {
		Code    int    `json:"code"`
		Message string `json:"message"`
		Data    struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
			Page  int          `json:"page"`
			Size  int          `json:"size"`
		} `json:"data"`
	}
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, int64(2), resp.Data.Total)
	assert.Len(t, resp.Data.Files, 2)
	assert.Equal(t, 1, resp.Data.Page)
	assert.Equal(t, 20, resp.Data.Size)
}

func TestList_Handler_Unauthorized(t *testing.T) {
	handler, _, cfg := setupFileHandler(t)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/file/list", handler.List)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/list?parent_path=/", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)
}

func TestList_Handler_DefaultParams(t *testing.T) {
	handler, fileRepo, cfg := setupFileHandler(t)

	fileRepo.Create(&model.File{
		FileID:     "file-001",
		UserID:     "user-123",
		FileName:   "test.txt",
		ParentPath: "/",
	})

	token, _ := pkg.GenerateToken("user-123", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/file/list", handler.List)

	// No query params - should use defaults (parent_path="/", page=1, size=20)
	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/list", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp struct {
		Code int `json:"code"`
		Data struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
			Page  int          `json:"page"`
			Size  int          `json:"size"`
		} `json:"data"`
	}
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, 1, resp.Data.Page)
	assert.Equal(t, 20, resp.Data.Size)
}

func TestList_Handler_EmptyResult(t *testing.T) {
	handler, _, cfg := setupFileHandler(t)

	token, _ := pkg.GenerateToken("user-123", cfg.JWT.Secret, cfg.JWT.ExpireHours)

	router := gin.New()
	router.Use(middleware.JWTAuth(cfg.JWT.Secret))
	router.GET("/api/v1/file/list", handler.List)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/list?parent_path=/empty", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp struct {
		Code int `json:"code"`
		Data struct {
			Total int64 `json:"total"`
		} `json:"data"`
	}
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, int64(0), resp.Data.Total)
}

func TestPublicList_Handler_Success(t *testing.T) {
	handler, fileRepo, _ := setupFileHandler(t)

	// Create public and private files
	fileRepo.Create(&model.File{
		FileID:     "file-pub-001",
		UserID:     "user-123",
		FileName:   "public-doc.pdf",
		ParentPath: "/",
		IsPublic:   true,
	})
	fileRepo.Create(&model.File{
		FileID:     "file-priv-001",
		UserID:     "user-123",
		FileName:   "private-doc.pdf",
		ParentPath: "/",
		IsPublic:   false,
	})
	fileRepo.Create(&model.File{
		FileID:     "file-pub-002",
		UserID:     "user-456",
		FileName:   "another-public.txt",
		ParentPath: "/",
		IsPublic:   true,
	})

	router := gin.New()
	// PublicList does not require auth
	router.GET("/api/v1/file/public", handler.PublicList)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/public?page=1&size=20", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp struct {
		Code int `json:"code"`
		Data struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
			Page  int          `json:"page"`
			Size  int          `json:"size"`
		} `json:"data"`
	}
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, int64(2), resp.Data.Total)
	assert.Len(t, resp.Data.Files, 2)

	// Verify only public files are returned
	for _, f := range resp.Data.Files {
		assert.True(t, f.IsPublic)
	}
}

func TestPublicList_Handler_DefaultParams(t *testing.T) {
	handler, _, _ := setupFileHandler(t)

	router := gin.New()
	router.GET("/api/v1/file/public", handler.PublicList)

	// No query params
	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/public", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp struct {
		Code int `json:"code"`
		Data struct {
			Page int `json:"page"`
			Size int `json:"size"`
		} `json:"data"`
	}
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, 1, resp.Data.Page)
	assert.Equal(t, 20, resp.Data.Size)
}

func TestPublicList_Handler_Pagination(t *testing.T) {
	handler, fileRepo, _ := setupFileHandler(t)

	// Create several public files
	ids := []string{"a", "b", "c", "d", "e"}
	for _, id := range ids {
		fileRepo.Create(&model.File{
			FileID:     "file-pub-" + id,
			UserID:     "user-123",
			FileName:   "public-" + id + ".txt",
			ParentPath: "/",
			IsPublic:   true,
		})
	}

	router := gin.New()
	router.GET("/api/v1/file/public", handler.PublicList)

	req := httptest.NewRequest(http.MethodGet, "/api/v1/file/public?page=1&size=2", nil)
	w := httptest.NewRecorder()

	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp struct {
		Code int `json:"code"`
		Data struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
			Size  int          `json:"size"`
		} `json:"data"`
	}
	json.Unmarshal(w.Body.Bytes(), &resp)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, int64(5), resp.Data.Total)
	assert.Len(t, resp.Data.Files, 2) // Only 2 per page
}
