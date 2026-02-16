package pkg

import (
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"testing"

	"github.com/gin-gonic/gin"
	"github.com/stretchr/testify/assert"
)

// ==================== Hash Tests ====================

func TestHashPassword_And_CheckPassword(t *testing.T) {
	t.Run("correct password", func(t *testing.T) {
		hash, err := HashPassword("mypassword")
		assert.NoError(t, err)
		assert.NotEmpty(t, hash)
		assert.True(t, CheckPassword("mypassword", hash))
	})

	t.Run("wrong password", func(t *testing.T) {
		hash, err := HashPassword("mypassword")
		assert.NoError(t, err)
		assert.False(t, CheckPassword("wrongpassword", hash))
	})

	t.Run("empty password", func(t *testing.T) {
		hash, err := HashPassword("")
		assert.NoError(t, err)
		assert.NotEmpty(t, hash)
		assert.True(t, CheckPassword("", hash))
		assert.False(t, CheckPassword("notempty", hash))
	})

	t.Run("very long password 500 chars", func(t *testing.T) {
		longPwd := strings.Repeat("a", 500)
		// bcrypt rejects passwords exceeding 72 bytes
		_, err := HashPassword(longPwd)
		assert.Error(t, err)
	})

	t.Run("max bcrypt length password 72 chars", func(t *testing.T) {
		pwd72 := strings.Repeat("b", 72)
		hash, err := HashPassword(pwd72)
		assert.NoError(t, err)
		assert.NotEmpty(t, hash)
		assert.True(t, CheckPassword(pwd72, hash))
	})

	t.Run("unicode password", func(t *testing.T) {
		unicodePwd := "密码测试🔑パスワード"
		hash, err := HashPassword(unicodePwd)
		assert.NoError(t, err)
		assert.NotEmpty(t, hash)
		assert.True(t, CheckPassword(unicodePwd, hash))
		assert.False(t, CheckPassword("different", hash))
	})

	t.Run("different hashes for same password", func(t *testing.T) {
		hash1, err1 := HashPassword("samepassword")
		hash2, err2 := HashPassword("samepassword")
		assert.NoError(t, err1)
		assert.NoError(t, err2)
		assert.NotEqual(t, hash1, hash2) // bcrypt uses random salt
		assert.True(t, CheckPassword("samepassword", hash1))
		assert.True(t, CheckPassword("samepassword", hash2))
	})

	t.Run("check against invalid hash", func(t *testing.T) {
		assert.False(t, CheckPassword("password", "not-a-valid-hash"))
	})
}

// ==================== JWT Tests ====================

func TestGenerateToken_And_ParseToken(t *testing.T) {
	secret := "test-secret-key-12345"

	t.Run("valid token", func(t *testing.T) {
		token, err := GenerateToken("user-abc-123", secret, 24)
		assert.NoError(t, err)
		assert.NotEmpty(t, token)

		claims, err := ParseToken(token, secret)
		assert.NoError(t, err)
		assert.NotNil(t, claims)
		assert.Equal(t, "user-abc-123", claims.UserID)
	})

	t.Run("wrong secret", func(t *testing.T) {
		token, err := GenerateToken("user-abc-123", secret, 24)
		assert.NoError(t, err)

		_, err = ParseToken(token, "wrong-secret")
		assert.Error(t, err)
	})

	t.Run("empty userID", func(t *testing.T) {
		token, err := GenerateToken("", secret, 24)
		assert.NoError(t, err)
		assert.NotEmpty(t, token)

		claims, err := ParseToken(token, secret)
		assert.NoError(t, err)
		assert.Equal(t, "", claims.UserID)
	})

	t.Run("tampered token", func(t *testing.T) {
		token, err := GenerateToken("user-abc-123", secret, 24)
		assert.NoError(t, err)

		// Tamper with the token by modifying a character
		tampered := token[:len(token)-5] + "XXXXX"
		_, err = ParseToken(tampered, secret)
		assert.Error(t, err)
	})

	t.Run("completely invalid token string", func(t *testing.T) {
		_, err := ParseToken("not.a.valid.jwt.token", secret)
		assert.Error(t, err)
	})

	t.Run("empty token string", func(t *testing.T) {
		_, err := ParseToken("", secret)
		assert.Error(t, err)
	})

	t.Run("token signed with different secret fails", func(t *testing.T) {
		token, err := GenerateToken("user-123", "secret-A", 24)
		assert.NoError(t, err)

		_, err = ParseToken(token, "secret-B")
		assert.Error(t, err)
	})
}

