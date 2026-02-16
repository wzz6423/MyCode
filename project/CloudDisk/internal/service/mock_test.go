package service

import (
	"clouddisk/internal/model"
	"sync"

	"gorm.io/gorm"
)

// --- Mock User Repository ---

type mockUserRepo struct {
	mu     sync.RWMutex
	users  map[string]*model.User
	phones map[string]*model.User
	emails map[string]*model.User
}

func newMockUserRepo() *mockUserRepo {
	return &mockUserRepo{
		users:  make(map[string]*model.User),
		phones: make(map[string]*model.User),
		emails: make(map[string]*model.User),
	}
}

func (m *mockUserRepo) Create(user *model.User) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.users[user.UserID] = user
	if user.Phone != nil {
		m.phones[*user.Phone] = user
	}
	if user.Email != nil {
		m.emails[*user.Email] = user
	}
	return nil
}

func (m *mockUserRepo) GetByUserID(userID string) (*model.User, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	u, ok := m.users[userID]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return u, nil
}

func (m *mockUserRepo) GetByPhone(phone string) (*model.User, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	u, ok := m.phones[phone]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return u, nil
}

func (m *mockUserRepo) GetByEmail(email string) (*model.User, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	u, ok := m.emails[email]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return u, nil
}

func (m *mockUserRepo) Update(user *model.User) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.users[user.UserID] = user
	if user.Phone != nil {
		m.phones[*user.Phone] = user
	}
	if user.Email != nil {
		m.emails[*user.Email] = user
	}
	return nil
}

// --- Mock File Repository ---

type mockFileRepo struct {
	mu    sync.RWMutex
	files map[string]*model.File
}

func newMockFileRepo() *mockFileRepo {
	return &mockFileRepo{
		files: make(map[string]*model.File),
	}
}

func (m *mockFileRepo) Create(file *model.File) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.files[file.FileID] = file
	return nil
}

func (m *mockFileRepo) GetByFileID(fileID string) (*model.File, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	f, ok := m.files[fileID]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return f, nil
}

func (m *mockFileRepo) GetByUniqueFlag(uniqueFlag string) (*model.File, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	for _, f := range m.files {
		if f.UniqueFlag == uniqueFlag {
			return f, nil
		}
	}
	return nil, gorm.ErrRecordNotFound
}

func (m *mockFileRepo) ListByUserAndPath(userID, parentPath string, offset, limit int) ([]model.File, int64, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	var result []model.File
	for _, f := range m.files {
		if f.UserID == userID && f.ParentPath == parentPath {
			result = append(result, *f)
		}
	}
	total := int64(len(result))
	if offset >= len(result) {
		return []model.File{}, total, nil
	}
	end := offset + limit
	if end > len(result) {
		end = len(result)
	}
	return result[offset:end], total, nil
}

func (m *mockFileRepo) ListPublic(offset, limit int) ([]model.File, int64, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	var result []model.File
	for _, f := range m.files {
		if f.IsPublic {
			result = append(result, *f)
		}
	}
	total := int64(len(result))
	if offset >= len(result) {
		return []model.File{}, total, nil
	}
	end := offset + limit
	if end > len(result) {
		end = len(result)
	}
	return result[offset:end], total, nil
}

func (m *mockFileRepo) Update(file *model.File) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.files[file.FileID] = file
	return nil
}

func (m *mockFileRepo) Delete(fileID string) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	if _, ok := m.files[fileID]; !ok {
		return gorm.ErrRecordNotFound
	}
	delete(m.files, fileID)
	return nil
}

// --- Mock Share Repository ---

type mockShareRepo struct {
	mu     sync.RWMutex
	shares map[string]*model.Share
}

func newMockShareRepo() *mockShareRepo {
	return &mockShareRepo{
		shares: make(map[string]*model.Share),
	}
}

func (m *mockShareRepo) Create(share *model.Share) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.shares[share.ShareID] = share
	return nil
}

func (m *mockShareRepo) GetByShareID(shareID string) (*model.Share, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	s, ok := m.shares[shareID]
	if !ok {
		return nil, gorm.ErrRecordNotFound
	}
	return s, nil
}

func (m *mockShareRepo) GetByFileIDAndType(fileID string, shareType uint8) (*model.Share, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	for _, s := range m.shares {
		if s.FileID == fileID && s.ShareType == shareType {
			return s, nil
		}
	}
	return nil, gorm.ErrRecordNotFound
}

func (m *mockShareRepo) DeleteByFileIDAndType(fileID string, shareType uint8) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	for id, s := range m.shares {
		if s.FileID == fileID && s.ShareType == shareType {
			delete(m.shares, id)
			return nil
		}
	}
	return nil
}

func (m *mockShareRepo) IncrementCount(shareID string) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	s, ok := m.shares[shareID]
	if !ok {
		return gorm.ErrRecordNotFound
	}
	s.CurrentCount++
	return nil
}
