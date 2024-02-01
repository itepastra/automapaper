# Fork  
This repo is a fork of [glpaper](https://hg.sr.ht/~scoopta/glpaper).

# Automapaper
Automapaper is a wallpaper program for wlroots based wayland compositors such as sway that allows you to render glsl shaders as your wallpaper

## Dependencies
	libwayland-dev
	libegl-dev
	pkg-config
	meson
## Building
	git clone https://github.com/itepastra/automapaper.git
	cd automapaper
	meson setup build
	ninja -C build
## Installing
	sudo ninja -C build install
## Uninstalling
	sudo ninja -C build uninstall
## Bug Reports
Please file bug reports at https://github.com/itepastra/automapaper/issues
## Contributing
Please submit patches to https://github.com/itepastra/automapaper/pulls