// ==================== UUID Tests ====================

func TestNewUUID(t *testing.T) {
	t.Run("uniqueness 1000 UUIDs", func(t *testing.T) {
		seen := make(map[string]bool, 1000)
		for i := 0; i < 1000; i++ {
			id := NewUUID()
			assert.False(t, seen[id], "duplicate UUID found: %s", id)
			seen[id] = true
		}
	})

	t.Run("UUID v4 format", func(t *testing.T) {
		uuidRegex := regexp.MustCompile(`^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$`)
		for i := 0; i < 100; i++ {
			id := NewUUID()
			assert.Regexp(t, uuidRegex, id, "UUID does not match v4 format: %s", id)
		}
	})

	t.Run("not empty", func(t *testing.T) {
		id := NewUUID()
		assert.NotEmpty(t, id)
		assert.Len(t, id, 36) // standard UUID string length
	})
}

// ==================== DetectFileType Tests ====================

func TestDetectFileType(t *testing.T) {
	t.Run("image extensions", func(t *testing.T) {
		for _, ext := range []string{".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp", ".svg"} {
			assert.Equal(t, uint8(2), DetectFileType("photo"+ext), "expected IMAGE for %s", ext)
		}
	})

	t.Run("audio extensions", func(t *testing.T) {
		for _, ext := range []string{".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma"} {
			assert.Equal(t, uint8(3), DetectFileType("song"+ext), "expected AUDIO for %s", ext)
		}
	})

	t.Run("video extensions", func(t *testing.T) {
		for _, ext := range []string{".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm"} {
			assert.Equal(t, uint8(4), DetectFileType("movie"+ext), "expected VIDEO for %s", ext)
		}
	})

	t.Run("compress extensions", func(t *testing.T) {
		for _, ext := range []string{".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz"} {
			assert.Equal(t, uint8(5), DetectFileType("archive"+ext), "expected COMPRESS for %s", ext)
		}
	})

	t.Run("unknown extension", func(t *testing.T) {
		assert.Equal(t, uint8(0), DetectFileType("readme.txt"))
		assert.Equal(t, uint8(0), DetectFileType("document.pdf"))
		assert.Equal(t, uint8(0), DetectFileType("script.go"))
	})

	t.Run("case insensitive", func(t *testing.T) {
		assert.Equal(t, uint8(2), DetectFileType("photo.JPG"))
		assert.Equal(t, uint8(2), DetectFileType("photo.Png"))
		assert.Equal(t, uint8(3), DetectFileType("song.MP3"))
		assert.Equal(t, uint8(4), DetectFileType("video.AVI"))
		assert.Equal(t, uint8(5), DetectFileType("archive.ZIP"))
	})

	t.Run("no extension", func(t *testing.T) {
		assert.Equal(t, uint8(0), DetectFileType("Makefile"))
		assert.Equal(t, uint8(0), DetectFileType("README"))
	})

	t.Run("dot only filename", func(t *testing.T) {
		assert.Equal(t, uint8(0), DetectFileType("."))
		assert.Equal(t, uint8(0), DetectFileType(".."))
	})

	t.Run("hidden file with extension", func(t *testing.T) {
		assert.Equal(t, uint8(2), DetectFileType(".hidden.jpg"))
	})
}

// ==================== Response Tests ====================

