#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <GL/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <cmath>
#define rndclr {urdColor(dre), urdColor(dre), urdColor(dre)}
#define PIE 3.1415926535f

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<> urdColor(0.0f, 1.0f);
std::uniform_real_distribution<> urdDegree(0.0f, 360.0f);

class Robot {
public:
	float x;
	float z;
	float Degree = 0.0f;
	float ArmDegree = 0.0f;
	float speed = 3.0f;
	float dd = 1.0f;
	glm::vec3 headColor;
	glm::vec3 bodyColor;
	glm::vec3 leftArmColor;
	glm::vec3 rightArmColor;
	glm::vec3 leftLegColor;
	glm::vec3 rightLegColor;
	glm::vec3 noseColor;
	bool isChase = false;

	Robot(float x, float z) {
		this->x = x;
		this->z = z;
		this->headColor = rndclr;
		this->bodyColor = headColor;
		this->leftArmColor = rndclr;
		this->rightArmColor = rndclr;
		this->leftLegColor = rndclr;
		this->rightLegColor = rndclr;
		this->noseColor = rndclr;
		Degree = urdDegree(dre);
	}

	void Draw(GLuint shaderID) {
		unsigned int transformLocation;
		unsigned int colorLocation;
		glm::mat4 transformMatrix(1.0f);
		transformLocation = glGetUniformLocation(shaderID, "g_modelTransform");
		colorLocation = glGetUniformLocation(shaderID, "color");
		// 머리 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.95f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(headColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 몸통 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.2f, 0.0f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(bodyColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 왼쪽 팔 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.2f, -0.6f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.35f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(ArmDegree), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.35f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.7f, 0.2f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(leftArmColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 오른쪽 팔 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.2f, 0.6f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.35f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-ArmDegree), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.35f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.7f, 0.2f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(rightArmColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 왼쪽 다리 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.35f, -0.25f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.35f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-ArmDegree), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.35f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.7f, 0.2f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(leftLegColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 오른쪽 다리 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.35f, 0.25f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.35f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(ArmDegree), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.35f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.7f, 0.2f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(rightLegColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 코 그리기
		transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::rotate(glm::mat4(1.0f), glm::radians(Degree), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 1.95f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glUniform3fv(colorLocation, 1, glm::value_ptr(noseColor));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Update(float dt, float px, float pz) {
		ArmDegree += dt * dd * 90.0f;
		if (ArmDegree > 45.0f && dd > 0.0f)
			dd = -1.0f;
		else if (ArmDegree < -45.0f && dd < 0.0f)
			dd = 1.0f;

		// 플레이어 추적 체크
		float distance = sqrtf(powf(px - x, 2) + powf(pz - z, 2));
		if (distance < 8.0f) {
			isChase = true;
		}
		else {
			if(isChase)
				Degree = urdDegree(dre);
			isChase = false;
		}

		// 추적중일때 방향전환
		if (isChase) {
			Degree = (atan2f(-pz + z, px - x) * 180.0f) / PIE;
		}

		// 이동

		x += cosf(glm::radians(Degree)) * dt * speed;
		z -= sinf(glm::radians(Degree)) * dt * speed;

		// 왼쪽벽 충돌
		if (x < -15.0f) {
			x = -15.0f;
			Degree += 180.0f;
		}
		// 오른쪽벽 충돌
		if (x > 15.0f) {
			x = 15.0f;
			Degree += 180.0f;
		}
		// 위쪽 벽 충돌
		if (z < -15.0f) {
			z = -15.0f;
			Degree += 180.0f;
		}
		// 아래쪽 벽 충돌
		if (z > 15.0f) {
			z = 15.0f;
			Degree += 180.0f;
		}
	}
};

class Player {
public:
	float x = 0.0f;
	float z = 0.0f;
	float degree = 90.0f;
	bool isThirdView = true;
};

void DrawGround(GLuint shaderID) {
	unsigned int transformLocation;
	unsigned int colorLocation;
	glm::mat4 transformMatrix(1.0f);
	transformLocation = glGetUniformLocation(shaderID, "g_modelTransform");
	colorLocation = glGetUniformLocation(shaderID, "color");

	transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 1.0f, 30.0f));
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
	glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.7f, 0.7f, 0.7f)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void DrawPlayer(GLuint shaderID, Player player) {
	unsigned int transformLocation;
	unsigned int colorLocation;
	glm::mat4 transformMatrix(1.0f);
	transformLocation = glGetUniformLocation(shaderID, "g_modelTransform");
	colorLocation = glGetUniformLocation(shaderID, "color");

	transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(player.x, 2.0f, player.z)) * glm::rotate(glm::mat4(1.0f), glm::radians(player.degree), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
	glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.9f)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}