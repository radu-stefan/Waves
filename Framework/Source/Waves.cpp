#include "Waves.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

std::vector<Sphere*> Waves::objects;

Waves::Waves()
{
}

Waves::~Waves()
{
}

void Waves::Init()
{
	dropped = 0;
	drop_time = 0.0f;
	dt = 0.0f;
	sp = new Sphere();
	speed = 1;
	atenuare_timp = 3;
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;

	}

	{
		int n = 200;
		int m = 200;

		vector<VertexFormat> vertices;

		for (float r = 0; r < n; r++) {
			for (float c = 0; c < m; c++) {
				vertices.push_back(glm::vec3( r/20, 0, c/20));
			}
		}

		vector<unsigned short> indices;

		// Set up indices

		for (int r = 0; r < n - 1; r++) {
			for (int c = 0; c < m - 1; c++) {
				indices.push_back(r*m + c);
				indices.push_back(r*m + c + 1);
				indices.push_back((r + 1) * m + c);
			}
		}
		for (int r = 0; r < n - 1 ; r++) {
			for (int c = 0; c < m - 1; c++) {
				indices.push_back( r * m + c + 1);
				indices.push_back( (r + 1) * m + c + 1);
				indices.push_back( (r + 1) * m + c);
			}
		}


		CreateMesh("water", vertices, indices);
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderLab6");
		shader->AddShader("Source/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	

	light_angle_o = 90.0f;
	light_angle_v = 90.0f;
	m = glm::mat4(1.0f);

	//Light & material properties
	{
		lightPosition = glm::vec3(5, 2, 5);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}
}

Mesh* Waves::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}


void Waves::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Waves::Update(float deltaTimeSeconds)
{

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		RenderSimpleMesh(meshes["water"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0,0,1));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, sp->position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		RenderSimpleMesh(meshes["sphere"], shaders["Simple"], modelMatrix, glm::vec3(0, 0, 1));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, lightPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderSimpleMesh(meshes["sphere"], shaders["Simple"], modelMatrix, glm::vec3(1, 0, 0));	
	}

	Sphere::dt = dt;
	Sphere::wave_update();
	Sphere::speed = speed;
	for (int i = 0; i < objects.size() ; i++) {
		objects[i]->operate();

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, objects[i]->position);
		modelMatrix = glm::scale(modelMatrix, objects[i]->scale);
		RenderSimpleMesh(meshes["sphere"], shaders["Simple"], modelMatrix, glm::vec3(0,1,0));

		if (objects[i]->count == 5) {
			objects.erase(objects.begin() + i);
			i--;
		}
	}
}

void Waves::FrameEnd()
{
	DrawCoordinatSystem();
}

void Waves::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	//-----------LIGHTING-----------------------------------


	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);
	//------------------------------------------------

	//-----------WAVES-------------

	GLint amplitude = glGetUniformLocation(shader->program, "amplitude");
	glUniform1fv(amplitude, Sphere::amplitude.size(), reinterpret_cast<const GLfloat*>(Sphere::amplitude.data()));

	GLint wavelength = glGetUniformLocation(shader->program, "wavelength");
	glUniform1fv(wavelength, Sphere::wavelength.size(), reinterpret_cast<const GLfloat*>(Sphere::wavelength.data()));

	GLint phase = glGetUniformLocation(shader->program, "phase");
	glUniform1fv(phase, Sphere::phase.size(), reinterpret_cast<const GLfloat*>(Sphere::phase.data()));

	GLint frequence = glGetUniformLocation(shader->program, "frequence");
	glUniform1fv(frequence, Sphere::frequence.size(), reinterpret_cast<const GLfloat*>(Sphere::frequence.data()));

	GLint distance = glGetUniformLocation(shader->program, "distance");
	glUniform1fv(distance, Sphere::distance.size(), reinterpret_cast<const GLfloat*>(Sphere::distance.data()));

	GLint center = glGetUniformLocation(shader->program, "wave_center");
	glUniform2fv(center, Sphere::wave_center.size(), reinterpret_cast<const GLfloat*>(Sphere::wave_center.data()) );
	GLint wave_n = glGetUniformLocation(shader->program, "wave_num");
	glUniform1i(wave_n, Sphere::wave_center.size());

	//--------------------------------------

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Waves::OnInputUpdate(float deltaTime, int mods)
{	
	
	dt = deltaTime;
	float s = 5;
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		if (window->KeyHold(GLFW_KEY_W)) {
			sp->position.x += s * deltaTime;
		}
		if (window->KeyHold(GLFW_KEY_S)) {
			sp->position.x -= s * deltaTime;
		}
		if (window->KeyHold(GLFW_KEY_A)) {
			sp->position.z -= s * deltaTime;
		}
		if (window->KeyHold(GLFW_KEY_D)) {
			sp->position.z += s * deltaTime;
		}
	}

	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

		// Control light position using on W, A, S, D, E, Q
		if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * s;
		if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * s;
		if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * s;
		if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * s;
		if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * s;
		if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * s;

		m = glm::mat4(1.0f);
		m = glm::rotate(m, RADIANS(light_angle_v), glm::vec3(1, 0, 0));
		m = glm::rotate(m, RADIANS(light_angle_o), glm::vec3(0, 1, 0));

		lightDirection = glm::vec3(m*glm::vec4(glm::vec3(0, 1, 0), 1));

	}
}

void Waves::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) {
		objects.push_back(new Sphere(sp->position));
	}
	if (key == GLFW_KEY_LEFT) {
		speed -= 0.1f;
	}
	if (key == GLFW_KEY_RIGHT) {
		speed += 0.1f;
	}
}

void Waves::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Waves::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Waves::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Waves::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Waves::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Waves::OnWindowResize(int width, int height)
{
}
