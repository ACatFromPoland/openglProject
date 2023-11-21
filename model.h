#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct t_Vec3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};

struct t_Vec2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		float v[2];
	};
};

struct t_VertexStruct
{
	t_Vec3 position;
	t_Vec3 color{ 1.0f, 1.0f, 1.0f };
	t_Vec3 normal;
	t_Vec2 uv;
};

struct t_Model
{
	std::string name;
	std::vector<t_VertexStruct> vertices;
};

void loadModel(t_Model& model, std::string path)
{
	int n = 0;
	std::vector<t_Vec3> vertexPositions;
	std::vector<t_Vec3> vertexColors;
	std::vector<t_Vec3> vertexNormals;
	std::vector<t_Vec2> texturePositions;

	std::ifstream infile(path);
	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::string line;
	const char* _delimiter = " \n\r\t";
	while (std::getline(infile, line))
	{
		char* tokenBuffer = nullptr;
		std::string type = strtok_s(line.data(), _delimiter, &tokenBuffer);

		if (type == "o")
		{
			model.name = strtok_s(nullptr, _delimiter, &tokenBuffer);
			strtok_s(line.data(), _delimiter, &tokenBuffer);
		}
		//else if (type == "s")
		else if (type == "v")
		{
			t_Vec3& vertexPosition = vertexPositions.emplace_back();
			for (char i = 0; i < 3; i++)
			{
				vertexPosition.v[i] = (float)std::atof(strtok_s(nullptr, _delimiter, &tokenBuffer));
			}

			vertexColors.push_back({ 1.0f, 1.0f, 1.0f });
		}
		else if (type == "vn")
		{
			t_Vec3& vertexNormal = vertexNormals.emplace_back();
			for (char i = 0; i < 3; i++)
			{
				vertexNormal.v[i] = (float)std::atof(strtok_s(nullptr, _delimiter, &tokenBuffer));
			}
		}
		else if (type == "vt")
		{
			t_Vec2& texturePosition = texturePositions.emplace_back();
			for (char i = 0; i < 2; i++)
			{
				texturePosition.v[i] = (float)std::atof(strtok_s(nullptr, _delimiter, &tokenBuffer));
			}
		}
		else if (type == "f")
		{
			for (int i = 0; i < 3; i++)
			{
				std::string data = strtok_s(nullptr, _delimiter, &tokenBuffer);
				const char* float_decimator = "/";
				char* floatData = nullptr;

				t_VertexStruct& vertex = model.vertices.emplace_back();
				vertex.position = vertexPositions[std::atoi(strtok_s(data.data(), float_decimator, &floatData))-1];
				vertex.uv = texturePositions[std::atoi(strtok_s(nullptr, float_decimator, &floatData)) - 1];
				vertex.normal = vertexNormals[std::atoi(strtok_s(nullptr, float_decimator, &floatData))-1];
			}
		}
	}
}