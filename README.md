# kuri — Modern C++23 Project Template

## Prerequisites

- CMake ≥ 3.30
- Ninja
- A C++23-capable compiler (Clang 18+ / GCC 14+)

```bash
# Arch Linux
sudo pacman -S cmake ninja
```

## Build & Run

| Preset       | Build Type | Compiler | Extra Flags          |
|--------------|-----------|----------|----------------------|
| development  | Debug     | clang++  | strict + sanitizers  |
| test         | Debug     | clang++  | strict + sanitizers  |
| release      | Release   | g++      | none                 |

```bash
# Configure
cmake --preset=development

# Build
cmake --build --preset=development

# Run
./build/src/main

# Run tests
ctest --preset=development

# Run example
./build/examples/example_core
```

**Note:** All presets share the `build/` directory. When switching compilers or build types, reconfigure from scratch:

```bash
cmake --preset=development --fresh
```

## Customization

### Change compiler or build type

Edit `CMakePresets.json` or pass on the command line:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_BUILD_TYPE=Debug
```

### Toggle strict checks / sanitizers

```bash
cmake -S . -B build -G Ninja \
  -DENABLE_STRICT_CHECKS=ON \
  -DENABLE_SANITIZERS=ON
```

### Change C++ standard

Each target specifies its standard individually via `target_compile_features(... cxx_std_23)`. To override, edit the per-target `CMakeLists.txt` or add a CMake variable.

## License

TODO
