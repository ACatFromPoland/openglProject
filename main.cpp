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

void Loop()
{
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float yaw = -90.0f;
	float pitch = 0.0f;
	float sensitivity = 0.1f;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

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
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Draw
		drawTerrain();

		SDL_GL_SwapWindow(gGraphicsApplicationWindow);
	}
}

int main(int argc, char* argv[])
{
	// SDL, OPENGL Setup
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	gGraphicsApplicationWindow = SDL_CreateWindow("Project Graphics", 
													50, 50, 
													gScreenWidth, gScreenHeight, 
													SDL_WINDOW_OPENGL);

	gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	initTerrainGeometry();

	Loop();

	SDL_DestroyWindow(gGraphicsApplicationWindow);
	SDL_Quit();
	return 0;
}