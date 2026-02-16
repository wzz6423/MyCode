package handler

import (
	"archive/zip"
	"fmt"
	"net/http"
	"strconv"
	"time"

	"clouddisk/internal/config"
	"clouddisk/internal/model"
	"clouddisk/internal/pkg"
	"clouddisk/internal/repository"
	"clouddisk/internal/service"

	"github.com/gin-gonic/gin"
)

type fileResponse struct {
	model.File
	IsDir    bool   `json:"is_dir"`
	ShareID  string `json:"share_id,omitempty"`
	PublicID string `json:"public_id,omitempty"`
}

type FileHandler struct {
	fileService *service.FileService
	shareRepo   repository.ShareRepository
	userRepo    repository.UserRepository
	cfg         *config.Config
}

func NewFileHandler(fileService *service.FileService, shareRepo repository.ShareRepository, userRepo repository.UserRepository, cfg *config.Config) *FileHandler {
	return &FileHandler{fileService: fileService, shareRepo: shareRepo, userRepo: userRepo, cfg: cfg}
}

func (h *FileHandler) enrichFiles(files []model.File) []fileResponse {
	fileIDs := make([]string, len(files))
	for i, f := range files {
		fileIDs[i] = f.FileID
	}
	shares, _ := h.shareRepo.GetByFileIDs(fileIDs)

	shareMap := make(map[string]string)
	publicMap := make(map[string]string)
	for _, s := range shares {
		if s.ShareType == model.ShareTypeShare {
			shareMap[s.FileID] = s.ShareID
		} else if s.ShareType == model.ShareTypePublic {
			publicMap[s.FileID] = s.ShareID
		}
	}

	result := make([]fileResponse, len(files))
	for i, f := range files {
		result[i] = fileResponse{
			File:     f,
			IsDir:    f.FileType == model.FileTypeFolder,
			ShareID:  shareMap[f.FileID],
			PublicID: publicMap[f.FileID],
		}
	}
	return result
}

func (h *FileHandler) Upload(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	fileHeader, err := c.FormFile("file")
	if err != nil {
		pkg.Error(c, http.StatusBadRequest, "file is required")
		return
	}

	parentPath := c.PostForm("parent_path")
	if parentPath == "" {
		parentPath = "/"
	}

	uniqueFlag := c.PostForm("unique_flag")

	file, err := h.fileService.Upload(userID, fileHeader, parentPath, uniqueFlag, h.cfg)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, file)
}

func (h *FileHandler) Download(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	fileID := c.Param("file_id")
	if fileID == "" {
		pkg.Error(c, http.StatusBadRequest, "file_id is required")
		return
	}

	file, err := h.fileService.Download(userID, fileID)
	if err != nil {
		if err.Error() == "file not found" {
			pkg.Error(c, http.StatusNotFound, err.Error())
			return
		}
		if err.Error() == "access denied" {
			pkg.Error(c, http.StatusForbidden, err.Error())
			return
		}
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	// Folder download: package as ZIP
	if file.FileType == model.FileTypeFolder {
		children, prefix, err := h.fileService.GetFolderChildren(userID, file)
		if err != nil {
			pkg.Error(c, http.StatusInternalServerError, "failed to list folder contents")
			return
		}

		c.Header("Content-Type", "application/zip")
		c.Header("Content-Disposition", pkg.SafeContentDisposition("attachment", file.FileName+".zip"))

		zw := zip.NewWriter(c.Writer)
		defer zw.Close()

		writeFolderToZip(zw, file.FileName, prefix, children)
		return
	}

	if !pkg.FileExists(file.StoragePath) {
		pkg.Error(c, http.StatusNotFound, "file content not found on disk")
		return
	}

	if c.Query("preview") == "1" {
		c.Header("Content-Disposition", pkg.SafeContentDisposition("inline", file.FileName))
		c.Header("X-Content-Type-Options", "nosniff")
		c.Header("Content-Security-Policy", "default-src 'none'; img-src 'self'; media-src 'self'; style-src 'unsafe-inline'")
	} else {
		c.Header("Content-Disposition", pkg.SafeContentDisposition("attachment", file.FileName))
	}
	c.File(file.StoragePath)
}

// writeFolderToZip writes all children of a folder into a zip, preserving directory structure.
func writeFolderToZip(zw *zip.Writer, folderName, childPrefix string, children []model.File) {
	for _, child := range children {
		if child.FileType == model.FileTypeFolder || !pkg.FileExists(child.StoragePath) {
			continue
		}
		relPath := child.ParentPath[len(childPrefix):]
		zipPath := folderName + "/" + relPath + child.FileName

		fw, err := zw.Create(zipPath)
		if err != nil {
			continue
		}
		data, err := pkg.ReadFileBytes(child.StoragePath)
		if err != nil {
			continue
		}
		fw.Write(data)
	}
}

func (h *FileHandler) Delete(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	fileID := c.Param("file_id")
	if fileID == "" {
		pkg.Error(c, http.StatusBadRequest, "file_id is required")
		return
	}

	if err := h.fileService.Delete(userID, fileID); err != nil {
		if err.Error() == "file not found" {
			pkg.Error(c, http.StatusNotFound, err.Error())
			return
		}
		if err.Error() == "access denied" {
			pkg.Error(c, http.StatusForbidden, err.Error())
			return
		}
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, nil)
}

func (h *FileHandler) List(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	parentPath := c.DefaultQuery("parent_path", "/")
	page, _ := strconv.Atoi(c.DefaultQuery("page", "1"))
	size, _ := strconv.Atoi(c.DefaultQuery("size", "20"))

	if page < 1 {
		page = 1
	}
	if size < 1 || size > 100 {
		size = 20
	}

	var filter *repository.FileFilter
	status := c.Query("filter")
	startTimeStr := c.Query("start_time")
	endTimeStr := c.Query("end_time")
	if status != "" || startTimeStr != "" || endTimeStr != "" {
		filter = &repository.FileFilter{Status: status}
		if startTimeStr != "" {
			if t, err := time.Parse(time.RFC3339, startTimeStr); err == nil {
				filter.StartTime = &t
			}
		}
		if endTimeStr != "" {
			if t, err := time.Parse(time.RFC3339, endTimeStr); err == nil {
				filter.EndTime = &t
			}
		}
	}

	files, total, err := h.fileService.List(userID, parentPath, page, size, filter)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, gin.H{
		"files": h.enrichFiles(files),
		"total": total,
		"page":  page,
		"size":  size,
	})
}

