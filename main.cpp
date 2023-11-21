#include <SDL.h>
#include "glad/glad.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObjectPosition = 0;
GLuint gVertexBufferObjectColor = 0;
GLuint gGraphicsPipelineShaderProgram = 0;

int gScreenHeight = 720;
int gScreenWidth = 960;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

t_Model model;

struct Camera
{
	glm::mat4 model = glm::mat4(0.5f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
};

Camera gCameraMatrixs = {};

std::string LoadShaderAsString(const std::string& filename)
{
	std::string result = "";
	std::ifstream file(filename.c_str());

	if (file.is_open())
	{
		std::string line = "";
		while (std::getline(file, line))
		{		
			result += line + '\n';
		}
		file.close();
	}

	return result;
}

GLuint CompileShader(GLuint type, const std::string& source)
{
	GLuint shaderObject;
	if (type == GL_VERTEX_SHADER)
	{
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == GL_FRAGMENT_SHADER)
	{
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else
	{
		shaderObject = glCreateShader(GL_SHADER);
	}
	
	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	GLuint programObject = glCreateProgram();
	GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	
	glAttachShader(programObject, myVertexShader);
	glAttachShader(programObject, myFragmentShader);
	glLinkProgram(programObject);

	glValidateProgram(programObject);

	return programObject;
}

void Loop()
{
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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
				cameraFront = glm::normalize(front);
			}

			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			float cameraSpeed = 50.0f * deltaTime;

			if (currentKeyStates[SDL_SCANCODE_W])
				cameraPos += cameraSpeed * cameraFront;
			if (currentKeyStates[SDL_SCANCODE_S])
				cameraPos -= cameraSpeed * cameraFront;
			if (currentKeyStates[SDL_SCANCODE_A])
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			if (currentKeyStates[SDL_SCANCODE_D])
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

		}

		// Predraw
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glViewport(0, 0, gScreenWidth, gScreenHeight);
		glClearColor(0.003f, 0.137f, 0.172f, 1.0f);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Draw
		float angle = fmod(currentFrame / 10.0f, 1.0f) * 720.0f;
		float angleRadians = glm::radians(angle);
		glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		gCameraMatrixs.model = modelMatrix;//glm::translate(gCameraMatrixs.model, glm::vec3(0.0f, 0.0f, 0.0f));
		gCameraMatrixs.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		gCameraMatrixs.projection = glm::perspective(glm::radians(45.0f), (float)gScreenWidth / (float)gScreenHeight, 0.1f, 1000.0f);

		glUseProgram(gGraphicsPipelineShaderProgram);

		GLint modelLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "view");
		GLint projLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.projection));

		glBindVertexArray(gVertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjectPosition);
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjectColor);

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.vertices.size());

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

	// Create some vertex data for testing
	{
		loadModel(model, "Submarine.obj");

		// Positions
		glGenVertexArrays(1, &gVertexArrayObject);
		glBindVertexArray(gVertexArrayObject);

		glGenBuffers(1, &gVertexBufferObjectPosition);
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjectPosition);
		glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(t_VertexStruct), model.vertices.data(), GL_STATIC_DRAW);

		GLsizei stride = sizeof(t_VertexStruct);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0 );

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(t_Vec3) );
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(t_Vec3) * 2) );

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(t_Vec3) * 2 + sizeof(t_Vec2)) );

		glBindVertexArray(0); // Unbind buffer

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}

	// Graphics pipeline init
	{
		std::string vertexShaderSource = LoadShaderAsString("shaders/vert.glsl");
		std::string fragmentShaderSource = LoadShaderAsString("shaders/frag.glsl");
		gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	}

	// Enable logic
	Loop();

	SDL_DestroyWindow(gGraphicsApplicationWindow);
	SDL_Quit();
	return 0;
}