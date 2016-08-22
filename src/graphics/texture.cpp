/*
 * Copyright (C) 2012 Marc-Olivier Bloch <wormsparty [at] gmail [dot] com>
 *
 * This file is part of the 'Beautiful, absurd, subtle.' project.
 *
 * 'Beautiful, absurd, subtle.' is free software: you can redistribute it 
 * and/or modify it under the terms of the 'New BSD License'.
 *
 */

#include <GL/glew.h>

#include "../include/texture.h"
#include "lodepng/lodepng.h"

#include <iostream>

//#define DEBUG_TEXTURE

struct shader_t
{
    GLuint program, vertexShader, fragmentShader;
    GLint positionLoc, texCoordLoc, samplerLoc, lightLoc, projectionLoc;
    GLuint vertexObject, indexObject;
    GLfloat vertices[16]; // 4 vertices x (2 position coordinates + 2 texture coordinate
};

struct texture_t
{
    int frame_count; // the frame counts for animation
    float frame_duration;

    GLuint texid; // the opengl texture id

    int width; // the texture size
    int height;

#if DEBUG_TEXTURE
	std::string filename;
#endif
};

static shader_t default_shader;

static const char default_vertex_shader[] =
    "uniform mat4 projection;                                \n"
    "attribute vec2 position;                                \n"
    "attribute vec2 a_texCoord;                              \n"
    "varying vec2 v_texCoord;                                \n"
    "void main()                                             \n"
    "{                                                       \n"
    "   gl_Position = projection * vec4(position, 0.0, 1.0); \n"
    "   v_texCoord = a_texCoord;                             \n"
    "}                                                       \n";

static const char default_fragment_shader[] =
    "#ifdef GL_ES                                                       \n"
    "     precision lowp float;                                         \n"
    "#endif                                                             \n"
    "varying vec2 v_texCoord;                                           \n"
    "uniform sampler2D s_texture;                                       \n"
    "uniform vec4 s_light;                                              \n"
    "void main()                                                        \n"
    "{                                                                  \n"
    "  gl_FragColor = s_light * texture2D(s_texture, v_texCoord);       \n"
    "}                                                                  \n";

static int texture_count = 0;

static void logShaderError(GLuint shader)
{
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if(infoLen > 1)
    {
        auto infoLog = (char*)malloc(infoLen);
        glGetShaderInfoLog (shader, infoLen, NULL, infoLog);
        std::cout << infoLog << std::endl;
        free(infoLog);
    }
}

static void logProgramError(shader_t* shader)
{
    GLint infoLen = 0;
    glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &infoLen);

    if(infoLen > 0)
    {
        auto infoLog = (char*)malloc(infoLen);
        glGetProgramInfoLog(shader->program, infoLen, nullptr, infoLog);
        std::cout << infoLog << std::endl;
        free(infoLog);
    }
}

static void loadShader(GLuint program, GLuint shader, const GLchar *shaderSrc) {
    GLint compiled = 0;

    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);
    glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE)
    {
        logShaderError(shader);
    }

    glAttachShader(program, shader);
}

int texture_begin()
{
    GLint success = 0;
	shader_t* shader = &default_shader;

    shader->program = glCreateProgram();
    shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    loadShader(shader->program, shader->vertexShader, default_vertex_shader);
    loadShader(shader->program, shader->fragmentShader, default_fragment_shader);
    glLinkProgram(shader->program);
    glGetProgramiv (shader->program, GL_LINK_STATUS, &success);

    if (success == GL_TRUE)
    {
        glValidateProgram(shader->program);
        glGetProgramiv (shader->program, GL_VALIDATE_STATUS, &success);
    }

    if (success == GL_FALSE)
    {
        logProgramError(shader);
        return 0;
    }

    glUseProgram(shader->program);

    shader->positionLoc = glGetAttribLocation(shader->program, "position");
    shader->texCoordLoc = glGetAttribLocation(shader->program, "a_texCoord");
    shader->samplerLoc = glGetUniformLocation(shader->program, "s_texture" );
    shader->lightLoc = glGetUniformLocation(shader->program, "s_light" );
    shader->projectionLoc = glGetUniformLocation(shader->program, "projection");

    glGenBuffers(1, &shader->vertexObject);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shader->vertices), shader->vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    static GLushort indices[] = { 0, 1, 2, 2, 1, 3 };

    glGenBuffers(1, &shader->indexObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->indexObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 1;
}

