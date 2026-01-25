# sunvox-max Makefile (cross-platform: macOS and Windows)

# OS detection
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := Darwin
    else
        PLATFORM := $(UNAME_S)
    endif
endif

# Configuration
PROJECT_NAME := sunvox~
BUILD_DIR := build
EXTERNALS_DIR := externals
BUILD_CONFIG := Release

# Platform-specific configuration
ifeq ($(PLATFORM),Darwin)
    EXTERNAL_EXT := .mxo
    EXTERNAL_PATH := $(EXTERNALS_DIR)/$(PROJECT_NAME)$(EXTERNAL_EXT)
    BINARY_PATH := $(EXTERNAL_PATH)/Contents/MacOS/$(PROJECT_NAME)
    CMAKE_GENERATOR := Xcode
    JOBS := $(shell sysctl -n hw.ncpu 2>/dev/null || echo 4)
    MAX_DOCS_DIR := $(HOME)/Documents
    RM_RF := rm -rf
else ifeq ($(PLATFORM),Windows)
    EXTERNAL_EXT := .mxe64
    EXTERNAL_PATH := $(EXTERNALS_DIR)/$(PROJECT_NAME)$(EXTERNAL_EXT)
    BINARY_PATH := $(EXTERNAL_PATH)
    CMAKE_GENERATOR := Visual Studio 17 2022
    JOBS := $(NUMBER_OF_PROCESSORS)
    MAX_DOCS_DIR := $(USERPROFILE)/Documents
    RM_RF := rd /s /q
endif

# Max versions to check for (space-separated)
MAX_VERSIONS := 9 8

.PHONY: all build clean sign submodules link setup help check-deps \
		configure rebuild

# Default target
# Note: Xcode generator handles code signing automatically, so 'sign' is not needed
# Use 'make sign' manually only if building with a non-Xcode generator (e.g., Unix Makefiles)
all: clean build

# Show available targets
help:
	@echo "sunvox-max build system ($(PLATFORM))"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Clean and build (default)"
	@echo "  build     - Configure and compile"
	@echo "  rebuild   - Build without reconfiguring"
	@echo "  clean     - Remove build and externals directories"
ifeq ($(PLATFORM),Darwin)
	@echo "  sign      - Code sign (macOS, only needed for non-Xcode builds)"
endif
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
ifeq ($(PLATFORM),Darwin)
	@command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found. Install with: brew install cmake"; exit 1; }
	@command -v xcodebuild >/dev/null 2>&1 || { echo "Error: Xcode command line tools not found. Install with: xcode-select --install"; exit 1; }
else ifeq ($(PLATFORM),Windows)
	@where cmake >nul 2>&1 || ( echo "Error: cmake not found. Install from https://cmake.org/download/" & exit /b 1 )
endif
	@echo "All dependencies found"

# CMake configuration step
configure: check-deps
ifeq ($(PLATFORM),Darwin)
	@mkdir -p $(BUILD_DIR)
else ifeq ($(PLATFORM),Windows)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
endif
	@echo "Configuring with CMake ($(CMAKE_GENERATOR))..."
	@cd $(BUILD_DIR) && cmake -G"$(CMAKE_GENERATOR)" ..

# Build (configure if needed)
build: check-deps
ifeq ($(PLATFORM),Darwin)
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
else ifeq ($(PLATFORM),Windows)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BUILD_DIR)\CMakeCache.txt ( \
		echo Configuring with CMake ($(CMAKE_GENERATOR))... & \
		cd $(BUILD_DIR) && cmake -G"$(CMAKE_GENERATOR)" -A x64 .. \
	)
	@echo Building $(BUILD_CONFIG) with $(JOBS) parallel jobs...
	@cmake --build $(BUILD_DIR) --config $(BUILD_CONFIG) -j $(JOBS)
	@if exist "$(EXTERNAL_PATH)" ( \
		echo Build successful: $(EXTERNAL_PATH) \
	) else ( \
		echo Error: Build completed but external not found at $(EXTERNAL_PATH) & \
		exit /b 1 \
	)
endif

# Rebuild without reconfiguring
rebuild:
ifeq ($(PLATFORM),Darwin)
	@if [ ! -f $(BUILD_DIR)/CMakeCache.txt ]; then \
		echo "Error: No existing configuration. Run 'make build' first."; \
		exit 1; \
	fi
else ifeq ($(PLATFORM),Windows)
	@if not exist $(BUILD_DIR)\CMakeCache.txt ( \
		echo Error: No existing configuration. Run 'make build' first. & \
		exit /b 1 \
	)
endif
	@echo "Rebuilding $(BUILD_CONFIG)..."
	@cmake --build $(BUILD_DIR) --config $(BUILD_CONFIG) -j $(JOBS)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
ifeq ($(PLATFORM),Darwin)
	@rm -rf $(BUILD_DIR)
	@rm -rf $(EXTERNALS_DIR)
else ifeq ($(PLATFORM),Windows)
	@if exist $(BUILD_DIR) rd /s /q $(BUILD_DIR)
	@if exist $(EXTERNALS_DIR) rd /s /q $(EXTERNALS_DIR)
endif
	@echo "Clean complete"

# Code sign the external (macOS only)
# Note: Only needed if building with non-Xcode generator;
# Xcode signs automatically
sign:
ifeq ($(PLATFORM),Darwin)
	@if [ ! -f "$(BINARY_PATH)" ]; then \
		echo "Error: Binary not found at $(BINARY_PATH)"; \
		echo "Run 'make build' first."; \
		exit 1; \
	fi
	@echo "Signing $(BINARY_PATH)..."
	@codesign -s "-" -f "$(BINARY_PATH)"
	@echo "Signing complete"
else
	@echo "Skipping code signing (macOS only)"
endif

# Init submodules
submodules:
	@echo "Initializing git submodules..."
	@git submodule init
	@git submodule update

# Symlink this package to Max Packages directory (macOS only)
link:
ifeq ($(PLATFORM),Darwin)
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
else
	@echo "Skipping symlink creation (macOS only)"
	@echo "On Windows, copy the package folder manually to your Max Packages directory"
endif

# Initial project setup
ifeq ($(PLATFORM),Darwin)
setup: submodules link
	@echo "Setup complete"
else
setup: submodules
	@echo "Setup complete"
	@echo "Note: Copy this folder to your Max Packages directory manually"
endif
