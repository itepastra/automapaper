{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
  };

  outputs = { nixpkgs, ... }: {
    
    packages.x86_64-linux.default = 
    with import nixpkgs { system = "x86_64-linux"; };
    stdenv.mkDerivation {
      name = "automapaper";
      src = ./.;
      nativeBuildInputs = with nixpkgs; [ meson ninja pkg-config pacman libarchive ];
      buildInputs = with nixpkgs; [ wayland libGL ];
      configurePhase = "meson setup build";
      buildPhase = "ninja -C build";
      installPhase = ''
        mkdir -p $out/bin
        mv build/automapaper $out/bin
      '';
    };
  };
}

