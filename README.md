# Automapaper
Automapaper is a wallpaper program for wlroots based wayland compositors such as sway that allows you to render glsl es shaders as your wallpaper
your shaders have to use one of the GLES versions:
```
#version 100
#version 300 es
#version 310 es
#version 320 es
```

## Todo
 - [ ] add mouse input

## Configuration
A template config is provided in the defaults/ folder of the repository. You can use and edit it to your liking. Some default shaders are also included.

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

# References  
This repo is based on of [glpaper](https://hg.sr.ht/~scoopta/glpaper).  
It also uses a [toml parser](https://github.com/cktan/tomlc99).
