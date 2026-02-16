package pkg

import (
	"fmt"
	"net/url"
	"path/filepath"
	"strings"
	"unicode"
)

// Dangerous file extensions that could be executed on server or client.
var blockedExtensions = map[string]bool{
	".exe": true, ".bat": true, ".cmd": true, ".com": true, ".msi": true,
	".scr": true, ".pif": true, ".vbs": true, ".vbe": true, ".ws": true,
	".wsf": true, ".wsc": true, ".wsh": true, ".ps1": true, ".psm1": true,
	".sh": true, ".bash": true, ".csh": true, ".ksh": true,
	".php": true, ".phtml": true, ".php3": true, ".php4": true, ".php5": true,
	".jsp": true, ".jspx": true, ".asp": true, ".aspx": true, ".ashx": true,
	".cgi": true, ".pl": true, ".py": true, ".rb": true,
	".dll": true, ".so": true, ".dylib": true,
	".htaccess": true, ".htpasswd": true,
	".jar": true, ".war": true, ".class": true,
}

var allowedImageExtensions = map[string]bool{
	".jpg": true, ".jpeg": true, ".png": true, ".gif": true,
	".bmp": true, ".webp": true, ".svg": true, ".ico": true,
}

// IsBlockedExtension returns true if the file extension is dangerous.
func IsBlockedExtension(filename string) bool {
	ext := strings.ToLower(filepath.Ext(filename))
	return blockedExtensions[ext]
}

// IsAllowedImageExt returns true if the file extension is a safe image type.
func IsAllowedImageExt(filename string) bool {
	ext := strings.ToLower(filepath.Ext(filename))
	return allowedImageExtensions[ext]
}

// SanitizeFileName removes dangerous characters from a filename.
// Strips path separators, null bytes, control characters, and leading dots.
func SanitizeFileName(name string) string {
	// Remove null bytes
	name = strings.ReplaceAll(name, "\x00", "")
	// Remove path separators
	name = strings.ReplaceAll(name, "/", "")
	name = strings.ReplaceAll(name, "\\", "")
	// Remove control characters
	name = strings.Map(func(r rune) rune {
		if unicode.IsControl(r) {
			return -1
		}
		return r
	}, name)
	// Remove leading dots to prevent hidden files / directory traversal
	name = strings.TrimLeft(name, ".")
	// Trim whitespace
	name = strings.TrimSpace(name)
	if name == "" {
		name = "unnamed"
	}
	return name
}

// ValidatePath checks that a path is safe (no traversal, no null bytes).
func ValidatePath(path string) error {
	if strings.Contains(path, "\x00") {
		return fmt.Errorf("path contains null byte")
	}
	if strings.Contains(path, "..") {
		return fmt.Errorf("path contains directory traversal")
	}
	// Must start with /
	if !strings.HasPrefix(path, "/") {
		return fmt.Errorf("path must start with /")
	}
	return nil
}

// SafeContentDisposition returns a properly encoded Content-Disposition header value.
// Uses RFC 5987 encoding for non-ASCII filenames.
func SafeContentDisposition(disposition, filename string) string {
	// URL-encode the filename for the filename* parameter (RFC 5987)
	encoded := url.PathEscape(filename)
	return fmt.Sprintf(`%s; filename="%s"; filename*=UTF-8''%s`, disposition, encoded, encoded)
}

// EscapeLikePattern escapes SQL LIKE wildcards (% and _) in a string.
func EscapeLikePattern(pattern string) string {
	pattern = strings.ReplaceAll(pattern, "\\", "\\\\")
	pattern = strings.ReplaceAll(pattern, "%", "\\%")
	pattern = strings.ReplaceAll(pattern, "_", "\\_")
	return pattern
}
