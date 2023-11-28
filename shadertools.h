#pragma once

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
