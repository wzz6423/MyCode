package repository

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"strings"

	"clouddisk/internal/model"

	"github.com/elastic/go-elasticsearch/v7"
	"github.com/elastic/go-elasticsearch/v7/esapi"
)

type ESRepository struct {
	client *elasticsearch.Client
	index  string
}

func NewESRepository(addresses []string, username, password, index string) (*ESRepository, error) {
	cfg := elasticsearch.Config{
		Addresses: addresses,
		Username:  username,
		Password:  password,
	}
	client, err := elasticsearch.NewClient(cfg)
	if err != nil {
		return nil, fmt.Errorf("failed to create elasticsearch client: %w", err)
	}
	return &ESRepository{client: client, index: index}, nil
}

type esFileDoc struct {
	FileID   string `json:"file_id"`
	FileName string `json:"file_name"`
	UserID   string `json:"user_id"`
	FileType uint8  `json:"file_type"`
	IsPublic bool   `json:"is_public"`
}

func (r *ESRepository) IndexFile(f *model.File) error {
	doc := esFileDoc{
		FileID:   f.FileID,
		FileName: f.FileName,
		UserID:   f.UserID,
		FileType: f.FileType,
		IsPublic: f.IsPublic,
	}

	body, err := json.Marshal(doc)
	if err != nil {
		return fmt.Errorf("failed to marshal document: %w", err)
	}

	req := esapi.IndexRequest{
		Index:      r.index,
		DocumentID: f.FileID,
		Body:       bytes.NewReader(body),
		Refresh:    "true",
	}

	res, err := req.Do(context.Background(), r.client)
	if err != nil {
		return fmt.Errorf("failed to index document: %w", err)
	}
	defer res.Body.Close()

	if res.IsError() {
		return fmt.Errorf("index document error: %s", res.String())
	}
	return nil
}

func (r *ESRepository) DeleteFile(fileID string) error {
	req := esapi.DeleteRequest{
		Index:      r.index,
		DocumentID: fileID,
		Refresh:    "true",
	}

	res, err := req.Do(context.Background(), r.client)
	if err != nil {
		return fmt.Errorf("failed to delete document: %w", err)
	}
	defer res.Body.Close()

	if res.IsError() && res.StatusCode != 404 {
		return fmt.Errorf("delete document error: %s", res.String())
	}
	return nil
}

func (r *ESRepository) SearchFiles(keyword string, offset, limit int) (fileIDs []string, total int, err error) {
	query := map[string]interface{}{
		"from": offset,
		"size": limit,
		"query": map[string]interface{}{
			"match": map[string]interface{}{
				"file_name": keyword,
			},
		},
	}

	body, err := json.Marshal(query)
	if err != nil {
		return nil, 0, fmt.Errorf("failed to marshal query: %w", err)
	}

	res, err := r.client.Search(
		r.client.Search.WithContext(context.Background()),
		r.client.Search.WithIndex(r.index),
		r.client.Search.WithBody(strings.NewReader(string(body))),
		r.client.Search.WithTrackTotalHits(true),
	)
	if err != nil {
		return nil, 0, fmt.Errorf("failed to search: %w", err)
	}
	defer res.Body.Close()

	if res.IsError() {
		return nil, 0, fmt.Errorf("search error: %s", res.String())
	}

	var result struct {
		Hits struct {
			Total struct {
				Value int `json:"value"`
			} `json:"total"`
			Hits []struct {
				Source esFileDoc `json:"_source"`
			} `json:"hits"`
		} `json:"hits"`
	}

	if err := json.NewDecoder(res.Body).Decode(&result); err != nil {
		return nil, 0, fmt.Errorf("failed to decode response: %w", err)
	}

	for _, hit := range result.Hits.Hits {
		fileIDs = append(fileIDs, hit.Source.FileID)
	}

	return fileIDs, result.Hits.Total.Value, nil
}
