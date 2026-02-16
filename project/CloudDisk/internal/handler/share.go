package handler

import (
	"net/http"

	"clouddisk/internal/pkg"
	"clouddisk/internal/service"

	"github.com/gin-gonic/gin"
)

type ShareHandler struct {
	shareService *service.ShareService
}

func NewShareHandler(shareService *service.ShareService) *ShareHandler {
	return &ShareHandler{shareService: shareService}
}

type shareRequest struct {
	Message     string `json:"message"`
	ExpireHours int    `json:"expire_hours"`
	MaxCount    int    `json:"max_count"`
}

type publicRequest struct {
	ExpireHours int `json:"expire_hours"`
	MaxCount    int `json:"max_count"`
}

type receiveRequest struct {
	SavePath string `json:"save_path"`
	FileName string `json:"file_name"`
}

func (h *ShareHandler) Share(c *gin.Context) {
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

	var req shareRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request body")
		return
	}

	share, err := h.shareService.ShareFile(userID, fileID, req.Message, req.ExpireHours, req.MaxCount)
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

	pkg.Success(c, share)
}

func (h *ShareHandler) Unshare(c *gin.Context) {
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

	if err := h.shareService.UnshareFile(userID, fileID); err != nil {
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

func (h *ShareHandler) Public(c *gin.Context) {
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

	var req publicRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request body")
		return
	}

	share, err := h.shareService.PublicFile(userID, fileID, req.ExpireHours, req.MaxCount)
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

	pkg.Success(c, share)
}

func (h *ShareHandler) Unpublic(c *gin.Context) {
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

	if err := h.shareService.UnpublicFile(userID, fileID); err != nil {
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

func (h *ShareHandler) GetShareInfo(c *gin.Context) {
	shareID := c.Param("share_id")
	if shareID == "" {
		pkg.Error(c, http.StatusBadRequest, "share_id is required")
		return
	}

	info, err := h.shareService.GetShareInfo(shareID)
	if err != nil {
		if err.Error() == "share not found" {
			pkg.Error(c, http.StatusNotFound, err.Error())
			return
		}
		if err.Error() == "share has expired" || err.Error() == "share has reached maximum receive count" {
			pkg.Error(c, http.StatusForbidden, err.Error())
			return
		}
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, info)
}

func (h *ShareHandler) Receive(c *gin.Context) {
	userID := c.GetString("user_id")
	if userID == "" {
		pkg.Error(c, http.StatusUnauthorized, "unauthorized")
		return
	}

	shareID := c.Param("share_id")
	if shareID == "" {
		pkg.Error(c, http.StatusBadRequest, "share_id is required")
		return
	}

	var req receiveRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request body")
		return
	}

	if req.SavePath == "" {
		req.SavePath = "/"
	}

	file, err := h.shareService.ReceiveFile(userID, shareID, req.SavePath, req.FileName)
	if err != nil {
		if err.Error() == "share not found" {
			pkg.Error(c, http.StatusNotFound, err.Error())
			return
		}
		if err.Error() == "share has expired" || err.Error() == "share has reached maximum receive count" {
			pkg.Error(c, http.StatusForbidden, err.Error())
			return
		}
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, file)
}
