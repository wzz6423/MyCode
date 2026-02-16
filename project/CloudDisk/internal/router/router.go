package router

import (
	"net/http"
	"path/filepath"

	"clouddisk/internal/config"
	"clouddisk/internal/handler"
	"clouddisk/internal/middleware"
	"clouddisk/internal/repository"
	"clouddisk/internal/service"

	"github.com/gin-gonic/gin"
	"github.com/redis/go-redis/v9"
	"gorm.io/gorm"
)

func Setup(r *gin.Engine, cfg *config.Config, db *gorm.DB, rdb *redis.Client) {
	// Repositories
	userRepo := repository.NewUserRepository(db)
	fileRepo := repository.NewFileRepository(db)
	shareRepo := repository.NewShareRepository(db)

	esRepo, err := repository.NewESRepository(
		cfg.Elasticsearch.Addresses,
		cfg.Elasticsearch.Username,
		cfg.Elasticsearch.Password,
		cfg.Elasticsearch.Index,
	)
	if err != nil {
		// ES is optional, log and continue without search
		esRepo = nil
	}

	// Services
	verifySvc := service.NewVerifyService(rdb, cfg)
	userSvc := service.NewUserService(userRepo, fileRepo, shareRepo, verifySvc, cfg)
	fileSvc := service.NewFileService(fileRepo, esRepo)
	shareSvc := service.NewShareService(fileRepo, shareRepo, cfg)

	// Handlers
	userHandler := handler.NewUserHandler(userSvc, cfg)
	fileHandler := handler.NewFileHandler(fileSvc, shareRepo, userRepo, cfg)
	shareHandler := handler.NewShareHandler(shareSvc)

	v1 := r.Group("/api/v1")

	// Public user routes (no auth)
	user := v1.Group("/user")
	{
		user.POST("/verify/code", userHandler.GetVerifyCode)
		user.POST("/register", userHandler.Register)
		user.POST("/login", userHandler.Login)
	}

	// Public file routes (no auth)
	v1.GET("/file/public", fileHandler.PublicList)
	v1.GET("/share/:share_id", shareHandler.GetShareInfo)

	// Auth-required routes
	auth := v1.Group("")
	auth.Use(middleware.JWTAuth(cfg.JWT.Secret))
	{
		// User profile routes
		u := auth.Group("/user")
		{
			u.GET("/profile", userHandler.GetProfile)
			u.PUT("/avatar", userHandler.SetAvatar)
			u.PUT("/nickname", userHandler.SetNickname)
			u.PUT("/description", userHandler.SetDescription)
			u.PUT("/password", userHandler.SetPassword)
			u.PUT("/phone", userHandler.SetPhone)
			u.PUT("/email", userHandler.SetEmail)
			u.DELETE("/account", userHandler.DeleteAccount)
		}

		// File routes
		f := auth.Group("/file")
		{
			f.POST("/upload", fileHandler.Upload)
			f.GET("/download/:file_id", fileHandler.Download)
			f.DELETE("/:file_id", fileHandler.Delete)
			f.GET("/list", fileHandler.List)
			f.GET("/search", fileHandler.Search)
			f.POST("/folder", fileHandler.CreateFolder)
			f.POST("/batch-download", fileHandler.BatchDownload)

			// Share sub-routes
			f.POST("/:file_id/share", shareHandler.Share)
			f.DELETE("/:file_id/share", shareHandler.Unshare)
			f.POST("/:file_id/public", shareHandler.Public)
			f.DELETE("/:file_id/public", shareHandler.Unpublic)
			f.POST("/receive/:share_id", shareHandler.Receive)
		}
	}

	// Serve avatar files
	r.Static("/avatars", filepath.Join(cfg.Server.UploadDir, "avatars"))

	// Serve frontend static files
	r.NoRoute(func(c *gin.Context) {
		c.File("./web/dist/index.html")
	})
	r.Static("/assets", "./web/dist/assets")
	r.StaticFile("/favicon.ico", "./web/dist/favicon.ico")

	// Health check
	r.GET("/health", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{"status": "ok"})
	})
}
