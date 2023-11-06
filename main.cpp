#include <SDL.h>
#include "glad/glad.h"
#include <iostream>
#include <vector>

const std::string gVertexShaderSource =
	"#version 410 core\n"
	"in vec4 position;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position.x, position.y, position.z, position.w);"
	"}\n";

const std::string gFragmentShaderSource =
	"#version 410 core\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"	color = vec4(1.0f, 0.5f, 0.0f, 1.0f);"
	"}\n";

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gGraphicsPipelineShaderProgram = 0;

int gScreenHeight = 480;
int gScreenWidth = 640;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;


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
	while (true)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return;
			}
		}

		// Predraw
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glViewport(0, 0, gScreenWidth, gScreenHeight);
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glUseProgram(gGraphicsPipelineShaderProgram);

		// Draw
		glBindVertexArray(gVertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

		glDrawArrays(GL_TRIANGLES, 0, 3);

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
		const std::vector<GLfloat> vertexPosition{
			-0.8f, -0.8f, 0.0f,
			0.8f, -0.8f, 0.0f,
			0.0f, 0.8f, 0.0f
		};
		glGenVertexArrays(1, &gVertexArrayObject);
		glBindVertexArray(gVertexArrayObject);

		glGenBuffers(1, &gVertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
	}

	// Graphics pipeline init
	{
		gGraphicsPipelineShaderProgram = CreateShaderProgram(gVertexShaderSource, gFragmentShaderSource);
	}

	// Enable logic
	Loop();

	SDL_DestroyWindow(gGraphicsApplicationWindow);
	SDL_Quit();
	return 0;
}