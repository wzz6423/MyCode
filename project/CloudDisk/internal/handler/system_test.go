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
	"clouddisk/internal/service"

	"github.com/alicebob/miniredis/v2"
	"github.com/gin-gonic/gin"
	"github.com/redis/go-redis/v9"
	"github.com/stretchr/testify/assert"
)

// systemEnv holds all the wired-up components for system tests.
type systemEnv struct {
	router    *gin.Engine
	cfg       *config.Config
	userRepo  *mockUserRepo
	fileRepo  *mockFileRepo
	shareRepo *mockShareRepo
	mr        *miniredis.Miniredis
	verifySvc *service.VerifyService
}

func setupSystemEnv(t *testing.T) *systemEnv {
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
			Secret:      "system-test-secret",
			ExpireHours: 24,
		},
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 10,
		},
		Server: config.ServerConfig{
			UploadDir: "/tmp/system-test-uploads",
		},
	}

	userRepo := newMockUserRepo()
	fileRepo := newMockFileRepo()
	shareRepo := newMockShareRepo()

	verifySvc := service.NewVerifyService(rdb, cfg)
	userSvc := service.NewUserService(userRepo, verifySvc, cfg)
	fileSvc := service.NewFileService(fileRepo, nil)
	shareSvc := service.NewShareService(fileRepo, shareRepo)

	userHandler := NewUserHandler(userSvc, cfg)
	fileHandler := NewFileHandler(fileSvc, cfg)
	shareHandler := NewShareHandler(shareSvc)

	router := gin.New()

	// Public routes (no auth)
	router.POST("/api/v1/user/verify-code", userHandler.GetVerifyCode)
	router.POST("/api/v1/user/register", userHandler.Register)
	router.POST("/api/v1/user/login", userHandler.Login)
	router.GET("/api/v1/file/public", fileHandler.PublicList)

	// Auth routes
	auth := router.Group("/api/v1")
	auth.Use(middleware.JWTAuth(cfg.JWT.Secret))
	{
		auth.GET("/user/profile", userHandler.GetProfile)
		auth.GET("/file/list", fileHandler.List)
		auth.POST("/file/:file_id/share", shareHandler.Share)
		auth.DELETE("/file/:file_id/share", shareHandler.Unshare)
		auth.POST("/file/:file_id/public", shareHandler.Public)
		auth.DELETE("/file/:file_id/public", shareHandler.Unpublic)
		auth.POST("/share/receive/:share_id", shareHandler.Receive)
	}

	return &systemEnv{
		router:    router,
		cfg:       cfg,
		userRepo:  userRepo,
		fileRepo:  fileRepo,
		shareRepo: shareRepo,
		mr:        mr,
		verifySvc: verifySvc,
	}
}

// registerUser is a helper that registers a user by seeding a verify code in miniredis,
// then calling the register endpoint.
func (env *systemEnv) registerUser(t *testing.T, regType, target, password, nickname string) {
	t.Helper()

	// Step 1: request verify code
	vcBody, _ := json.Marshal(map[string]string{
		"type":   regType,
		"target": target,
	})
	vcReq := httptest.NewRequest(http.MethodPost, "/api/v1/user/verify-code", bytes.NewBuffer(vcBody))
	vcReq.Header.Set("Content-Type", "application/json")
	vcW := httptest.NewRecorder()
	env.router.ServeHTTP(vcW, vcReq)
	assert.Equal(t, http.StatusOK, vcW.Code)

	var vcResp struct {
		Code int `json:"code"`
		Data struct {
			VerifyCodeID string `json:"verify_code_id"`
		} `json:"data"`
	}
	json.Unmarshal(vcW.Body.Bytes(), &vcResp)
	codeID := vcResp.Data.VerifyCodeID

	// Retrieve the code from miniredis
	code, err := env.mr.Get("verify:" + codeID)
	assert.NoError(t, err)

	// Step 2: register
	regBody, _ := json.Marshal(map[string]string{
		"type":           regType,
		"target":         target,
		"password":       password,
		"nickname":       nickname,
		"verify_code_id": codeID,
		"verify_code":    code,
	})
	regReq := httptest.NewRequest(http.MethodPost, "/api/v1/user/register", bytes.NewBuffer(regBody))
	regReq.Header.Set("Content-Type", "application/json")
	regW := httptest.NewRecorder()
	env.router.ServeHTTP(regW, regReq)
	assert.Equal(t, http.StatusOK, regW.Code)

	var regResp apiResponse
	json.Unmarshal(regW.Body.Bytes(), &regResp)
	assert.Equal(t, 0, regResp.Code)
}

