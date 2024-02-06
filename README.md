# Automapaper
Automapaper is a wallpaper program for wlroots based wayland compositors such as sway that allows you to render glsl shaders as your wallpaper

## Todo
 - [ ] add config option for wrapping the state texture vs clamping
 - [ ] add config option for amount of cycles before reset
 - [ ] add mouse input
 - [ ] non-absolute paths in the config file
 - [ ] add support for multi screen from one call

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
## Running
	if it's not installed, you can run it from the build folder using
	./automapaper -C {config}
## Bug Reports
Please file bug reports at https://github.com/itepastra/automapaper/issues
## Contributing
Please submit patches to https://github.com/itepastra/automapaper/pulls

# References  
This repo is based on of [glpaper](https://hg.sr.ht/~scoopta/glpaper).  
It also uses a [toml parser](https://github.com/cktan/tomlc99).
