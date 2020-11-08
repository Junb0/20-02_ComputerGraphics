#version 330 core

in vec3 out_position;
in vec3 out_normal;
out vec4 FragColor;

uniform vec3 g_lightAmbient;
uniform vec3 g_lightPos;
uniform vec3 g_lightColor;
uniform vec3 g_objectColor;
uniform vec3 g_cameraPos;

void main(){
	// Ambient
	vec3 ambient = g_lightAmbient * g_lightColor;

	// Diffuse
	vec3 normalVector = normalize(out_normal);
	vec3 lightDir = normalize(g_lightPos - out_position);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * g_lightColor;

	// Specular
	int shininess = 128;
	vec3 viewDir = normalize(g_cameraPos - out_position);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float specularLight = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularLight * g_lightColor;

	vec3 result = (diffuse + specular + ambient) * g_objectColor;

	FragColor = vec4(result, 1.0);
}