// loginUser logs in and returns the JWT token.
func (env *systemEnv) loginUser(t *testing.T, loginType, target, password string) string {
	t.Helper()

	loginBody, _ := json.Marshal(map[string]string{
		"type":     loginType,
		"target":   target,
		"password": password,
	})
	loginReq := httptest.NewRequest(http.MethodPost, "/api/v1/user/login", bytes.NewBuffer(loginBody))
	loginReq.Header.Set("Content-Type", "application/json")
	loginW := httptest.NewRecorder()
	env.router.ServeHTTP(loginW, loginReq)
	assert.Equal(t, http.StatusOK, loginW.Code)

	var loginResp struct {
		Code int `json:"code"`
		Data struct {
			Token string     `json:"token"`
			User  model.User `json:"user"`
		} `json:"data"`
	}
	json.Unmarshal(loginW.Body.Bytes(), &loginResp)
	assert.Equal(t, 0, loginResp.Code)
	assert.NotEmpty(t, loginResp.Data.Token)

	return loginResp.Data.Token
}

// --- System Tests ---

func TestSystem_RegisterAndLogin(t *testing.T) {
	env := setupSystemEnv(t)
	defer env.mr.Close()

	// Step 1: Register via phone
	env.registerUser(t, "phone", "13900001111", "mypassword", "PhoneUser")

	// Step 2: Login
	token := env.loginUser(t, "phone", "13900001111", "mypassword")

	// Step 3: Get profile
	profileReq := httptest.NewRequest(http.MethodGet, "/api/v1/user/profile", nil)
	profileReq.Header.Set("Authorization", "Bearer "+token)
	profileW := httptest.NewRecorder()
	env.router.ServeHTTP(profileW, profileReq)

	assert.Equal(t, http.StatusOK, profileW.Code)

	var profileResp struct {
		Code int        `json:"code"`
		Data model.User `json:"data"`
	}
	json.Unmarshal(profileW.Body.Bytes(), &profileResp)
	assert.Equal(t, 0, profileResp.Code)
	assert.Equal(t, "PhoneUser", profileResp.Data.Nickname)
	assert.NotNil(t, profileResp.Data.Phone)
	assert.Equal(t, "13900001111", *profileResp.Data.Phone)
}

func TestSystem_RegisterAndLoginEmail(t *testing.T) {
	env := setupSystemEnv(t)
	defer env.mr.Close()

	// Step 1: Register via email
	env.registerUser(t, "email", "test@example.com", "emailpass123", "EmailUser")

	// Step 2: Login
	token := env.loginUser(t, "email", "test@example.com", "emailpass123")

	// Step 3: Get profile
	profileReq := httptest.NewRequest(http.MethodGet, "/api/v1/user/profile", nil)
	profileReq.Header.Set("Authorization", "Bearer "+token)
	profileW := httptest.NewRecorder()
	env.router.ServeHTTP(profileW, profileReq)

	assert.Equal(t, http.StatusOK, profileW.Code)

	var profileResp struct {
		Code int        `json:"code"`
		Data model.User `json:"data"`
	}
	json.Unmarshal(profileW.Body.Bytes(), &profileResp)
	assert.Equal(t, 0, profileResp.Code)
	assert.Equal(t, "EmailUser", profileResp.Data.Nickname)
	assert.NotNil(t, profileResp.Data.Email)
	assert.Equal(t, "test@example.com", *profileResp.Data.Email)
}

