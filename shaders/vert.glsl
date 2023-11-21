#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;
layout(location=2) in vec3 normals;
layout(location=3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_vertexColors;
out vec3 v_normals;
out vec2 v_UV;

void main()
{
	v_vertexColors = vertexColors;
	v_UV = uv;
	v_normals = mat3(transpose(inverse(model))) * normals;
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0f);
}