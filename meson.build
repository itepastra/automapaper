project('glpaper', 'c', default_options : ['buildtype=release', 'warning_level=2'])
cc = meson.get_compiler('c')

inc = include_directories('inc')
wayland = dependency('wayland-client')
wl_egl = dependency('wayland-egl')
egl = dependency('egl')

executable(meson.project_name(),
'src/main.c',
'src/paper.c',
'src/utils.c',
'glad/glad.c',
'proto/wlr-layer-shell-unstable-v1-protocol.c',
'proto/xdg-output-unstable-v1-protocol.c',
'proto/xdg-shell-protocol.c',
include_directories : inc,
dependencies : [wayland, wl_egl, egl], install : true)