func TestSystem_FileListEmpty(t *testing.T) {
	env := setupSystemEnv(t)
	defer env.mr.Close()

	// Register and login
	env.registerUser(t, "phone", "13900002222", "pass123", "ListUser")
	token := env.loginUser(t, "phone", "13900002222", "pass123")

	// List files - should be empty
	listReq := httptest.NewRequest(http.MethodGet, "/api/v1/file/list?parent_path=/&page=1&size=20", nil)
	listReq.Header.Set("Authorization", "Bearer "+token)
	listW := httptest.NewRecorder()
	env.router.ServeHTTP(listW, listReq)

	assert.Equal(t, http.StatusOK, listW.Code)

	var listResp struct {
		Code int `json:"code"`
		Data struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
		} `json:"data"`
	}
	json.Unmarshal(listW.Body.Bytes(), &listResp)
	assert.Equal(t, 0, listResp.Code)
	assert.Equal(t, int64(0), listResp.Data.Total)
}

func TestSystem_ShareFlow(t *testing.T) {
	env := setupSystemEnv(t)
	defer env.mr.Close()

	// Setup: create owner user and file directly in repos
	ownerPassword, _ := pkg.HashPassword("ownerpass")
	ownerPhone := "13900003333"
	env.userRepo.Create(&model.User{
		UserID:   "owner-sys-001",
		Nickname: "Owner",
		Phone:    &ownerPhone,
		Password: ownerPassword,
	})

	env.fileRepo.Create(&model.File{
		FileID:      "file-sys-001",
		UserID:      "owner-sys-001",
		FileName:    "important.pdf",
		FileType:    model.FileTypeFile,
		FileSize:    2048,
		StoragePath: "/storage/important.pdf",
		ParentPath:  "/",
		UniqueFlag:  "unique-sys-001",
	})

	// Setup: create receiver user
	receiverPassword, _ := pkg.HashPassword("receiverpass")
	receiverPhone := "13900004444"
	env.userRepo.Create(&model.User{
		UserID:   "receiver-sys-001",
		Nickname: "Receiver",
		Phone:    &receiverPhone,
		Password: receiverPassword,
	})

	// Step 1: Owner logs in
	ownerToken := env.loginUser(t, "phone", "13900003333", "ownerpass")

	// Step 2: Owner shares the file
	shareBody, _ := json.Marshal(map[string]interface{}{
		"message":      "Here is the file",
		"expire_hours": 24,
		"max_count":    5,
	})
	shareReq := httptest.NewRequest(http.MethodPost, "/api/v1/file/file-sys-001/share", bytes.NewBuffer(shareBody))
	shareReq.Header.Set("Content-Type", "application/json")
	shareReq.Header.Set("Authorization", "Bearer "+ownerToken)
	shareW := httptest.NewRecorder()
	env.router.ServeHTTP(shareW, shareReq)

	assert.Equal(t, http.StatusOK, shareW.Code)

	var shareResp struct {
		Code int         `json:"code"`
		Data model.Share `json:"data"`
	}
	json.Unmarshal(shareW.Body.Bytes(), &shareResp)
	assert.Equal(t, 0, shareResp.Code)
	shareID := shareResp.Data.ShareID
	assert.NotEmpty(t, shareID)

	// Step 3: Receiver logs in
	receiverToken := env.loginUser(t, "phone", "13900004444", "receiverpass")

	// Step 4: Receiver receives the shared file
	recvBody, _ := json.Marshal(map[string]string{
		"save_path": "/received-files",
	})
	recvReq := httptest.NewRequest(http.MethodPost, "/api/v1/share/receive/"+shareID, bytes.NewBuffer(recvBody))
	recvReq.Header.Set("Content-Type", "application/json")
	recvReq.Header.Set("Authorization", "Bearer "+receiverToken)
	recvW := httptest.NewRecorder()
	env.router.ServeHTTP(recvW, recvReq)

	assert.Equal(t, http.StatusOK, recvW.Code)

	var recvResp struct {
		Code int        `json:"code"`
		Data model.File `json:"data"`
	}
	json.Unmarshal(recvW.Body.Bytes(), &recvResp)
	assert.Equal(t, 0, recvResp.Code)
	assert.Equal(t, "important.pdf", recvResp.Data.FileName)
	assert.Equal(t, "/received-files", recvResp.Data.ParentPath)
	assert.Equal(t, model.SourceOthers, recvResp.Data.Source)
	assert.Equal(t, "owner-sys-001", recvResp.Data.SourceUserID)

	// Verify the new file exists in receiver's file list
	listReq := httptest.NewRequest(http.MethodGet, "/api/v1/file/list?parent_path=/received-files&page=1&size=20", nil)
	listReq.Header.Set("Authorization", "Bearer "+receiverToken)
	listW := httptest.NewRecorder()
	env.router.ServeHTTP(listW, listReq)

	assert.Equal(t, http.StatusOK, listW.Code)

	var listResp struct {
		Code int `json:"code"`
		Data struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
		} `json:"data"`
	}
	json.Unmarshal(listW.Body.Bytes(), &listResp)
	assert.Equal(t, 0, listResp.Code)
	assert.Equal(t, int64(1), listResp.Data.Total)
	assert.Equal(t, "important.pdf", listResp.Data.Files[0].FileName)
}

