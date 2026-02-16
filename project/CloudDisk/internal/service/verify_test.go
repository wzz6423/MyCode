package service

import (
	"context"
	"fmt"
	"testing"

	"clouddisk/internal/config"

	"github.com/alicebob/miniredis/v2"
	"github.com/redis/go-redis/v9"
	"github.com/stretchr/testify/assert"
)

func setupVerifyService(t *testing.T) (*VerifyService, *miniredis.Miniredis) {
	t.Helper()
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	cfg := &config.Config{
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 10,
		},
	}

	svc := NewVerifyService(rdb, cfg)
	return svc, mr
}

func TestSendCode_Phone(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	codeID, err := svc.SendCode("phone", "13800138000")
	assert.NoError(t, err)
	assert.NotEmpty(t, codeID)

	// Verify the code is stored in Redis
	key := fmt.Sprintf("verify:%s", codeID)
	val, err := mr.Get(key)
	assert.NoError(t, err)
	assert.Len(t, val, 6)
}

func TestSendCode_Email(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	codeID, err := svc.SendCode("email", "test@example.com")
	assert.NoError(t, err)
	assert.NotEmpty(t, codeID)

	// Verify the code is stored in Redis
	key := fmt.Sprintf("verify:%s", codeID)
	val, err := mr.Get(key)
	assert.NoError(t, err)
	assert.Len(t, val, 6)
}

func TestSendCode_UnsupportedType(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	_, err := svc.SendCode("fax", "12345")
	assert.Error(t, err)
	assert.Contains(t, err.Error(), "unsupported verify code type")
}

func TestValidateCode_Success(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	// Manually set a code in Redis
	codeID := "test-code-id"
	code := "123456"
	key := fmt.Sprintf("verify:%s", codeID)
	mr.Set(key, code)

	// Validate should succeed
	ok := svc.ValidateCode(codeID, code)
	assert.True(t, ok)

	// After validation, the key should be deleted
	_, err := mr.Get(key)
	assert.Error(t, err) // key no longer exists
}

func TestValidateCode_Wrong(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	codeID := "test-code-id"
	code := "123456"
	key := fmt.Sprintf("verify:%s", codeID)
	mr.Set(key, code)

	// Wrong code should fail
	ok := svc.ValidateCode(codeID, "999999")
	assert.False(t, ok)

	// Key should still exist (not deleted on wrong code)
	val, err := mr.Get(key)
	assert.NoError(t, err)
	assert.Equal(t, code, val)
}

func TestValidateCode_Expired(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	// Don't set any code in Redis - simulates expired/missing code
	ok := svc.ValidateCode("nonexistent-id", "123456")
	assert.False(t, ok)
}

func TestSendCode_StoresWithTTL(t *testing.T) {
	svc, mr := setupVerifyService(t)
	defer mr.Close()

	codeID, err := svc.SendCode("phone", "13800138000")
	assert.NoError(t, err)

	key := fmt.Sprintf("verify:%s", codeID)

	// Verify TTL is set
	ttl := mr.TTL(key)
	assert.True(t, ttl > 0, "TTL should be positive")
}

func TestValidateCode_RedisDown(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	cfg := &config.Config{
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 10,
		},
	}

	svc := NewVerifyService(rdb, cfg)

	// Close Redis to simulate connection failure
	mr.Close()

	ok := svc.ValidateCode("some-id", "123456")
	assert.False(t, ok)
}

func TestSendCode_DefaultLength(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}
	defer mr.Close()

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	// Config with zero length - should default to 6
	cfg := &config.Config{
		VerifyCode: config.VerifyCodeConfig{
			Length:        0,
			ExpireMinutes: 10,
		},
	}

	svc := NewVerifyService(rdb, cfg)
	codeID, err := svc.SendCode("phone", "13800138000")
	assert.NoError(t, err)

	key := fmt.Sprintf("verify:%s", codeID)
	val, err := mr.Get(key)
	assert.NoError(t, err)
	assert.Len(t, val, 6)
}

func TestSendCode_DefaultTTL(t *testing.T) {
	mr, err := miniredis.Run()
	if err != nil {
		t.Fatalf("failed to start miniredis: %v", err)
	}
	defer mr.Close()

	rdb := redis.NewClient(&redis.Options{
		Addr: mr.Addr(),
	})

	// Config with zero expire - should default to 10 minutes
	cfg := &config.Config{
		VerifyCode: config.VerifyCodeConfig{
			Length:        6,
			ExpireMinutes: 0,
		},
	}

	svc := NewVerifyService(rdb, cfg)
	codeID, err := svc.SendCode("email", "test@example.com")
	assert.NoError(t, err)

	key := fmt.Sprintf("verify:%s", codeID)
	ttl := mr.TTL(key)
	assert.True(t, ttl > 0)

	// Verify the code exists
	rdbClient := redis.NewClient(&redis.Options{Addr: mr.Addr()})
	val, err := rdbClient.Get(context.Background(), key).Result()
	assert.NoError(t, err)
	assert.NotEmpty(t, val)
}
