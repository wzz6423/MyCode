package handler

import (
	"fmt"
	"net/http"
	"path/filepath"

	"clouddisk/internal/config"
	"clouddisk/internal/pkg"
	"clouddisk/internal/service"

	"github.com/gin-gonic/gin"
)

type UserHandler struct {
	userSvc *service.UserService
	cfg     *config.Config
}

func NewUserHandler(userSvc *service.UserService, cfg *config.Config) *UserHandler {
	return &UserHandler{userSvc: userSvc, cfg: cfg}
}

type getVerifyCodeReq struct {
	Type   string `json:"type" binding:"required,oneof=phone email"`
	Target string `json:"target" binding:"required"`
}

func (h *UserHandler) GetVerifyCode(c *gin.Context) {
	var req getVerifyCodeReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	codeID, err := h.userSvc.SendVerifyCode(req.Type, req.Target)
	if err != nil {
		pkg.Error(c, http.StatusInternalServerError, "failed to send verify code: "+err.Error())
		return
	}

	pkg.Success(c, gin.H{"verify_code_id": codeID})
}

type registerReq struct {
	Type         string `json:"type" binding:"required,oneof=nickname phone email"`
	Target       string `json:"target"`
	Password     string `json:"password" binding:"required"`
	Nickname     string `json:"nickname" binding:"required"`
	VerifyCodeID string `json:"verify_code_id"`
	VerifyCode   string `json:"verify_code"`
}

func (h *UserHandler) Register(c *gin.Context) {
	var req registerReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if req.Type != "nickname" && req.Target == "" {
		pkg.Error(c, http.StatusBadRequest, "target is required for phone/email registration")
		return
	}

	if err := h.userSvc.Register(req.Type, req.Target, req.Password, req.Nickname, req.VerifyCodeID, req.VerifyCode); err != nil {
		pkg.Error(c, http.StatusBadRequest, err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}

type loginReq struct {
	Type     string `json:"type" binding:"required,oneof=nickname phone email"`
	Target   string `json:"target" binding:"required"`
	Password string `json:"password" binding:"required"`
}

func (h *UserHandler) Login(c *gin.Context) {
	var req loginReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	token, user, err := h.userSvc.Login(req.Type, req.Target, req.Password)
	if err != nil {
		pkg.Error(c, http.StatusUnauthorized, err.Error())
		return
	}

	pkg.Success(c, gin.H{"token": token, "user": user})
}

func (h *UserHandler) GetProfile(c *gin.Context) {
	userID := c.GetString("user_id")

	user, err := h.userSvc.GetProfile(userID)
	if err != nil {
		pkg.Error(c, http.StatusNotFound, err.Error())
		return
	}

	pkg.Success(c, user)
}

func (h *UserHandler) SetAvatar(c *gin.Context) {
	userID := c.GetString("user_id")

	file, err := c.FormFile("avatar")
	if err != nil {
		pkg.Error(c, http.StatusBadRequest, "avatar file is required")
		return
	}

	if !pkg.IsAllowedImageExt(file.Filename) {
		pkg.Error(c, http.StatusBadRequest, "avatar must be an image file (jpg, png, gif, webp, etc.)")
		return
	}

	ext := filepath.Ext(file.Filename)
	filename := pkg.NewUUID() + ext
	dst := filepath.Join(h.cfg.Server.UploadDir, "avatars", filename)

	if err := pkg.SaveUploadedFile(file, dst); err != nil {
		pkg.Error(c, http.StatusInternalServerError, "failed to save avatar: "+err.Error())
		return
	}

	avatarURL := fmt.Sprintf("/avatars/%s", filename)
	if err := h.userSvc.UpdateAvatar(userID, avatarURL); err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, gin.H{"avatar_url": avatarURL})
}

type setNicknameReq struct {
	Nickname string `json:"nickname" binding:"required"`
}

func (h *UserHandler) SetNickname(c *gin.Context) {
	userID := c.GetString("user_id")

	var req setNicknameReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if err := h.userSvc.UpdateNickname(userID, req.Nickname); err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}

type setDescriptionReq struct {
	Description string `json:"description"`
}

func (h *UserHandler) SetDescription(c *gin.Context) {
	userID := c.GetString("user_id")

	var req setDescriptionReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if err := h.userSvc.UpdateDescription(userID, req.Description); err != nil {
		pkg.Error(c, http.StatusInternalServerError, err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}

type setPasswordReq struct {
	OldPassword string `json:"old_password" binding:"required"`
	NewPassword string `json:"new_password" binding:"required"`
}

func (h *UserHandler) SetPassword(c *gin.Context) {
	userID := c.GetString("user_id")

	var req setPasswordReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if err := h.userSvc.UpdatePassword(userID, req.OldPassword, req.NewPassword); err != nil {
		pkg.Error(c, http.StatusBadRequest, err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}

type setPhoneReq struct {
	NewPhone     string `json:"new_phone" binding:"required"`
	VerifyCodeID string `json:"verify_code_id" binding:"required"`
	VerifyCode   string `json:"verify_code" binding:"required"`
	Password     string `json:"password" binding:"required"`
}

func (h *UserHandler) SetPhone(c *gin.Context) {
	userID := c.GetString("user_id")

	var req setPhoneReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if err := h.userSvc.UpdatePhone(userID, req.NewPhone, req.VerifyCodeID, req.VerifyCode, req.Password); err != nil {
		pkg.Error(c, http.StatusBadRequest, err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}

type deleteAccountReq struct {
	Password string `json:"password" binding:"required"`
}

func (h *UserHandler) DeleteAccount(c *gin.Context) {
	userID := c.GetString("user_id")

	var req deleteAccountReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if err := h.userSvc.DeleteAccount(userID, req.Password); err != nil {
		if err.Error() == "incorrect password" {
			pkg.Error(c, http.StatusBadRequest, err.Error())
			return
		}
		pkg.Error(c, http.StatusInternalServerError, "failed to delete account: "+err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}

type setEmailReq struct {
	NewEmail     string `json:"new_email" binding:"required"`
	VerifyCodeID string `json:"verify_code_id" binding:"required"`
	VerifyCode   string `json:"verify_code" binding:"required"`
	Password     string `json:"password" binding:"required"`
}

func (h *UserHandler) SetEmail(c *gin.Context) {
	userID := c.GetString("user_id")

	var req setEmailReq
	if err := c.ShouldBindJSON(&req); err != nil {
		pkg.Error(c, http.StatusBadRequest, "invalid request: "+err.Error())
		return
	}

	if err := h.userSvc.UpdateEmail(userID, req.NewEmail, req.VerifyCodeID, req.VerifyCode, req.Password); err != nil {
		pkg.Error(c, http.StatusBadRequest, err.Error())
		return
	}

	pkg.Success(c, gin.H{})
}
