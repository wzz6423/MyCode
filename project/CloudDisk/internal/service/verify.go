package service

import (
	"context"
	"fmt"
	"log"
	"math/rand"
	"time"

	"clouddisk/internal/config"
	"clouddisk/internal/pkg"

	"github.com/redis/go-redis/v9"
)

type VerifyService struct {
	rdb *redis.Client
	cfg *config.Config
}

func NewVerifyService(rdb *redis.Client, cfg *config.Config) *VerifyService {
	return &VerifyService{rdb: rdb, cfg: cfg}
}

func (s *VerifyService) SendCode(codeType, target string) (string, error) {
	length := s.cfg.VerifyCode.Length
	if length <= 0 {
		length = 6
	}

	code := generateCode(length)
	codeID := pkg.NewUUID()

	key := fmt.Sprintf("verify:%s", codeID)
	ttl := time.Duration(s.cfg.VerifyCode.ExpireMinutes) * time.Minute
	if ttl <= 0 {
		ttl = 10 * time.Minute
	}

	ctx := context.Background()
	if err := s.rdb.Set(ctx, key, code, ttl).Err(); err != nil {
		return "", fmt.Errorf("failed to store verify code: %w", err)
	}

	switch codeType {
	case "phone":
		log.Printf("[SMS] sending verify code %s to phone %s (not implemented)", code, target)
	case "email":
		emailCfg := pkg.EmailConfig{
			SMTPHost: s.cfg.Email.SMTPHost,
			SMTPPort: s.cfg.Email.SMTPPort,
			Username: s.cfg.Email.Username,
			Password: s.cfg.Email.Password,
			From:     s.cfg.Email.From,
		}
		subject := "CloudDisk 验证码"
		body := fmt.Sprintf(
			`<div style="font-family:sans-serif;max-width:400px;margin:0 auto;padding:20px">
				<h2 style="color:#356bfb">CloudDisk</h2>
				<p>您的验证码是：</p>
				<p style="font-size:28px;font-weight:bold;letter-spacing:6px;color:#356bfb">%s</p>
				<p>验证码 %d 分钟内有效，请勿泄露给他人。</p>
			</div>`, code, s.cfg.VerifyCode.ExpireMinutes)
		if err := pkg.SendEmail(emailCfg, target, subject, body); err != nil {
			log.Printf("[Email] failed to send verify code to %s: %v", target, err)
			return "", fmt.Errorf("failed to send email: %w", err)
		}
		log.Printf("[Email] verify code sent to %s", target)
	default:
		return "", fmt.Errorf("unsupported verify code type: %s", codeType)
	}

	return codeID, nil
}

func (s *VerifyService) ValidateCode(codeID, code string) bool {
	key := fmt.Sprintf("verify:%s", codeID)
	ctx := context.Background()

	stored, err := s.rdb.Get(ctx, key).Result()
	if err != nil {
		return false
	}

	if stored != code {
		return false
	}

	s.rdb.Del(ctx, key)
	return true
}

func generateCode(length int) string {
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	digits := make([]byte, length)
	for i := range digits {
		digits[i] = '0' + byte(r.Intn(10))
	}
	return string(digits)
}
