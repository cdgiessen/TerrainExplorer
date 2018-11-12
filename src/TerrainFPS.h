#pragma once

#ifndef TERRAIN_FPS_H
#define TERRAIN_FPS_H

#include <iostream>

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL
#include "SOIL.h"

// Other includes	
#include "Shader.h"
#include "Camera.h"
#include "GridFloatData.h"
#include "Bullet.h"
class TerrainFPS;

void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode);
void OnMouseMoved(GLFWwindow* window, double xpos, double ypos);
void OnMouseButton(GLFWwindow* window, int glfwButton, int glfwAction);
void OnScrollMoved(GLFWwindow* window, double xoffset, double yoffset);
void do_movement(TerrainFPS terrain);
void ResetCamera();
void setup_window();

// Properties
GLuint WIDTH = 800, HEIGHT = 600;
std::string inputFileName = "ned_86879038";

bool Wireframe;
bool Disco = false;
bool flashLight = true;
bool shifted = false;
const float WalkSpeed = 1.34;

glm::vec3 sunDir(-0.2f, -1.0f, -0.3f);

std::vector<Bullet> bullets;
GLuint bulletVAO, bulletVBO;

GLfloat cubeVerts[] = {
	-0.5f, -0.5f, -0.5f, 
	0.5f, -0.5f, -0.5f,  
	0.5f,  0.5f, -0.5f,  
	0.5f,  0.5f, -0.5f,  
	-0.5f,  0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f, 

	-0.5f, -0.5f,  0.5f, 
	0.5f, -0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f, 
	-0.5f, -0.5f,  0.5f, 

	-0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f, 
	-0.5f, -0.5f,  0.5f, 
	-0.5f,  0.5f,  0.5f, 

	0.5f,  0.5f,  0.5f,  
	0.5f,  0.5f, -0.5f,  
	0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  

	-0.5f, -0.5f, -0.5f, 
	0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f,  0.5f,  
	0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f, 
	-0.5f, -0.5f, -0.5f, 

	-0.5f,  0.5f, -0.5f, 
	0.5f,  0.5f, -0.5f,  
	0.5f,  0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f, -0.5f
};

GLuint stone_texture;

// Camera
Camera camera(glm::vec3(0, 60, 0));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool mouseControl = false;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// GLFW window
GLFWwindow* window;

class TerrainFPS
{
public:
	TerrainFPS(std::string filename);
	~TerrainFPS();

	void setup_vao();
	void GeneratePolygonSurface();
	void GenSubDividedTerrainSurface();
	float CalcColorAtElevation(float elevation, int rgb);
	float Height(float x, float y); //returns the height of a cell at a given grid point
	float TerrainHeight(float x, float y);
	void DrawTerrain(Shader shader);

	// Terrain Data
	GridFloatData *data;
	int nrows;
	int ncols;
	GLuint terrainVBO, terrainVAO, terrainEBO;

	std::vector<GLfloat> terVerts;
	std::vector<GLuint> terIndices;

	float colorHeightMap[4][3] = {{ 0, 0.65f, 0.33f } ,{ 0.47f, 0.28f, 0.0f },{ 0.5f, 0.5f, 0.5f } ,{ 1.2f, 1.2f, 1.2f } };

	float BgColor[3];
	float terrainHeightScale;
};

#endif // !TERRAIN_FPS_H