func (h *FileHandler) Search(c *gin.Context) {
	keyword := c.Query("keyword")
	if keyword == "" {
		pkg.Error(c, http.StatusBadRequest, "keyword is required")
		return
	}

	page, _ := strconv.Atoi(c.DefaultQuery("page", "1"))
	size, _ := strconv.Atoi(c.DefaultQuery("size", "20"))

	if page < 1 {
		page = 1
	}
	if size < 1 || size > 100 {
		size = 20
	}

	files, total, err := h.fileService.Search(keyword, page, size)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, gin.H{
		"files": h.enrichFiles(files),
		"total": total,
		"page":  page,
		"size":  size,
	})
}

type publicFileResponse struct {
	fileResponse
	OwnerNickname string `json:"owner_nickname,omitempty"`
	OwnerAvatar   string `json:"owner_avatar,omitempty"`
}

func (h *FileHandler) PublicList(c *gin.Context) {
	page, _ := strconv.Atoi(c.DefaultQuery("page", "1"))
	size, _ := strconv.Atoi(c.DefaultQuery("size", "20"))

	if page < 1 {
		page = 1
	}
	if size < 1 || size > 100 {
		size = 20
	}

	files, total, err := h.fileService.PublicList(page, size)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	enriched := h.enrichFiles(files)

	// Collect unique user IDs
	userIDSet := make(map[string]struct{})
	for _, f := range files {
		userIDSet[f.UserID] = struct{}{}
	}
	userIDs := make([]string, 0, len(userIDSet))
	for uid := range userIDSet {
		userIDs = append(userIDs, uid)
	}

	// Batch fetch users
	type userInfo struct {
		Nickname string
		Avatar   string
	}
	userMap := make(map[string]userInfo)
	if len(userIDs) > 0 {
		users, err := h.userRepo.GetByUserIDs(userIDs)
		if err == nil {
			for _, u := range users {
				userMap[u.UserID] = userInfo{Nickname: u.Nickname, Avatar: u.AvatarURL}
			}
		}
	}

	// Build response with owner info
	result := make([]publicFileResponse, len(enriched))
	for i, ef := range enriched {
		result[i] = publicFileResponse{
			fileResponse:  ef,
			OwnerNickname: userMap[ef.UserID].Nickname,
			OwnerAvatar:   userMap[ef.UserID].Avatar,
		}
	}

	pkg.Success(c, gin.H{
		"files": result,
		"total": total,
		"page":  page,
		"size":  size,
	})
}

func (h *FileHandler) CreateFolder(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	var req struct {
		FolderName string `json:"folder_name" binding:"required"`
		ParentPath string `json:"parent_path"`
	}
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}
	if req.ParentPath == "" {
		req.ParentPath = "/"
	}

	folder, err := h.fileService.CreateFolder(userID, req.FolderName, req.ParentPath)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, folder)
}

func (h *FileHandler) BatchDownload(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	var req struct {
		FileIDs []string `json:"file_ids" binding:"required"`
	}
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	files, err := h.fileService.GetFilesByIDs(userID, req.FileIDs)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}
	if len(files) == 0 {
		pkg.Error(c, http.StatusNotFound, "no files found")
		return
	}

	c.Header("Content-Type", "application/zip")
	c.Header("Content-Disposition", fmt.Sprintf("attachment; filename=batch_download.zip"))

	zw := zip.NewWriter(c.Writer)
	defer zw.Close()

	for _, f := range files {
		if f.FileType == model.FileTypeFolder {
			// Expand folder recursively into ZIP
			children, prefix, err := h.fileService.GetFolderChildren(userID, &f)
			if err != nil {
				continue
			}
			writeFolderToZip(zw, f.FileName, prefix, children)
			continue
		}
		if !pkg.FileExists(f.StoragePath) {
			continue
		}
		fw, err := zw.Create(f.FileName)
		if err != nil {
			continue
		}
		data, err := pkg.ReadFileBytes(f.StoragePath)
		if err != nil {
			continue
		}
		fw.Write(data)
	}
}
