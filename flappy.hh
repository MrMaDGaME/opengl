#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FLOOR_POSITION -0.9f
#define JUMP_FORCE 1.7f
#define GRAVITY 0.02f
#define PIPE_WIDTH 40.0f
#define PIPE_HEIGHT 250.0f
#define PIPE_GAP 180.0f
#define PIPE_SPEED 1.0f

std::string loadShaderSource(const char* filepath);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

bool checkCollision(float birdX, float birdY, float pipeX, float pipeY);

void updateBird();

void updatePipes();

void renderBitmapString(float x, float y, void *font, const char *string);

void render(GLuint textureID, GLuint pipeUpTextureID, GLuint pipeDownTextureID);

void update();
