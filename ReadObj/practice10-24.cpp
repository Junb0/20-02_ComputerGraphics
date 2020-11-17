#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "freeglut.lib")

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <GL/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ReadObj.h"
#include "Object.hpp"
#include "Snow.hpp"

using namespace std;

#define WINDOW_WITDH	1280
#define WINDOW_HEIGHT	800

#define CAMERA_POS			glm::vec3(0.0, 30.0, 100.0)
#define OBJECT_WHITE_COLOR	glm::vec3(1.0, 1.0, 1.0)
#define OBJECT_GRAY_COLOR	glm::vec3(0.5, 0.5, 0.5)
#define OBJECT_RED_COLOR	glm::vec3(1.0, 0.0, 0.0)
#define OBJECT_GREEN_COLOR	glm::vec3(0.0, 1.0, 0.0)
#define OBJECT_BLUE_COLOR	glm::vec3(0.0, 0.0, 1.0)
#define OBJECT_YELLOW_COLOR	glm::vec3(0.9, 0.7, 0.0)
#define LIGHT_AMBIENT		glm::vec3(0.1, 0.1, 0.1)
#define SUPER_AMBIENT		glm::vec3(0.5, 0.5, 0.5)

#define FILE_NAME		"objs/sphere.obj"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
void InitShader();

GLuint fragmentShader;
GLuint vertexShader;
GLuint s_program;

Object Sphere("objs/sphere.obj");
Object Pyramid("objs/pyramid.obj");
Object Cube("objs/cube2.obj");

float degree_cameraRotate = 0.0f;
float degree_lightPos = 0.0f;
bool draw_fill = true;

int light_type = 0;		// 0 : WHITE 1 : MAGENTA 2 : CYAN
int old_t;	// 델타타임 계산용
int isLightRotate;

glm::vec3 light_colors[4] = { glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 0.5, 1.0), glm::vec3(0.5, 1.0, 1.0) };
glm::vec3 light_pos;
glm::mat4 light_pos_mat(1.0f);

glm::mat4 Earth_rotate(1.0f);
glm::mat4 Mercury_rotate(1.0f);
glm::mat4 Venus_rotate(1.0f);

vector<Snow> snows(200);

bool snow_move = true;
bool isRotate = false;

GLuint VAO, VBO;

GLfloat Cube2[] = {	// 카메라 기준
	// 앞면
	-0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 0.0, 1.0,
	0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 1.0,
	-0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 1.0,
								  
	-0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 1.0,
	0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 1.0,
	0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 1.0, 1.0,
	// 오른쪽 면					      
	0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 0.0,
	0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 0.0,
								  
	0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 0.0,
	0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		1.0, 0.0, 0.0,
	// 뒷쪽 면						 
	0.5f, -0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 0.0, -1.0,
	-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0, 0.5,		0.0, 0.0, -1.0,
	0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 0.0, -1.0,
							
	0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 0.0, -1.0,
	-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0, 0.5,		0.0, 0.0, -1.0,
	-0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 0.0, -1.0,
	// 왼쪽 면				 
	-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0, 0.5,		-1.0, 0.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		-1.0, 0.0, 0.0,
	-0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		-1.0, 0.0, 0.0,
						
	-0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		-1.0, 0.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		-1.0, 0.0, 0.0,
	-0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		-1.0, 0.0, 0.0,
	// 위쪽 면			 
	-0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 0.0,
	0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 0.0,
	-0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 0.0,
							
	-0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 0.0,
	0.5f, 0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 0.0,
	0.5f, 0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, 1.0, 0.0,
	// 아랫 면					 
	-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0, 0.5,		0.0, -1.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, -1.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, -1.0, 0.0,
								
	-0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, -1.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, -1.0, 0.0,
	0.5f, -0.5f, 0.5f,		0.0, 0.0, 1.0, 0.5,		0.0, -1.0, 0.0
};

