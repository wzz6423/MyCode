package config

import (
	"os"
	"path/filepath"
	"strings"

	"gopkg.in/yaml.v3"
)

type Config struct {
	Server        ServerConfig        `yaml:"server"`
	JWT           JWTConfig           `yaml:"jwt"`
	MySQL         MySQLConfig         `yaml:"mysql"`
	Redis         RedisConfig         `yaml:"redis"`
	Elasticsearch ElasticsearchConfig `yaml:"elasticsearch"`
	SMS           SMSConfig           `yaml:"sms"`
	Email         EmailConfig         `yaml:"email"`
	VerifyCode    VerifyCodeConfig    `yaml:"verify_code"`
}

type ServerConfig struct {
	Port          int    `yaml:"port"`
	Mode          string `yaml:"mode"`
	UploadDir     string `yaml:"upload_dir"`
	MaxUploadSize int64  `yaml:"max_upload_size"`
}

type JWTConfig struct {
	Secret      string `yaml:"secret"`
	ExpireHours int    `yaml:"expire_hours"`
}

type MySQLConfig struct {
	Host         string `yaml:"host"`
	Port         int    `yaml:"port"`
	User         string `yaml:"user"`
	Password     string `yaml:"password"`
	Database     string `yaml:"database"`
	MaxOpenConns int    `yaml:"max_open_conns"`
	MaxIdleConns int    `yaml:"max_idle_conns"`
}

type RedisConfig struct {
	Addr     string `yaml:"addr"`
	Password string `yaml:"password"`
	DB       int    `yaml:"db"`
}

type ElasticsearchConfig struct {
	Addresses []string `yaml:"addresses"`
	Username  string   `yaml:"username"`
	Password  string   `yaml:"password"`
	Index     string   `yaml:"index"`
}

type SMSConfig struct {
	AccessKeyID     string `yaml:"access_key_id"`
	AccessKeySecret string `yaml:"access_key_secret"`
	SignName        string `yaml:"sign_name"`
	TemplateCode    string `yaml:"template_code"`
}

type EmailConfig struct {
	SMTPHost string `yaml:"smtp_host"`
	SMTPPort int    `yaml:"smtp_port"`
	Username string `yaml:"username"`
	Password string `yaml:"password"`
	From     string `yaml:"from"`
}

type VerifyCodeConfig struct {
	Length        int `yaml:"length"`
	ExpireMinutes int `yaml:"expire_minutes"`
}

func Load(path string) (*Config, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	var cfg Config
	if err := yaml.Unmarshal(data, &cfg); err != nil {
		return nil, err
	}

	// Resolve upload_dir to absolute path
	if cfg.Server.UploadDir != "" {
		if strings.HasPrefix(cfg.Server.UploadDir, "~") {
			home, err := os.UserHomeDir()
			if err == nil {
				cfg.Server.UploadDir = filepath.Join(home, cfg.Server.UploadDir[1:])
			}
		} else if !filepath.IsAbs(cfg.Server.UploadDir) {
			wd, err := os.Getwd()
			if err == nil {
				cfg.Server.UploadDir = filepath.Join(wd, cfg.Server.UploadDir)
			}
		}
	}

	return &cfg, nil
}
