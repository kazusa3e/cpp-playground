SOURCES := $(shell fd -E build '\.(h|hpp|cc)$$')
BUILD_DIR := build

.PHONY: build
build:
	cmake --build $(BUILD_DIR) -j$(shell nproc)

.PHONY: configure
configure:
	cmake --preset=development

.PHONY: test
test: build
	./$(BUILD_DIR)/tests/all_tests "~[benchmark]"

.PHONY: run
run: build
	./$(BUILD_DIR)/src/main

.PHONY: sanitize
sanitize: build
	ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1 TSAN_OPTIONS=history_size=7 ./$(BUILD_DIR)/tests/all_tests

.PHONY: benchmark
benchmark: build
	./$(BUILD_DIR)/tests/all_tests "[benchmark]"

.PHONY: format
format:
	clang-format -i $(SOURCES)

.PHONY: clang-tidy-check
clang-tidy-check:
	clang-tidy -p $(BUILD_DIR) --warnings-as-errors='*' -header-filter='.*' $(SOURCES)

.PHONY: clang-format-check
clang-format-check:
	clang-format --Werror --dry-run --ferror-limit=0 $(SOURCES)

.PHONY: lint
lint: clang-format-check clang-tidy-check
	@echo "Lint checks passed."

.PHONY: clean
clean:
	cmake --build $(BUILD_DIR) --target clean
