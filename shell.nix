let
  pkgs = import <nixpkgs> { };

in
pkgs.mkShell {
  name = "cpp-playground";

  buildInputs = with pkgs; [
    # clang-tools must come before clang, otherwise clang-unwrapped's
    # bare clang-tidy/clang-format shadow clang-tools' wrapped versions
    # (which auto-inject nix include paths via clang-wrapper nix-support).
    clang-tools
    clang
    gnumake
    cmake
    ninja
    llvmPackages.llvm
  ];

  shellHook = ''
    export CC=clang
    export CXX=clang++
  '';
}
