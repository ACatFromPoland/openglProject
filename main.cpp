#include <SDL.h>
#include "glad/glad.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int gScreenHeight = 720;
int gScreenWidth = 960;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

#include "shadertools.h"
#include "scenetools.h"

#include "model.h"

#include "terraingeneration.h"
#include "terraingeometry.h"

void runGameScene();

void generateMapData();
void drawMap();

int main(int argc, char* argv[])
{
	// SDL, OPENGL Setup
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	gGraphicsApplicationWindow = SDL_CreateWindow("Project Graphics", 
													100, 35, 
													gScreenWidth, gScreenHeight, 
													SDL_WINDOW_OPENGL);

	gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	runGameScene();

	SDL_DestroyWindow(gGraphicsApplicationWindow);
	SDL_Quit();
	return 0;
}

void runGameScene()
{
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float yaw = -90.0f;
	float pitch = 0.0f;
	float sensitivity = 0.1f;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

	//initTerrainGeometry();
	//generateCaveChunk(g_terrainGeometry.chunk);
	generateMapData();

	while (true)
	{
		float currentFrame = SDL_GetTicks() / 1000.0f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return;
			}

			if (e.type == SDL_MOUSEMOTION)
			{
				float offsetX = e.motion.xrel * sensitivity;
				float offsetY = e.motion.yrel * sensitivity;

				yaw += offsetX;
				pitch -= offsetY;

				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				glm::vec3 front;
				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				gCameraMatrixs.cameraFront = glm::normalize(front);
			}

			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			float cameraSpeed = 50.0f * deltaTime;

			if (currentKeyStates[SDL_SCANCODE_W])
				gCameraMatrixs.cameraPos += cameraSpeed * gCameraMatrixs.cameraFront;
			if (currentKeyStates[SDL_SCANCODE_S])
				gCameraMatrixs.cameraPos -= cameraSpeed * gCameraMatrixs.cameraFront;
			if (currentKeyStates[SDL_SCANCODE_A])
				gCameraMatrixs.cameraPos -= glm::normalize(glm::cross(gCameraMatrixs.cameraFront, gCameraMatrixs.cameraUp)) * cameraSpeed;
			if (currentKeyStates[SDL_SCANCODE_D])
				gCameraMatrixs.cameraPos += glm::normalize(glm::cross(gCameraMatrixs.cameraFront, gCameraMatrixs.cameraUp)) * cameraSpeed;
		}

		// Predraw
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glViewport(0, 0, gScreenWidth, gScreenHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Draw
		//drawTerrain();
		
		drawMap();
			//Draws terrain
			//Draws terrain static solids
			//Draws map entities
		
		//drawEntities();
			//Draws unit entities
		
		//drawUI();
			//Draw ui on screen

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::string error;

			switch (err)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			default:                               error = "UNKNOWN_ERROR"; break;
			}

			std::cerr << "[OpenGL Error] (" << error << "): " << std::hex << err << std::endl;
		}

		SDL_GL_SwapWindow(gGraphicsApplicationWindow);
	}
}

struct terrainDataStruct
{
	t_Vec3* vertices;
	int verticeCount;

	unsigned int* indices;
	int indiceCount;
};

void generateMapData()
{
	/*
	terrainDataStruct terraindata = {};

	int verticeWidth = 4;
	int verticeHeight = 2;

	terraindata.verticeCount = verticeHeight * verticeWidth;
	terraindata.vertices = new t_Vec3[terraindata.verticeCount];

	for (int y = 0; y < verticeHeight; y++)
	{
		int base = y * verticeWidth;
		for (int x = 0; x < verticeWidth; x++)
		{
			int index = base + x;
			t_Vec3* v = &terraindata.vertices[index];
			//memset(v->v, 0, sizeof(float) * 3);
			v->x = x * 10.0f;
			v->z = 0.0f;
			v->y = y * 10.0f;
		}
	}

	terraindata.indices = new unsigned int[8];
	terraindata.indiceCount = 8;
	unsigned int indiceValues[8] = { 0, 4, 1, 5, 2, 6, 3, 7};
	memcpy_s(terraindata.indices, sizeof(unsigned int) * terraindata.indiceCount, indiceValues, sizeof(unsigned int) * terraindata.indiceCount);
	*/

	std::vector<float> vertices = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.8f, 0.0f
	};

	std::vector<unsigned int> indices = {
		0, 1, 2 // The three vertices in order
	};

	// Terrain data layout for opengl
	{
		glGenVertexArrays(1, &g_terrainGeometry.VAO);
		glBindVertexArray(g_terrainGeometry.VAO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vec3), (void*)0);

		glGenBuffers(1, &g_terrainGeometry.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, g_terrainGeometry.VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, terraindata.verticeCount * sizeof(t_Vec3), terraindata.vertices, GL_STATIC_DRAW);


		//glGenBuffers(1, &g_terrainGeometry.EBO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_terrainGeometry.EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	{
		std::string vertexShaderSource = LoadShaderAsString("shaders/terrainvert.glsl");
		std::string fragmentShaderSource = LoadShaderAsString("shaders/terrainfrag.glsl");
		g_terrainGeometry.terrainShader = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	}
}

void drawMap()
{
	//gCameraMatrixs.model = glm::translate(gCameraMatrixs.model, glm::vec3(g_terrainGeometry.chunkOriginPosition.x, g_terrainGeometry.chunkOriginPosition.y, g_terrainGeometry.chunkOriginPosition.z));
	//gCameraMatrixs.view = glm::lookAt(gCameraMatrixs.cameraPos, gCameraMatrixs.cameraPos + gCameraMatrixs.cameraFront, gCameraMatrixs.cameraUp);
	//gCameraMatrixs.projection = glm::perspective(glm::radians(45.0f), (float)gScreenWidth / (float)gScreenHeight, 0.1f, 2000.0f);

	//

	//GLint modelLoc = glGetUniformLocation(g_terrainGeometry.terrainShader, "model");
	//GLint viewLoc = glGetUniformLocation(g_terrainGeometry.terrainShader, "view");
	//GLint projLoc = glGetUniformLocation(g_terrainGeometry.terrainShader, "projection");

	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.model));
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.view));
	//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.projection));

	glUseProgram(g_terrainGeometry.terrainShader);
	glBindVertexArray(g_terrainGeometry.VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

