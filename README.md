
# GLPaper
GLPaper is a wallpaper program for wlroots based wayland compositors such as sway that allows you to render glsl shaders as your wallpaper

[![builds.sr.ht status](https://builds.sr.ht/~scoopta/glpaper.svg)](https://builds.sr.ht/~scoopta/glpaper?)
## Dependencies
	libwayland-dev
	libegl-dev
	pkg-config
	meson
## Building
	hg clone https://hg.sr.ht/~scoopta/glpaper
	cd glpaper
	meson setup build
	ninja -C build
## Installing
	sudo ninja -C build install
## Uninstalling
	sudo ninja -C build uninstall
## Bug Reports
Please file bug reports at https://todo.sr.ht/~scoopta/glpaper
## Contributing
Please submit patches to https://lists.sr.ht/~scoopta/glpaper

You can find documentation here https://man.sr.ht/hg.sr.ht/email.md
## Packages
If you're on Arch there's an unofficial AUR package https://aur.archlinux.org/packages/glpaper-hg/