void texture_render(texture_t* texture, int frame_current, GLfloat* projection, opengl_state_t* state)
{
    shader_t* shader = &default_shader;

    GLfloat left, right, width, height;
    GLfloat scalex = state->scalex;
    GLfloat scaley = state->scaley;

    glUseProgram(shader->program);

    glUniformMatrix4fv(shader->projectionLoc, 1, GL_FALSE, projection);

    width = (GLfloat)texture_frame_width(texture);
    height = (GLfloat)texture_frame_height(texture);
    left = (GLfloat) frame_current / texture->frame_count;
    right = (GLfloat) (frame_current + 1) / texture->frame_count;

    shader->vertices[0] = state->px; // position 0
    shader->vertices[1] = state->py;
    shader->vertices[4] = state->px + width * scalex; // position 1
    shader->vertices[5] = state->py;
    shader->vertices[8] = state->px; // position 2
    shader->vertices[9] = state->py + height * scaley;
    shader->vertices[12] = state->px + width * scalex; // position 3
    shader->vertices[13] = state->py + height * scaley;

    shader->vertices[2] = left; // texture 0
    shader->vertices[3] = 0.0f;
    shader->vertices[6] = right; // texture 1
    shader->vertices[7] = 0.0f;
    shader->vertices[10] = left; // texture 2
    shader->vertices[11] = 1.0f;
    shader->vertices[14] = right; // texture 3
    shader->vertices[15] = 1.0f;

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shader->vertices), shader->vertices);
    glVertexAttribPointer(shader->positionLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(shader->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(shader->positionLoc);
    glEnableVertexAttribArray(shader->texCoordLoc);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->texid);
    glUniform1i(shader->samplerLoc, 0);

    glUniform4f(shader->lightLoc, state->r, state->g, state->b, state->a);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->indexObject);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (GLvoid*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void shader_close(shader_t* shader)
{
    glDeleteShader(shader->fragmentShader);
    glDeleteShader(shader->vertexShader);
    glDeleteShader(shader->program);
}

static GLuint opengl_load(unsigned char* image, unsigned width, unsigned height)
{
    GLuint texid;
    int pack, unpack;

    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            (GLsizei)width,
            (GLsizei)height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            image);

    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texid;
}

texture_t* texture_from_bytes(unsigned char* bytes, int width, int height)
{
    GLuint texid = opengl_load(bytes, width, height);

    if (texid == 0)
    {
        std::cout << "Failed to load image from bytes." << std::endl;
        return nullptr;
    }

    auto texture = new texture_t;

    texture->texid = texid;
    texture->width = width;
    texture->height = height;
    texture->frame_count = 1;

#ifdef DEBUG_TEXTURE
	texture->filename = "From bytes";
#endif

	texture_count++;

    return texture;
}

texture_t* texture_open(const std::string filename, int frame_count, float frame_duration)
{
    unsigned width, height;
    unsigned char* image;
    unsigned error = lodepng_decode32_file(&image, &width, &height, filename.c_str());

    if(error)
    {
        std::cout << "File '" << filename << "' could not be loaded: " << lodepng_error_text(error) << std::endl;
        return nullptr;
    }

	GLuint texid = opengl_load(image, width, height);

    if (texid == 0)
    {
        std::cout << "Failed to load image " << filename << " to OpenGL context." << std::endl;
        return nullptr;
    }

	auto texture = new texture_t;

    texture->texid = texid;
    texture->width = width;
    texture->height = height;
	texture->frame_duration = frame_duration;
	texture->frame_count = frame_count;
	texture_count++;

#ifdef DEBUG_TEXTURE
	texture->filename = filename;
	std::cout << "Loaded " << filename << std::endl;
#endif

	return texture;
}

void texture_close(texture_t* texture)
{
	if (texture != nullptr)
	{
#ifdef DEBUG_TEXTURE
		std::cout << "Unloaded " << texture->filename << std::endl;
#endif

		glDeleteTextures(1, &texture->texid);
		delete texture;
		texture_count--;
	}
}

int texture_frame_width(texture_t* texture)
{
    if (texture->frame_count != 0)
    {
        return texture->width / texture->frame_count;
    }

    return 0;
}

int texture_frame_height(texture_t* texture)
{
    return texture->height;
}

int texture_frame_count(texture_t* texture)
{
    return texture->frame_count;
}

float texture_frame_duration(texture_t* texture)
{
    return texture->frame_duration;
}

void texture_finish()
{
	shader_close(&default_shader);

    if (texture_count != 0)
    {
        std::cout << "Asymetrical number of calls to texture_open and texture_close: " << texture_count << std::endl;
    }
}