void variableInit() {
	light_pos = glm::vec3(0.0, 60.0, 0.0);
	light_pos_mat = glm::translate(light_pos_mat, light_pos);
	old_t = glutGet(GLUT_ELAPSED_TIME);
	isLightRotate = 0;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WITDH, WINDOW_HEIGHT);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	Sphere.InitObj();
	Pyramid.InitObj();
	Cube.InitObj();

	InitShader();
	InitBuffer();

	variableInit();

	glutTimerFunc(30, Timer, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(0.2, 0.2, 0.2, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glUseProgram(s_program);

	glm::mat4 modelTransform = glm::mat4(1.0f);

	glm::vec3 cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(degree_cameraRotate), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(CAMERA_POS, 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cameraPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), WINDOW_WITDH / (float)WINDOW_HEIGHT, 0.001f, 1000.f);

	GLuint modelTransformLocation = glGetUniformLocation(s_program, "g_modelTransform");

	GLuint viewLocation = glGetUniformLocation(s_program, "g_view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	GLuint projectLocation = glGetUniformLocation(s_program, "g_projection");
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glm::vec3 lightAmbient = LIGHT_AMBIENT;
	GLuint lightAmbientLocation = glGetUniformLocation(s_program, "g_lightAmbient");
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);

	glm::vec3 lightPos = light_pos;
	GLuint lightPosLocation = glGetUniformLocation(s_program, "g_lightPos");
	glUniform3fv(lightPosLocation, 1, (float*)&lightPos);

	glm::vec3 lightColor = light_colors[light_type];
	GLuint lightColorLocation = glGetUniformLocation(s_program, "g_lightColor");
	glUniform3fv(lightColorLocation, 1, (float*)&lightColor);

	glm::vec3 objColor = OBJECT_RED_COLOR;
	GLuint objColorLocation = glGetUniformLocation(s_program, "g_objectColor");
	glUniform3fv(objColorLocation, 1, (float*)&objColor);

	GLuint cameraPosLocation = glGetUniformLocation(s_program, "g_cameraPos");
	glUniform3fv(cameraPosLocation, 1, (float*)&cameraPos);

	GLuint vIsAlphaLocation = glGetUniformLocation(s_program, "vIsAlpha");
	glUniform1i(vIsAlphaLocation, 0);

	GLuint fIsAlphaLocation = glGetUniformLocation(s_program, "fIsAlpha");
	glUniform1i(fIsAlphaLocation, 0);

	// 바닥
	objColor = OBJECT_GRAY_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	glBindVertexArray(Cube.vao);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 0.1f, 50.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, Cube.indexCount, GL_UNSIGNED_INT, 0);

	// 피라미드
	objColor = OBJECT_YELLOW_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	glBindVertexArray(Pyramid.vao);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, Pyramid.indexCount, GL_UNSIGNED_INT, 0);

	// 눈
	objColor = OBJECT_WHITE_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	glBindVertexArray(Cube.vao);
	for (Snow s : snows) {
		modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(s.x, s.y, s.z)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
		glDrawElements(GL_TRIANGLES, Cube.indexCount, GL_UNSIGNED_INT, 0);
	}

	// 조명
	lightAmbient = SUPER_AMBIENT;
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);

	objColor = OBJECT_WHITE_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	glBindVertexArray(Sphere.vao);
	modelTransform = glm::translate(glm::mat4(1.0f), light_pos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, Sphere.indexCount, GL_UNSIGNED_INT, 0);

	// 반투명 큐브
	lightAmbient = LIGHT_AMBIENT;
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);
	glUniform1i(vIsAlphaLocation, 1);
	glUniform1i(fIsAlphaLocation, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(VAO);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 10.0f, 30.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 38.0f, 10.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 10.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 27.0f, 10.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 10.0f, 30.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 30.0f, 10.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 10.0f, -10.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 24.0f, 10.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 10.0f, 40.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 20.0f, 10.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisable(GL_BLEND);

	glutSwapBuffers();                        // 화면에 출력하기

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		degree_cameraRotate -= 20.f;
		break;
	case 'd':
		degree_cameraRotate += 20.f;
		break;
	case 'r':
		isLightRotate = 1;
		break;
	case 'R':
		isLightRotate = 0;
		break;
	case 'y':
		isRotate = true;
		break;
	case 'Y':
		isRotate = false;
		break;
	case 'c':
		light_type = (light_type + 1) % 3;
		break;
	case 's':
		snow_move = true;
		break;
	case 'S':
		snow_move = false;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fopen_s(&fptr, file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}


void make_vertexShaders()
{
	char* vertexsource = filetobuf("vertex10-24.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexsource, 0);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader error\n" << errorLog << endl;
	}
}

void make_fragmentShaders()
{
	char* fragmentsource = filetobuf("fragment10-24.glsl");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader error\n" << errorLog << endl;
	}
}

