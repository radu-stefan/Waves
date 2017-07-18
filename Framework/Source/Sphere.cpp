#include "Sphere.h"

std::vector<glm::vec2> Sphere::wave_center;
std::vector<float> Sphere::amplitude;
std::vector<float> Sphere::frequence;
std::vector<float> Sphere::distance;
std::vector<float> Sphere::wavelength;
std::vector<float> Sphere::phase;
float Sphere::speed;
float Sphere::dt;

Sphere::Sphere(glm::vec3 pos)
{
	position = pos;
	vel = glm::vec3(0);
	mass = 1.0f;
	scale = glm::vec3(1.0f);
	count = 0;
}

Sphere::Sphere() {
	position = glm::vec3(5.0f,3.0f,5.0f);
}

void Sphere::init() {
	force = glm::vec3(0.0f);
}

void Sphere::collision() {
	if (this->position.y < 0) {
		wavelength.push_back(this->scale.x*0.8);
		wave_center.push_back(glm::vec2(this->position.x, this->position.z));
		amplitude.push_back(this->scale.x*0.4);
		frequence.push_back( 2*glm::pi<float>()/wavelength.back() );
		distance.push_back(0);
		phase.push_back(0);
		
		this->count++;
		this->scale /= 2;
		this->vel *= -0.9;
		this->position.y = 0;
	}
}

void Sphere::erase(int i) {
	amplitude.erase(amplitude.begin() + i);
	wave_center.erase(wave_center.begin() + i);
	frequence.erase(frequence.begin() + i);
	wavelength.erase(wavelength.begin() + i);
	distance.erase(distance.begin() + i);
	phase.erase(phase.begin() + i);
}

void Sphere::wave_update() {
	for (int i = 0; i < amplitude.size(); i++) {
		amplitude[i] -= 0.1 * dt * speed;
		distance[i] += speed * dt;
		phase[i] += speed*dt*2*glm::pi<float>()/wavelength[i];
		if (amplitude[i] <= 0) {
			erase(i);
			i--;
		}
	}
}

void Sphere::update() {
	vel += (force / mass) * dt * speed;
	position += vel * dt * speed;
	mass = scale.x;
}

void Sphere::applyForce(glm::vec3 f) {
	force += f;
}

void Sphere::solve() {
	applyForce(glm::vec3( 0, -10.0f, 0)*mass);
}

void Sphere::operate() {
	init();
	solve();
	update();
	collision();
}


Sphere::~Sphere()
{
}
