let
  pkgs = import <nixpkgs> { };

in
pkgs.mkShell {
  name = "cpp-playground";

  buildInputs = with pkgs; [
    # clang-tools must come before clang, otherwise clang-unwrapped's
    # bare clang-tidy/clang-format shadow clang-tools' wrapped versions
    # (which auto-inject nix include paths via clang-wrapper nix-support).
    # clang-tools
    # clang
    llvmPackages_22.clang-tools
    llvmPackages_22.clang
    gnumake
    cmake
    ninja
    llvmPackages.llvm
    pre-commit
  ];

  shellHook = ''
    export CC=clang
    export CXX=clang++
  '';
}
