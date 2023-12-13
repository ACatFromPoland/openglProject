#pragma once

struct t_terrainGeometry
{
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint terrainShader = 0;

	t_Vec3 chunkOriginPosition = {0.0f, 0.0f, 0.0f};

	t_CaveChunk chunk;

	~t_terrainGeometry()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteShader(terrainShader);
	}

} g_terrainGeometry;


void initTerrainGeometry()
{
	glGenVertexArrays(1, &g_terrainGeometry.VAO);
	glBindVertexArray(g_terrainGeometry.VAO);

	glGenBuffers(1, &g_terrainGeometry.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_terrainGeometry.VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_VertexStruct), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(t_VertexStruct), (void*)sizeof(t_Vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(t_VertexStruct), (void*)(sizeof(t_Vec3) * 2));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(t_VertexStruct), (void*)(sizeof(t_Vec3) * 2 + sizeof(t_Vec2)));

	glBindVertexArray(0);

	// Bind shaders
	std::string vertexShaderSource = LoadShaderAsString("shaders/vert.glsl");
	std::string fragmentShaderSource = LoadShaderAsString("shaders/frag.glsl");
	g_terrainGeometry.terrainShader = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void drawTerrain()
{
	gCameraMatrixs.model = glm::translate(gCameraMatrixs.model, glm::vec3(g_terrainGeometry.chunkOriginPosition.x, g_terrainGeometry.chunkOriginPosition.y, g_terrainGeometry.chunkOriginPosition.z));
	gCameraMatrixs.view = glm::lookAt(gCameraMatrixs.cameraPos, gCameraMatrixs.cameraPos + gCameraMatrixs.cameraFront, gCameraMatrixs.cameraUp);
	gCameraMatrixs.projection = glm::perspective(glm::radians(45.0f), (float)gScreenWidth / (float)gScreenHeight, 0.1f, 2000.0f);

	glUseProgram(g_terrainGeometry.terrainShader);

	GLint modelLoc = glGetUniformLocation(g_terrainGeometry.terrainShader, "model");
	GLint viewLoc = glGetUniformLocation(g_terrainGeometry.terrainShader, "view");
	GLint projLoc = glGetUniformLocation(g_terrainGeometry.terrainShader, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(gCameraMatrixs.projection));

	glBindVertexArray(g_terrainGeometry.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_terrainGeometry.VBO);
	glBufferData(GL_ARRAY_BUFFER, g_terrainGeometry.chunk.points.size() * sizeof(t_VertexStruct), g_terrainGeometry.chunk.points.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)g_terrainGeometry.chunk.points.size());
}