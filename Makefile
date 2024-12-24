BUILD_DIR = build
SOURCE_DIR = ..

all: check_opencv build clean

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake $(SOURCE_DIR) && make

check_opencv:
	@if ! pkg-config --exists opencv4; then \
		echo "Нужен OpenCV. Сейчас накатим..."; \
		sudo apt-get update && sudo apt-get install -y libopencv-dev; \
	fi

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all build clean check_opencv