func TestSuccess(t *testing.T) {
	gin.SetMode(gin.TestMode)
	w := httptest.NewRecorder()
	c, _ := gin.CreateTestContext(w)

	Success(c, map[string]string{"key": "value"})

	assert.Equal(t, http.StatusOK, w.Code)

	var resp Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, "success", resp.Message)
	assert.NotNil(t, resp.Data)
}

func TestSuccess_NilData(t *testing.T) {
	gin.SetMode(gin.TestMode)
	w := httptest.NewRecorder()
	c, _ := gin.CreateTestContext(w)

	Success(c, nil)

	assert.Equal(t, http.StatusOK, w.Code)

	var resp Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, "success", resp.Message)
	assert.Nil(t, resp.Data)
}

func TestError(t *testing.T) {
	gin.SetMode(gin.TestMode)
	w := httptest.NewRecorder()
	c, _ := gin.CreateTestContext(w)

	Error(c, http.StatusBadRequest, "something went wrong")

	assert.Equal(t, http.StatusBadRequest, w.Code)

	var resp Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, -1, resp.Code)
	assert.Equal(t, "something went wrong", resp.Message)
}

func TestError_Unauthorized(t *testing.T) {
	gin.SetMode(gin.TestMode)
	w := httptest.NewRecorder()
	c, _ := gin.CreateTestContext(w)

	Error(c, http.StatusUnauthorized, "not authorized")

	assert.Equal(t, http.StatusUnauthorized, w.Code)

	var resp Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, -1, resp.Code)
	assert.Equal(t, "not authorized", resp.Message)
}

func TestErrorWithCode(t *testing.T) {
	gin.SetMode(gin.TestMode)
	w := httptest.NewRecorder()
	c, _ := gin.CreateTestContext(w)

	ErrorWithCode(c, http.StatusForbidden, 1001, "custom error")

	assert.Equal(t, http.StatusForbidden, w.Code)

	var resp Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 1001, resp.Code)
	assert.Equal(t, "custom error", resp.Message)
}

func TestErrorWithCode_ZeroCode(t *testing.T) {
	gin.SetMode(gin.TestMode)
	w := httptest.NewRecorder()
	c, _ := gin.CreateTestContext(w)

	ErrorWithCode(c, http.StatusInternalServerError, 0, "server error")

	assert.Equal(t, http.StatusInternalServerError, w.Code)

	var resp Response
	err := json.Unmarshal(w.Body.Bytes(), &resp)
	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Code)
	assert.Equal(t, "server error", resp.Message)
}

// ==================== FileExists Tests ====================

func TestFileExists(t *testing.T) {
	t.Run("existing file", func(t *testing.T) {
		tmpFile := filepath.Join(os.TempDir(), "pkg_test_exists.txt")
		err := os.WriteFile(tmpFile, []byte("hello"), 0644)
		assert.NoError(t, err)
		defer os.Remove(tmpFile)

		assert.True(t, FileExists(tmpFile))
	})

	t.Run("non existing file", func(t *testing.T) {
		assert.False(t, FileExists(filepath.Join(os.TempDir(), "this_file_does_not_exist_at_all.xyz")))
	})

	t.Run("existing directory", func(t *testing.T) {
		assert.True(t, FileExists(os.TempDir()))
	})
}

// ==================== SaveUploadedFile + DeleteFile Tests ====================

func TestDeleteFile(t *testing.T) {
	t.Run("delete existing file", func(t *testing.T) {
		tmpFile := filepath.Join(os.TempDir(), "pkg_test_delete.txt")
		err := os.WriteFile(tmpFile, []byte("to be deleted"), 0644)
		assert.NoError(t, err)

		assert.True(t, FileExists(tmpFile))

		err = DeleteFile(tmpFile)
		assert.NoError(t, err)

		assert.False(t, FileExists(tmpFile))
	})

	t.Run("delete non existing file", func(t *testing.T) {
		err := DeleteFile(filepath.Join(os.TempDir(), "nonexistent_delete_target.xyz"))
		assert.Error(t, err)
	})
}