func TestSystem_PublicFlow(t *testing.T) {
	env := setupSystemEnv(t)
	defer env.mr.Close()

	// Setup: create user and file
	userPassword, _ := pkg.HashPassword("publicpass")
	userPhone := "13900005555"
	env.userRepo.Create(&model.User{
		UserID:   "user-pub-sys-001",
		Nickname: "PublicUser",
		Phone:    &userPhone,
		Password: userPassword,
	})

	env.fileRepo.Create(&model.File{
		FileID:      "file-pub-sys-001",
		UserID:      "user-pub-sys-001",
		FileName:    "public-document.pdf",
		FileType:    model.FileTypeFile,
		FileSize:    4096,
		StoragePath: "/storage/public-document.pdf",
		ParentPath:  "/",
	})

	// Step 1: Login
	token := env.loginUser(t, "phone", "13900005555", "publicpass")

	// Step 2: Make file public
	pubBody, _ := json.Marshal(map[string]interface{}{
		"expire_hours": 48,
		"max_count":    100,
	})
	pubReq := httptest.NewRequest(http.MethodPost, "/api/v1/file/file-pub-sys-001/public", bytes.NewBuffer(pubBody))
	pubReq.Header.Set("Content-Type", "application/json")
	pubReq.Header.Set("Authorization", "Bearer "+token)
	pubW := httptest.NewRecorder()
	env.router.ServeHTTP(pubW, pubReq)

	assert.Equal(t, http.StatusOK, pubW.Code)

	var pubResp apiResponse
	json.Unmarshal(pubW.Body.Bytes(), &pubResp)
	assert.Equal(t, 0, pubResp.Code)

	// Step 3: List public files (no auth required)
	listReq := httptest.NewRequest(http.MethodGet, "/api/v1/file/public?page=1&size=20", nil)
	listW := httptest.NewRecorder()
	env.router.ServeHTTP(listW, listReq)

	assert.Equal(t, http.StatusOK, listW.Code)

	var listResp struct {
		Code int `json:"code"`
		Data struct {
			Files []model.File `json:"files"`
			Total int64        `json:"total"`
		} `json:"data"`
	}
	json.Unmarshal(listW.Body.Bytes(), &listResp)
	assert.Equal(t, 0, listResp.Code)
	assert.Equal(t, int64(1), listResp.Data.Total)
	assert.Len(t, listResp.Data.Files, 1)
	assert.Equal(t, "public-document.pdf", listResp.Data.Files[0].FileName)
	assert.True(t, listResp.Data.Files[0].IsPublic)
}

// Suppress unused import warning for time package
var _ = time.Now
