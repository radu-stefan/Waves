#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <include/glm.h>

class Sphere
{

public:
	glm::vec3 position;		//position in space
	glm::vec3 vel;			//velocity
	glm::vec3 force;		//total force applied to this object
	glm::vec3 scale;
	float mass;
	int count;

	void init();
	void collision();
	void static erase(int i);
	void static wave_update();
	void update();
	void Sphere::applyForce(glm::vec3 f);
	void Sphere::solve();

	void operate();

	static std::vector<glm::vec2> wave_center;
	static std::vector<float> amplitude;
	static std::vector<float> frequence;
	static std::vector<float> distance;
	static std::vector<float> phase;
	static std::vector<float> wavelength;
	static float dt;
	static float speed;

	Sphere(glm::vec3 pos);
	Sphere();
	~Sphere();
};