void InitBuffer()
{
	// 구 초기화
	glGenVertexArrays(1, &Sphere.vao);
	glBindVertexArray(Sphere.vao);

	glGenBuffers(3, Sphere.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Sphere.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, Sphere.vertexCount * sizeof(float) * 3, Sphere.vPosData, GL_STATIC_DRAW);
	int posLocation = glGetAttribLocation(s_program, "in_position");
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, Sphere.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, Sphere.vertexCount * sizeof(float) * 3, Sphere.vNormalData, GL_STATIC_DRAW);
	int normalLocation = glGetAttribLocation(s_program, "in_normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(normalLocation);

	glBindBuffer(GL_ARRAY_BUFFER, Sphere.vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, Sphere.vertexCount * sizeof(float) * 2, Sphere.vTextureCoordinateData, GL_STATIC_DRAW);
	int uvLocation = glGetAttribLocation(s_program, "in_uv");
	glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(uvLocation);

	glGenBuffers(1, &Sphere.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sphere.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Sphere.indexCount * sizeof(int), Sphere.indexData, GL_STATIC_DRAW);

	// 피라미드 초기화
	glGenVertexArrays(1, &Pyramid.vao);
	glBindVertexArray(Pyramid.vao);

	glGenBuffers(3, Pyramid.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Pyramid.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, Pyramid.vertexCount * sizeof(float) * 3, Pyramid.vPosData, GL_STATIC_DRAW);
	posLocation = glGetAttribLocation(s_program, "in_position");
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, Pyramid.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, Pyramid.vertexCount * sizeof(float) * 3, Pyramid.vNormalData, GL_STATIC_DRAW);
	normalLocation = glGetAttribLocation(s_program, "in_normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(normalLocation);

	glBindBuffer(GL_ARRAY_BUFFER, Pyramid.vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, Pyramid.vertexCount * sizeof(float) * 2, Pyramid.vTextureCoordinateData, GL_STATIC_DRAW);
	uvLocation = glGetAttribLocation(s_program, "in_uv");
	glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(uvLocation);

	glGenBuffers(1, &Pyramid.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Pyramid.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Pyramid.indexCount * sizeof(int), Pyramid.indexData, GL_STATIC_DRAW);

	// 큐브 초기화
	glGenVertexArrays(1, &Cube.vao);
	glBindVertexArray(Cube.vao);

	glGenBuffers(3, Cube.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Cube.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, Cube.vertexCount * sizeof(float) * 3, Cube.vPosData, GL_STATIC_DRAW);
	posLocation = glGetAttribLocation(s_program, "in_position");
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, Cube.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, Cube.vertexCount * sizeof(float) * 3, Cube.vNormalData, GL_STATIC_DRAW);
	normalLocation = glGetAttribLocation(s_program, "in_normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(normalLocation);

	glBindBuffer(GL_ARRAY_BUFFER, Cube.vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, Cube.vertexCount * sizeof(float) * 2, Cube.vTextureCoordinateData, GL_STATIC_DRAW);
	uvLocation = glGetAttribLocation(s_program, "in_uv");
	glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(uvLocation);

	glGenBuffers(1, &Cube.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Cube.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Cube.indexCount * sizeof(int), Cube.indexData, GL_STATIC_DRAW);

	// 반투명 큐브 초기화
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube2), Cube2, GL_STATIC_DRAW);
	posLocation = glGetAttribLocation(s_program, "in_position");
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	int colorLocation = glGetAttribLocation(s_program, "in_color");
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	normalLocation = glGetAttribLocation(s_program, "in_normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShaders();

	s_program = glCreateProgram();

	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	//checkCompileErrors(s_program, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(s_program);
}


GLvoid Timer(int value) {
	int t;
	float dt;

	t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - old_t) / 1000.0;
	old_t = t;
	if (isLightRotate == 1)
		light_pos = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f * dt), glm::vec3(0.0, 0.0, 1.0)) * glm::vec4(light_pos, 1.0);

	Mercury_rotate = glm::rotate(Mercury_rotate, glm::radians(30.0f * dt), glm::vec3(0.0, 1.0, 0.0));
	Venus_rotate = glm::rotate(Venus_rotate, glm::radians(-15.0f * dt), glm::vec3(0.0, 1.0, 0.0));
	Earth_rotate = glm::rotate(Earth_rotate, glm::radians(20.0f * dt), glm::vec3(0.0, 1.0, 0.0));

	if (snow_move) {
		for (Snow& s : snows) {
			s.y -= s.dy * dt;
			if (s.y <= 0.0f)
				s.y = 50.0f;
		}
	}

	if (isRotate) {
		degree_cameraRotate -= 50.0f * dt;
	}

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
