preset := development
bindir := build

.PHONY: none
none:
	@echo "Usage: make <target>"
	@echo ""
	@echo "  configure  cmake --preset=<preset>"
	@echo "  build      cmake --build --preset=<preset>"
	@echo "  test       ctest --preset=<preset>"
	@echo "  format     clang-format -i on all .cpp/.hpp"
	@echo "  lint       clang-tidy on src/ lib/ tests/ examples/"

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
	@find src lib tests examples include -type f \( -name '*.cpp' -o -name '*.hpp' \) -print | xargs clang-format -i --style=file

.PHONY: lint
lint: configure
	@find src lib tests examples -type f -name '*.cpp' -print | xargs clang-tidy -p $(bindir) --quiet; \
	echo "lint finished (see above for warnings)"

.PHONY: clean
clean:
	$(RM) -r $(bindir)
