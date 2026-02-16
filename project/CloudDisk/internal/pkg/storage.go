package pkg

import (
	"io"
	"mime/multipart"
	"os"
	"path/filepath"
	"strings"
)

func SaveUploadedFile(file *multipart.FileHeader, dst string) error {
	if err := os.MkdirAll(filepath.Dir(dst), 0755); err != nil {
		return err
	}
	src, err := file.Open()
	if err != nil {
		return err
	}
	defer src.Close()

	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer out.Close()

	_, err = io.Copy(out, src)
	return err
}

func DeleteFile(path string) error {
	return os.Remove(path)
}

func FileExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

func ReadFileBytes(path string) ([]byte, error) {
	return os.ReadFile(path)
}

func CopyFile(src, dst string) error {
	if err := os.MkdirAll(filepath.Dir(dst), 0755); err != nil {
		return err
	}
	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()
	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer out.Close()
	_, err = io.Copy(out, in)
	return err
}

func DetectFileType(filename string) uint8 {
	ext := strings.ToLower(filepath.Ext(filename))
	switch ext {
	case ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp", ".svg":
		return 2 // IMAGE
	case ".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma":
		return 3 // AUDIO
	case ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm":
		return 4 // VIDEO
	case ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz":
		return 5 // COMPRESS
	default:
		return 0 // FILE
	}
}
