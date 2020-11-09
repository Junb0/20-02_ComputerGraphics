#pragma once
#include<random>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd1(-50.0, 50.0);
std::uniform_real_distribution<float> urd2(5.0, 10.0);
std::uniform_real_distribution<float> urd3(20.0, 50.0);

class Snow {
public:
	float x = urd1(dre);
	float z = urd1(dre);
	float dy = urd2(dre);
	float y = urd3(dre);
};