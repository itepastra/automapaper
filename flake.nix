{
  description = "A wallpaper that does automatons";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
  };

  outputs = { nixpkgs, ... }:
    let
      allSystems = [
        "x86_64-linux" # 64-bit Intel/AMD Linux
        "aarch64-linux" # 64-bit ARM Linux
        "x86_64-darwin" # 64-bit Intel macOS
        "aarch64-darwin" # 64-bit ARM macOS
      ];
      forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f {
        inherit system;
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {

      packages = forAllSystems ({ system, pkgs, ... }:
        pkgs.stdenv.mkDerivation {
          name = "automapaper";
          src = ./.;
          nativeBuildInputs = with pkgs; [ meson ninja pkg-config pacman libarchive ];
          buildInputs = with pkgs; [ wayland libGL ];
          configurePhase = "meson setup build";
          buildPhase = "ninja -C build";
          installPhase = ''
            mkdir -p "$out/bin"
            mv build/automapaper "$out/bin"
          '';
        });

      devShell = forAllSystems ({ system, pkgs, ... }:
        pkgs.mkShell {
          buildInputs = with pkgs; [
            meson
            ninja
          ];
        });
    };
}

