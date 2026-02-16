.PHONY: build build-all run test clean web-install web-build web-dev all

# Project info
APP_NAME    := clouddisk-server
CMD_PATH    := ./cmd/server
BIN_DIR     := ./bin
WEB_DIR     := ./web
VERSION     ?= $(shell git describe --tags --always --dirty 2>/dev/null || echo "dev")
BUILD_TIME  := $(shell date -u '+%Y-%m-%dT%H:%M:%SZ')
LDFLAGS     := -s -w -X main.version=$(VERSION) -X main.buildTime=$(BUILD_TIME)

# Default target OS/ARCH
GOOS        ?= $(shell go env GOOS)
GOARCH      ?= $(shell go env GOARCH)

# --- Build ---

build:
	CGO_ENABLED=0 GOOS=$(GOOS) GOARCH=$(GOARCH) \
		go build -ldflags "$(LDFLAGS)" -o $(BIN_DIR)/$(APP_NAME)-$(GOOS)-$(GOARCH)$(if $(filter windows,$(GOOS)),.exe,) $(CMD_PATH)

# Cross-compile for all common platforms
build-all: build-linux-amd64 build-linux-arm64 build-darwin-amd64 build-darwin-arm64 build-windows-amd64

build-linux-amd64:
	CGO_ENABLED=0 GOOS=linux GOARCH=amd64 \
		go build -ldflags "$(LDFLAGS)" -o $(BIN_DIR)/$(APP_NAME)-linux-amd64 $(CMD_PATH)

build-linux-arm64:
	CGO_ENABLED=0 GOOS=linux GOARCH=arm64 \
		go build -ldflags "$(LDFLAGS)" -o $(BIN_DIR)/$(APP_NAME)-linux-arm64 $(CMD_PATH)

build-darwin-amd64:
	CGO_ENABLED=0 GOOS=darwin GOARCH=amd64 \
		go build -ldflags "$(LDFLAGS)" -o $(BIN_DIR)/$(APP_NAME)-darwin-amd64 $(CMD_PATH)

build-darwin-arm64:
	CGO_ENABLED=0 GOOS=darwin GOARCH=arm64 \
		go build -ldflags "$(LDFLAGS)" -o $(BIN_DIR)/$(APP_NAME)-darwin-arm64 $(CMD_PATH)

build-windows-amd64:
	CGO_ENABLED=0 GOOS=windows GOARCH=amd64 \
		go build -ldflags "$(LDFLAGS)" -o $(BIN_DIR)/$(APP_NAME)-windows-amd64.exe $(CMD_PATH)

# --- Run & Test ---

run:
	go run $(CMD_PATH)

test:
	go test ./... -v -count=1

clean:
	rm -rf $(BIN_DIR)/

# --- Web ---

web-install:
	cd $(WEB_DIR) && pnpm install

web-build:
	cd $(WEB_DIR) && pnpm run build

web-dev:
	cd $(WEB_DIR) && pnpm run dev

# --- All ---

all: build web-build
