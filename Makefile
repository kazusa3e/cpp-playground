preset := development
bindir := build

.PHONY: none
none:
	@echo "Usage: make <target>"
	@echo ""
	@echo "  configure     cmake --preset=<preset>"
	@echo "  build         cmake --build --preset=<preset>"
	@echo "  test          ctest --preset=<preset>"
	@echo "  format        apply clang-format to C/C++ sources"
	@echo "  format-check  verify C/C++ source formatting"
	@echo "  lint          verify formatting and lint all project sources with development"

.PHONY: configure
configure:
	cmake --preset=$(preset)

.PHONY: build
build: configure
	cmake --build --preset=$(preset)

.PHONY: test
test: build
	ctest --preset=$(preset)

.PHONY: format
format:
	@find src lib tests examples snippets include -type f \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -i --style=file {} +

.PHONY: format-check
format-check:
	@find src lib tests examples snippets include -type f \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format --dry-run --Werror --style=file {} +

.PHONY: lint
lint: format-check
	cmake --preset=development
	@find src lib tests examples snippets -type f -name '*.cpp' -exec clang-tidy -p build --quiet {} +
	@echo "lint finished"

.PHONY: clean
clean:
	$(RM) -r $(bindir)
