# Makefile for PC MIDI Studio Bitwig Extension
# Simple build automation without Maven or Gradle

# Configuration
JAVAC = javac
JAR = jar
JAVA_VERSION = 11

# Paths
SRC_DIR = src/main/java
BUILD_DIR = build
DIST_DIR = dist
LIB_DIR = lib

# Bitwig API JAR (you need to download this from Bitwig)
BITWIG_API = $(LIB_DIR)/bitwig-extension-api.jar

# Source files
SOURCES = $(shell find $(SRC_DIR) -name "*.java")

# Output JAR
OUTPUT_JAR = PCMidiStudio.jar

# Default target
all: clean build package

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(DIST_DIR)
	@mkdir -p $(LIB_DIR)

# Check for Bitwig API
check-api:
	@if [ ! -f "$(BITWIG_API)" ]; then \
		echo "ERROR: Bitwig Extension API not found!"; \
		echo "Please download bitwig-extension-api.jar and place it in $(LIB_DIR)/"; \
		echo "You can find it in your Bitwig Studio installation:"; \
		echo "  - Windows: C:/Program Files/Bitwig Studio/resources/extensions/"; \
		echo "  - macOS: /Applications/Bitwig Studio.app/Contents/Resources/extensions/"; \
		echo "  - Linux: /opt/bitwig-studio/resources/extensions/"; \
		exit 1; \
	fi

# Compile Java sources
build: dirs check-api
	@echo "Compiling Java sources..."
	$(JAVAC) -source $(JAVA_VERSION) -target $(JAVA_VERSION) \
		-cp "$(BITWIG_API)" \
		-d $(BUILD_DIR) \
		$(SOURCES)
	@echo "Compilation complete!"

# Package into JAR
package: build
	@echo "Creating JAR package..."
	@echo "Manifest-Version: 1.0" > $(BUILD_DIR)/MANIFEST.MF
	@echo "Extension-Name: PC MIDI Studio" >> $(BUILD_DIR)/MANIFEST.MF
	@echo "Extension-Version: 1.0.0" >> $(BUILD_DIR)/MANIFEST.MF
	@echo "Extension-Author: PetiteChose Audio" >> $(BUILD_DIR)/MANIFEST.MF
	@echo "" >> $(BUILD_DIR)/MANIFEST.MF
	
	cd $(BUILD_DIR) && $(JAR) cfm ../$(DIST_DIR)/$(OUTPUT_JAR) MANIFEST.MF audio/
	@echo "JAR created: $(DIST_DIR)/$(OUTPUT_JAR)"

# Install to Bitwig Extensions folder
install: package
	@echo "Installing extension to Bitwig Studio..."
	@if [ -d "$(HOME)/Documents/Bitwig Studio/Extensions" ]; then \
		cp $(DIST_DIR)/$(OUTPUT_JAR) "$(HOME)/Documents/Bitwig Studio/Extensions/"; \
		echo "Extension installed to: $(HOME)/Documents/Bitwig Studio/Extensions/"; \
		echo "Please restart Bitwig Studio or reload extensions."; \
	else \
		echo "ERROR: Bitwig Extensions folder not found!"; \
		echo "Please copy $(DIST_DIR)/$(OUTPUT_JAR) manually to your Bitwig Extensions folder."; \
	fi

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DIST_DIR)
	@echo "Clean complete!"

# Development build (with debug info)
dev: clean
	@echo "Building with debug information..."
	$(JAVAC) -source $(JAVA_VERSION) -target $(JAVA_VERSION) \
		-g \
		-cp "$(BITWIG_API)" \
		-d $(BUILD_DIR) \
		$(SOURCES)
	@make package

# Help
help:
	@echo "PC MIDI Studio Bitwig Extension - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make all      - Clean, build and package the extension"
	@echo "  make build    - Compile Java sources"
	@echo "  make package  - Create JAR file"
	@echo "  make install  - Install to Bitwig Extensions folder"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make dev      - Build with debug information"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Prerequisites:"
	@echo "  1. Java Development Kit (JDK) 11 or higher"
	@echo "  2. Bitwig Extension API JAR in $(LIB_DIR)/"

.PHONY: all dirs check-api build package install clean dev help