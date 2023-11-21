#version 410 core

in vec3 v_vertexColors;
in vec3 v_normals;
in vec2 v_UV;

out vec4 color;

void main()
{
	vec3 norm = normalize(v_normals);
	vec3 lightDir = normalize(vec3(0, 2, 5));
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * v_vertexColors;

	//color = vec4(diffuse, 1.0);
	color = vec4(normalize(v_normals) * 0.5 + 0.5, 1.0);
}