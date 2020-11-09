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

using namespace std;

#define WINDOW_WITDH	1280
#define WINDOW_HEIGHT	800

#define OBJECT_WHITE_COLOR	glm::vec3(1.0, 1.0, 1.0)
#define OBJECT_RED_COLOR	glm::vec3(1.0, 0.0, 0.0)
#define OBJECT_GREEN_COLOR	glm::vec3(0.0, 1.0, 0.0)
#define OBJECT_BLUE_COLOR	glm::vec3(0.0, 0.0, 1.0)
#define OBJECT_BROWN_COLOR	glm::vec3(0.5, 0.35, 0.05)
#define LIGHT_AMBIENT		glm::vec3(0.1, 0.1, 0.1)
#define SUPER_AMBIENT		glm::vec3(0.5, 0.5, 0.5)

#define FILE_NAME		"objs/cube2.obj"

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

GLuint vao;
GLuint vbo[3];
GLuint ebo;

float* vPosData;
float* vNormalData;
float* vTextureCoordinateData;
int* indexData;
int vertexCount;
int indexCount;
int isLightRotate = 0;

float degree_cameraRotate = 0.0f;
float degree_lightPos = 0.0f;
bool draw_fill = true;

int light_type = 0;		// 0 : WHITE 1 : MAGENTA 2 : CYAN

int machine_direction;
float machine_x;
float machine_z;

GLint is_light_on = true;
float lightOn;

glm::vec3 light_colors[4] = { glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 0.5, 1.0), glm::vec3(0.5, 1.0, 1.0) };
glm::vec3 light_pos;
glm::vec3 camera_pos;
glm::mat4 view_rotate_mat(1.0f);
glm::mat4 light_pos_mat(1.0f);
glm::mat4 machine_translate(1.0f);

int old_t;

void variableInit() {
	light_pos = glm::vec3(0.0, 2.0, 8.0);
	light_pos_mat = glm::translate(light_pos_mat, light_pos);
	camera_pos = glm::vec3(0.0, 3.0, 8.0);
	machine_direction = 1;
	machine_x = 0.0;
	machine_z = 0.0;
	lightOn = 1.0f;
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

	ReadObj(FILE_NAME, vPosData, vNormalData, vTextureCoordinateData, indexData, vertexCount, indexCount);
	InitShader();
	InitBuffer();

	variableInit();

	old_t = glutGet(GLUT_ELAPSED_TIME);

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

	glm::vec3 cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(degree_cameraRotate), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_pos, 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cameraPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0), cameraPos) * view_rotate_mat * glm::translate(glm::mat4(1.0), -cameraPos);
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

	glm::vec3 lightColor = light_colors[light_type] * lightOn;
	GLuint lightColorLocation = glGetUniformLocation(s_program, "g_lightColor");
	glUniform3fv(lightColorLocation, 1, (float*)&lightColor);

	glm::vec3 objColor = OBJECT_BROWN_COLOR;
	GLuint objColorLocation = glGetUniformLocation(s_program, "g_objectColor");
	glUniform3fv(objColorLocation, 1, (float*)&objColor);

	GLuint cameraPosLocation = glGetUniformLocation(s_program, "g_cameraPos");
	glUniform3fv(cameraPosLocation, 1, (float*)&cameraPos);

	GLuint LightOnLocation = glGetUniformLocation(s_program, "lightOn");
	glUniform1iv(LightOnLocation, 1, (GLint*)&is_light_on);

	glBindVertexArray(vao);
	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.2, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 1.0f, 5.0f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	objColor = OBJECT_RED_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	modelTransform = machine_translate * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.2f, 0.5f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	objColor = OBJECT_GREEN_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	modelTransform = machine_translate * glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.4, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.2f, 0.3f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	objColor = OBJECT_BLUE_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	modelTransform = machine_translate * glm::translate(glm::mat4(1.0f), glm::vec3(-0.2, 0.8, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.2f, 0.05f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	modelTransform = machine_translate * glm::translate(glm::mat4(1.0f), glm::vec3(0.2, 0.8, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.2f, 0.05f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	lightAmbient = SUPER_AMBIENT;
	glUniform3fv(lightAmbientLocation, 1, (float*)&lightAmbient);

	objColor = OBJECT_WHITE_COLOR;
	glUniform3fv(objColorLocation, 1, (float*)&objColor);
	modelTransform = glm::translate(glm::mat4(1.0f), light_pos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

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
		isLightRotate = 2;
		break;
	case 'e':
		view_rotate_mat = glm::rotate(view_rotate_mat, glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
		break;
	case 'E':
		view_rotate_mat = glm::rotate(view_rotate_mat, glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
		break;
	case 'z':
		camera_pos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -1.0)) * glm::vec4(camera_pos, 1.0);
		break;
	case 'Z':
		camera_pos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 1.0)) * glm::vec4(camera_pos, 1.0);
		break;
	case 'x':
		camera_pos = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0.0, 0.0)) * glm::vec4(camera_pos, 1.0);
		break;
	case 'X':
		camera_pos = glm::translate(glm::mat4(1.0f), glm::vec3(1.0, 0.0, 0.0)) * glm::vec4(camera_pos, 1.0);
		break;
	case 'c':
		light_type = (light_type + 1) % 3;
		break;
	case 'm':
	case 'M':
		if (is_light_on == true) {
			lightOn = 0.0f;
			is_light_on = false;
		}
		else {
			lightOn = 1.0f;
			is_light_on = true;
		}
		break;
	case 's':
	case 'S':
		isLightRotate = 0;
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
	char* vertexsource = filetobuf("vertex.glsl");

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
	char* fragmentsource = filetobuf("fragment.glsl");

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
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, vPosData, GL_STATIC_DRAW);
	int posLocation = glGetAttribLocation(s_program, "in_position");
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 3, vNormalData, GL_STATIC_DRAW);
	int normalLocation = glGetAttribLocation(s_program, "in_normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(normalLocation);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float) * 2, vTextureCoordinateData, GL_STATIC_DRAW);
	int uvLocation = glGetAttribLocation(s_program, "in_uv");
	glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(uvLocation);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indexData, GL_STATIC_DRAW);
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
		light_pos = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f * dt), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(light_pos, 1.0);
	if (isLightRotate == 2)
		light_pos = glm::rotate(glm::mat4(1.0f), glm::radians(-50.0f * dt), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(light_pos, 1.0);

	if (machine_direction == 1) {
		machine_translate = glm::translate(machine_translate, glm::vec3(2.0 * dt, 0.0, 0.0));
		machine_x += 2.0 * dt;
		if (machine_x >= 4.5)
			machine_direction = 2;
	}
	if (machine_direction == 2) {
		machine_translate = glm::translate(machine_translate, glm::vec3(-2.0 * dt, 0.0, 0.0));
		machine_x += -2.0 * dt;
		if (machine_x <= -4.5)
			machine_direction = 1;
	}

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
