{
		description = "A shader based wallpaper that can be used for cellular automata";

		inputs.nixpkgs.url = github:NixOs/nispkgs/nixos-unstable;

		outputs = {self, nixpkgs}: {
				defaultPackage.x86_64-linux =
						with import nixpkgs { system = "x86_64-linux"; };
						stdenv.mkDerivation {
								name = "automapaper"
								src = self;
								buildPhase = "meson setup build";
								installPhase = "meson install";
						};
		};
}
