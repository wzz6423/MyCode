package middleware

import (
	"net/http"
	"strings"

	"clouddisk/internal/pkg"

	"github.com/gin-gonic/gin"
)

func JWTAuth(secret string) gin.HandlerFunc {
	return func(c *gin.Context) {
		var token string
		auth := c.GetHeader("Authorization")
		if auth != "" {
			t := strings.TrimPrefix(auth, "Bearer ")
			if t != auth {
				token = t
			}
		}
		// Fallback: accept token from query parameter (for download/preview links)
		if token == "" {
			token = c.Query("token")
		}
		if token == "" {
			pkg.Error(c, http.StatusUnauthorized, "missing authorization")
			c.Abort()
			return
		}
		claims, err := pkg.ParseToken(token, secret)
		if err != nil {
			pkg.Error(c, http.StatusUnauthorized, "invalid or expired token")
			c.Abort()
			return
		}
		c.Set("user_id", claims.UserID)
		c.Next()
	}
}
