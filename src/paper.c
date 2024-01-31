/*
 *  Copyright (C) 2019-2020 Scoopta
 *  This file is part of GLPaper
 *  GLPaper is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GLPaper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GLPaper.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <paper.h>

#include <time.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <glad/glad_egl.h>

#include <wayland-egl.h>
#include <wayland-client.h>
#include <xdg-output-unstable-v1-client-protocol.h>
#include <wlr-layer-shell-unstable-v1-client-protocol.h>

#define PROTO_VERSION(v1, v2) (v1 < v2 ? v1 : v2)

static const char* monitor;
static struct node* output = NULL;
static struct wl_list outputs;
static struct wl_compositor* comp;
static struct zwlr_layer_shell_v1* shell;
static struct zxdg_output_manager_v1* output_manager;
static time_t start;

struct node {
	struct wl_output* output;
	int32_t width, height;
	struct wl_list link;
};

static void nop() {}

static void add_interface(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
	(void) data;
	if(strcmp(interface, wl_output_interface.name) == 0) {
		struct node* node = malloc(sizeof(struct node));
		node->output = wl_registry_bind(registry, name, &wl_output_interface, PROTO_VERSION(version, 4));
		wl_list_insert(&outputs, &node->link);
	} else if(strcmp(interface, wl_compositor_interface.name) == 0) {
		comp = wl_registry_bind(registry, name, &wl_compositor_interface, PROTO_VERSION(version, 4));
	} else if(strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
		output_manager = wl_registry_bind(registry, name, &zxdg_output_manager_v1_interface, PROTO_VERSION(version, 3));
	} else if(strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, PROTO_VERSION(version, 4));
	}
}

static void get_name(void* data, struct zxdg_output_v1* xdg_output, const char* name) {
	(void) xdg_output;
	struct node* node = data;
	if(strcmp(name, monitor) == 0) {
		output = node;
	}
}

static void config_surface(void* data, struct zwlr_layer_surface_v1* surface, uint32_t serial, uint32_t width, uint32_t height) {
	(void) data;
	(void) width;
	(void) height;
	zwlr_layer_surface_v1_ack_configure(surface, serial);
}

static void get_res(void* data, struct wl_output* output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
	(void) output;
	(void) refresh;
	if((flags & WL_OUTPUT_MODE_CURRENT) == WL_OUTPUT_MODE_CURRENT) {
		struct node* node = data;
		node->width = width;
		node->height = height;
	}
}

static void setup_fbo(GLuint* fbo, GLuint* prog, GLuint* texture, GLuint vert, uint16_t width, uint16_t height) {
	const char* frag_data[] = {
		"#version 100\n"
		"uniform sampler2D tex2D;"

		"varying highp vec2 texCoords;"

		"void main() {"
		"	gl_FragColor = texture2D(tex2D, texCoords);"
		"}"
	};

	*prog = glCreateProgram();
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	GLint frag_len[] = {strlen(frag_data[0])};
	glShaderSource(frag, 1, frag_data, frag_len);
	glCompileShader(frag);

	GLint status;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	if(!status) {
		char buff[255];
		glGetShaderInfoLog(frag, sizeof(buff), NULL, buff);
		fprintf(stderr, "Texture Frag: %s\n", buff);
		exit(1);
	}

	glAttachShader(*prog, vert);
	glAttachShader(*prog, frag);
	glLinkProgram(*prog);

	glBindAttribLocation(*prog, 0, "datIn");
	glBindAttribLocation(*prog, 1, "texIn");

	glGetProgramiv(*prog, GL_LINK_STATUS, &status);
	if(!status) {
		char buff[255];
		glGetProgramInfoLog(*prog, sizeof(buff), NULL, buff);
		fprintf(stderr, "Texture Shader: %s\n", buff);
		exit(1);
	}

	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void draw(GLuint prog) {
	glUseProgram(prog);
	glClear(GL_COLOR_BUFFER_BIT);
	GLint time_var = glGetUniformLocation(prog, "time");
	glUniform1f(time_var, (utils_get_time_millis() - start) / 1000.0f);
	GLint resolution = glGetUniformLocation(prog, "resolution");
	glUniform2f(resolution, output->width, output->height);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void draw_fbo(GLuint fbo, GLuint texture, GLuint main_prog, GLuint final_prog, uint16_t width, uint16_t height) {
	glViewport(0, 0, width, height);
	glUseProgram(main_prog);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLint time_var = glGetUniformLocation(main_prog, "time");
	glUniform1f(time_var, (utils_get_time_millis() - start) / 1000.0f);
	GLint resolution = glGetUniformLocation(main_prog, "resolution");
	glUniform2f(resolution, width, height);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glViewport(0, 0, output->width, output->height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(final_prog);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint tex2D = glGetUniformLocation(final_prog, "tex2D");
	glUniform1i(tex2D, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void paper_init(char* _monitor, char* frag_path, uint16_t fps, char* layer_name, uint16_t width, uint16_t height) {
	monitor = _monitor;
	start = utils_get_time_millis();
	wl_list_init(&outputs);
	struct wl_display* wl = wl_display_connect(NULL);


	struct wl_registry* registry = wl_display_get_registry(wl);
	struct wl_registry_listener reg_listener = {
		.global = add_interface,
		.global_remove = nop
	};
	wl_registry_add_listener(registry, &reg_listener, NULL);
	wl_display_roundtrip(wl);


	struct node* node;
	wl_list_for_each(node, &outputs, link) {
		struct zxdg_output_v1* xdg_output = zxdg_output_manager_v1_get_xdg_output(output_manager, node->output);
		struct zxdg_output_v1_listener xdg_listener = {
			.description = nop,
			.done = nop,
			.logical_position = nop,
			.logical_size = nop,
			.name = get_name
		};
		zxdg_output_v1_add_listener(xdg_output, &xdg_listener, node);

		struct wl_output_listener out_listener = {
			.done = nop,
			.geometry = nop,
			.mode = get_res,
			.scale = nop,
			.name = nop,
			.description = nop
		};
		wl_output_add_listener(node->output, &out_listener, node);
	}
	wl_display_roundtrip(wl);

	if(output == NULL) {
		fprintf(stderr, ":/ sorry about this but we can't seem to find that output\n");
		exit(1);
	}


	struct wl_surface* wl_surface = wl_compositor_create_surface(comp);
	struct wl_region* input_region = wl_compositor_create_region(comp);
	struct wl_region* render_region = wl_compositor_create_region(comp);
	wl_region_add(render_region, 0, 0, output->width, output->height);
	wl_surface_set_opaque_region(wl_surface, render_region);
	wl_surface_set_input_region(wl_surface, input_region);
	wl_display_roundtrip(wl);

	enum zwlr_layer_shell_v1_layer layer;

	if(layer_name == NULL) {
		layer = ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND;
	} else if(strcasecmp(layer_name, "top") == 0) {
		layer = ZWLR_LAYER_SHELL_V1_LAYER_TOP;
	} else if(strcasecmp(layer_name, "bottom") == 0) {
		layer = ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM;
	} else if(strcasecmp(layer_name, "background") == 0) {
		layer = ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND;
	} else if(strcasecmp(layer_name, "overlay") == 0) {
		layer = ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY;
	} else {
		layer = ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND;
	}
	struct zwlr_layer_surface_v1* surface = zwlr_layer_shell_v1_get_layer_surface(shell, wl_surface, output->output, layer, "glpaper");
	zwlr_layer_surface_v1_set_exclusive_zone(surface, -1);
	zwlr_layer_surface_v1_set_size(surface, output->width, output->height);
	struct zwlr_layer_surface_v1_listener surface_listener = {
		.closed = nop,
		.configure = config_surface
	};
	zwlr_layer_surface_v1_add_listener(surface, &surface_listener, NULL);
	wl_surface_commit(wl_surface);
	wl_display_roundtrip(wl);

	struct wl_egl_window* window = wl_egl_window_create(wl_surface, output->width, output->height);
	eglBindAPI(EGL_OPENGL_ES_API);
	EGLDisplay egl_display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_KHR, wl, NULL);
	eglInitialize(egl_display, NULL, NULL);
	const EGLint win_attrib[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_NONE
	};

	EGLConfig config;
	EGLint config_len;
	eglChooseConfig(egl_display, win_attrib, &config, 1, &config_len);
	const EGLint ctx_attrib[] = {
		EGL_CONTEXT_MAJOR_VERSION, 2,
		EGL_CONTEXT_MINOR_VERSION, 0,
		EGL_NONE
	};
	EGLContext ctx = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, ctx_attrib);

	EGLSurface egl_surface = eglCreatePlatformWindowSurface(egl_display, config, window, NULL);
	eglMakeCurrent(egl_display, egl_surface, egl_surface, ctx);
	if(fps == 0) {
		eglSwapInterval(egl_display, 1);
	} else {
		eglSwapInterval(egl_display, 0);
	}

	gladLoadGLES2Loader((GLADloadproc) eglGetProcAddress);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, output->width, output->height);
	GLfloat vbo_data[] = {
		//Vertex Data	//Texture data
		-1.0f, 1.0f,	0.0f, 1.0f, //Top left
		-1.0f, -1.0f,	0.0f, 0.0f, //Bottom left
		1.0f, -1.0f,	1.0f, 0.0f, //Bottom right
		1.0f, 1.0f,		1.0f, 1.0f, //Top right
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

	GLuint ebo_data[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo_data), ebo_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	const char* vert_data[] = {
		"#version 100\n"
		"attribute highp vec2 datIn;"
		"attribute highp vec2 texIn;"

		"varying vec2 texCoords;"

		"void main() {"
		"	texCoords = texIn;"
		"	gl_Position = vec4(datIn, 0.0, 1.0);"
		"}"
	};

	GLuint shader_prog = glCreateProgram();
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLint vert_len[] = {strlen(vert_data[0])};
	glShaderSource(vert, 1, vert_data, vert_len);
	glCompileShader(vert);
	GLint status;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
	if(!status) {
		char buff[255];
		glGetShaderInfoLog(vert, sizeof(buff), NULL, buff);
		fprintf(stderr, "Vert: %s\n", buff);
		exit(1);
	}

	if(access(frag_path, R_OK) != 0) {
		fprintf(stderr, "I can't seem to find %s\n", frag_path);
		exit(1);
	}

	FILE* f = fopen(frag_path, "r");
	fseek(f, 0L, SEEK_END);
	size_t f_size = ftell(f);
	const char* frag_data[1];
	frag_data[0] = calloc(1, f_size + 1);
	fseek(f, 0L, SEEK_SET);
	fread((void*) frag_data[0], 1, f_size, f);
	fclose(f);

	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	GLint frag_len[] = {f_size};
	glShaderSource(frag, 1, frag_data, frag_len);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	if(!status) {
		char buff[255];
		glGetShaderInfoLog(frag, sizeof(buff), NULL, buff);
		fprintf(stderr, "Frag: %s\n", buff);
		exit(1);
	}

	glAttachShader(shader_prog, vert);
	glAttachShader(shader_prog, frag);
	glLinkProgram(shader_prog);
	glBindAttribLocation(shader_prog, 0, "datIn");
	glBindAttribLocation(shader_prog, 1, "texIn");

	glGetProgramiv(shader_prog, GL_LINK_STATUS, &status);
	if(!status) {
		char buff[255];
		glGetProgramInfoLog(shader_prog, sizeof(buff), NULL, buff);
		fprintf(stderr, "Shader: %s\n", buff);
		exit(1);
	}


	bool use_fbo = width > 0 || height > 0;
	GLuint fbo = 0;
	GLuint final_prog = 0;
	GLuint render_tex = 0;
	if(use_fbo) {
		if(width == 0) {
			width = output->width;
		}
		if(height == 0) {
			height = output->height;
		}
		setup_fbo(&fbo, &final_prog, &render_tex, vert, width, height);
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
	glUseProgram(shader_prog);

	time_t frame_start;

	while(true) {
		frame_start = utils_get_time_millis();
		if(wl_display_flush(wl) == -1) {
			exit(0);
		}
		if(use_fbo) {
			draw_fbo(fbo, render_tex, shader_prog, final_prog, width, height);
		} else {
			draw(shader_prog);
		}
		eglSwapBuffers(egl_display, egl_surface);
		if(fps != 0) {
			int64_t sleep = (1000 / fps) - (utils_get_time_millis() - frame_start);
			utils_sleep_millis(sleep >= 0 ? sleep : 0);
		}
	}
}
