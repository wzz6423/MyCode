package middleware

import (
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"

	"clouddisk/internal/pkg"

	"github.com/gin-gonic/gin"
	"github.com/stretchr/testify/assert"
)

func init() {
	gin.SetMode(gin.TestMode)
}

const testSecret = "middleware-test-secret"

func setupRouter(secret string) *gin.Engine {
	r := gin.New()
	r.GET("/protected", JWTAuth(secret), func(c *gin.Context) {
		userID, _ := c.Get("user_id")
		c.JSON(http.StatusOK, gin.H{"user_id": userID})
	})
	return r
}

func TestJWTAuth_ValidToken(t *testing.T) {
	router := setupRouter(testSecret)

	token, err := pkg.GenerateToken("user-123", testSecret, 24)
	assert.NoError(t, err)

	w := httptest.NewRecorder()
	req, _ := http.NewRequest("GET", "/protected", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusOK, w.Code)

	var body map[string]interface{}
	err = json.Unmarshal(w.Body.Bytes(), &body)
	assert.NoError(t, err)
	assert.Equal(t, "user-123", body["user_id"])
}

func TestJWTAuth_MissingAuthorizationHeader(t *testing.T) {
	router := setupRouter(testSecret)

	w := httptest.NewRecorder()
	req, _ := http.NewRequest("GET", "/protected", nil)
	// No Authorization header
	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp pkg.Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Contains(t, resp.Message, "missing authorization header")
}

func TestJWTAuth_NoBearerPrefix(t *testing.T) {
	router := setupRouter(testSecret)

	token, err := pkg.GenerateToken("user-123", testSecret, 24)
	assert.NoError(t, err)

	w := httptest.NewRecorder()
	req, _ := http.NewRequest("GET", "/protected", nil)
	req.Header.Set("Authorization", "Token "+token) // wrong prefix
	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp pkg.Response
	err = json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Contains(t, resp.Message, "invalid authorization format")
}

func TestJWTAuth_InvalidToken(t *testing.T) {
	router := setupRouter(testSecret)

	w := httptest.NewRecorder()
	req, _ := http.NewRequest("GET", "/protected", nil)
	req.Header.Set("Authorization", "Bearer invalid.token.string")
	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp pkg.Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Contains(t, resp.Message, "invalid or expired token")
}

func TestJWTAuth_EmptyBearerToken(t *testing.T) {
	router := setupRouter(testSecret)

	w := httptest.NewRecorder()
	req, _ := http.NewRequest("GET", "/protected", nil)
	req.Header.Set("Authorization", "Bearer ") // empty token after Bearer
	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp pkg.Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Contains(t, resp.Message, "invalid or expired token")
}

func TestJWTAuth_TokenSignedWithDifferentSecret(t *testing.T) {
	router := setupRouter(testSecret)

	// Generate token with a different secret
	token, err := pkg.GenerateToken("user-123", "completely-different-secret", 24)
	assert.NoError(t, err)

	w := httptest.NewRecorder()
	req, _ := http.NewRequest("GET", "/protected", nil)
	req.Header.Set("Authorization", "Bearer "+token)
	router.ServeHTTP(w, req)

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp pkg.Response
	err = json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Contains(t, resp.Message, "invalid or expired token")
}
