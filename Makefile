# sunvox-max Makefile

# Configuration
PROJECT_NAME := sunvox~
BUILD_DIR := build
EXTERNALS_DIR := externals
EXTERNAL_PATH := $(EXTERNALS_DIR)/$(PROJECT_NAME).mxo
BINARY_PATH := $(EXTERNAL_PATH)/Contents/MacOS/$(PROJECT_NAME)
CMAKE_GENERATOR := Xcode
BUILD_CONFIG := Release

# Max versions to check for (space-separated, first found is default)
MAX_VERSIONS := 9 8
MAX_DOCS_DIR := $(HOME)/Documents

# Parallel build jobs (override with: make build JOBS=4)
JOBS := $(shell sysctl -n hw.ncpu 2>/dev/null || echo 4)

.PHONY: all build clean sign setup help check-deps configure rebuild

# Default target
all: clean build sign

# Show available targets
help:
	@echo "sunvox-max build system"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Clean, build, and sign (default)"
	@echo "  build     - Configure and compile"
	@echo "  rebuild   - Build without reconfiguring"
	@echo "  clean     - Remove build and externals directories"
	@echo "  sign      - Code sign the external"
	@echo "  setup     - Initialize submodules and create Max packages symlink"
	@echo "  configure - Run CMake configuration only"
	@echo "  check-deps - Verify build dependencies"
	@echo ""
	@echo "Options:"
	@echo "  JOBS=N           - Parallel build jobs (default: $(JOBS))"
	@echo "  BUILD_CONFIG=X   - Debug or Release (default: $(BUILD_CONFIG))"
	@echo "  MAX_VERSIONS='X' - Max versions to symlink (default: $(MAX_VERSIONS))"

# Check for required tools
check-deps:
	@command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found. Install with: brew install cmake"; exit 1; }
	@command -v xcodebuild >/dev/null 2>&1 || { echo "Error: Xcode command line tools not found. Install with: xcode-select --install"; exit 1; }
	@echo "All dependencies found"

# CMake configuration step
configure: check-deps
	@mkdir -p $(BUILD_DIR)
	@echo "Configuring with CMake ($(CMAKE_GENERATOR))..."
	@cd $(BUILD_DIR) && cmake -G$(CMAKE_GENERATOR) ..

# Build (configure if needed)
build: check-deps
	@mkdir -p $(BUILD_DIR)
	@if [ ! -f $(BUILD_DIR)/CMakeCache.txt ]; then \
		echo "Configuring with CMake ($(CMAKE_GENERATOR))..."; \
		cd $(BUILD_DIR) && cmake -G"$(CMAKE_GENERATOR)" ..; \
	fi
	@echo "Building $(BUILD_CONFIG) with $(JOBS) parallel jobs..."
	@cmake --build $(BUILD_DIR) --config $(BUILD_CONFIG) -j $(JOBS)
	@if [ -d "$(EXTERNAL_PATH)" ]; then \
		echo "Build successful: $(EXTERNAL_PATH)"; \
	else \
		echo "Error: Build completed but external not found at $(EXTERNAL_PATH)"; \
		exit 1; \
	fi

# Rebuild without reconfiguring
rebuild:
	@if [ ! -f $(BUILD_DIR)/CMakeCache.txt ]; then \
		echo "Error: No existing configuration. Run 'make build' first."; \
		exit 1; \
	fi
	@echo "Rebuilding $(BUILD_CONFIG)..."
	@cmake --build $(BUILD_DIR) --config $(BUILD_CONFIG) -j $(JOBS)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(EXTERNALS_DIR)
	@echo "Clean complete"

# Code sign the external
sign:
	@if [ ! -f "$(BINARY_PATH)" ]; then \
		echo "Error: Binary not found at $(BINARY_PATH)"; \
		echo "Run 'make build' first."; \
		exit 1; \
	fi
	@echo "Signing $(BINARY_PATH)..."
	@codesign -s "-" -f "$(BINARY_PATH)"
	@echo "Signing complete"

# Initial project setup
setup:
	@echo "Initializing git submodules..."
	@git submodule init
	@git submodule update
	@echo "Creating Max packages symlinks..."
	@found=0; \
	for ver in $(MAX_VERSIONS); do \
		packages_dir="$(MAX_DOCS_DIR)/Max $$ver/Packages"; \
		link_path="$$packages_dir/$(notdir $(CURDIR))"; \
		if [ -d "$$packages_dir" ]; then \
			if [ -L "$$link_path" ]; then \
				echo "Max $$ver: Symlink already exists, skipping"; \
				found=1; \
			elif [ -e "$$link_path" ]; then \
				echo "Max $$ver: Error - $$link_path exists but is not a symlink"; \
			else \
				ln -s "$(CURDIR)" "$$link_path"; \
				echo "Max $$ver: Created symlink"; \
				found=1; \
			fi; \
		else \
			echo "Max $$ver: Packages directory not found, skipping"; \
		fi; \
	done; \
	if [ $$found -eq 0 ]; then \
		echo "Warning: No Max installations found in $(MAX_DOCS_DIR)"; \
		echo "Looked for versions: $(MAX_VERSIONS)"; \
	fi
	@echo "Setup complete"
