
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Shader.h"

#include <iostream>
#include <vector>
#include <thread>

using namespace std;
using namespace glm;

float width = 800;
float height = 600;

Shader* tempShader;
mat4 proj;
void window_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
	glfwSetWindowSize(window, w, h);
	width = w;
	height = h;

	proj = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
	tempShader->setUniform4m("u_proj", proj);
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	vector<float> figureA;

	float radius = 1;

	for (unsigned int i = 0; i < 6; i++) {
		float angle = 60 * i;

		float x = cos(radians(angle)) * radius;
		float y = sin(radians(angle)) * radius;
		cout << "x cord: " << x << "y coord: " << y << endl;
		figureA.push_back(x);
		figureA.push_back(y);

	}
	figureA.push_back(0.0f);
	figureA.push_back(0.0f);

	vector<unsigned> indices{
		0,5,
		5,4,
		4,3,
		3,2,
		2,1,
		1,0,

		0,6,
		5,6,
		4,6,
		3,6,
		2,6,
		1,6
	};

	vector<float> figureB {
		0.0f,0.0f,
		-1.0f,1.0f,
		-6.0f,3.0f,
		-6.0f,1.0f,
		-6.0f,-1.0f,
		-6.0f,-3.0f,
		-1.0f,-1.0f,
	};

	for (auto& fl : figureB) {
		fl /= 8;
	}

	vector<unsigned> indicesB{
		0,6,
		6,5,
		5,4,
		4,6,
		4,3,
		3,2,
		3,1,
		2,1,
		1,0
	};

	unsigned vaoA;
	glGenVertexArrays(1, &vaoA);
	glBindVertexArray(vaoA);

	unsigned vboA;
	glGenBuffers(1, &vboA);
	glBindBuffer(GL_ARRAY_BUFFER, vboA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*figureA.size(), &figureA[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, 0, 0);

	unsigned ibA;
	glGenBuffers(1, &ibA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibA);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);


	unsigned vaoB;
	glGenVertexArrays(1, &vaoB);
	glBindVertexArray(vaoB);

	unsigned vboB;
	glGenBuffers(1, &vboB);
	glBindBuffer(GL_ARRAY_BUFFER, vboB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*figureB.size(), &figureB[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	unsigned ibB;
	glGenBuffers(1, &ibB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*indicesB.size(), &indicesB[0], GL_STATIC_DRAW);


	Shader basic("shaders/basic.shader");
	tempShader = &basic;
	proj = perspective(radians(45.0f), width / height, 0.1f, 100.0f);
	mat4 model(1.0f);
	
	model = translate(model, vec3(0, 0, -4));

	basic.setUniform4m("u_proj", proj);
	basic.setUniform4m("u_model", model);

	vec3 posA(0.0f);
	vec3 posB(0.0f);
	float rotationRadius = 1;
	float speed = 40;
	float scaleA = 0.4f;
	float scaleB = 0.4f;
	
	//Setup IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


	bool inCycle = false;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Text("Figure A settup");
		ImGui::SliderFloat("rotation\nradius", &rotationRadius, 0.1f, 2.5f);
		ImGui::SliderFloat("rotation\nspeed", &speed, 10.0f, 100.0f);
		ImGui::SliderFloat("figureA\nscale", &scaleA, 0.05f, 0.9f);
		ImGui::SliderFloat("translate X\ figure A", &posA.x, -1.0f, 1.0f);
		ImGui::SliderFloat("translate Y\ figure A", &posA.y, -1.0f, 1.0f);

		ImGui::Separator();
		ImGui::Text("Figure B settup");
		ImGui::SliderFloat("figureB\nscale", &scaleB, 0.05f, 2.0f);
		ImGui::SliderFloat("translate X\n figureB", &posB.x, -1.0f, 1.0f);
		ImGui::SliderFloat("translate Y\n figureB", &posB.y, -1.0f, 1.0f);


		string text = "press\nto enter cycle";
		if (inCycle) {
			text = "press\nto exit cycle";
		}

		if (ImGui::Button(text.c_str())) {
			inCycle = !inCycle;
		}



		glBindVertexArray(vaoB);
		mat4 mod(1.0f);
		mod = translate(mod, vec3(posB.x, posB.y, -4));
		
		mod = scale(mod, vec3(scaleB));
		basic.setUniform4m("u_model", mod);
		basic.bind();
		glDrawElements(GL_LINES, indicesB.size(), GL_UNSIGNED_INT, 0);
		


		glBindVertexArray(vaoA);
		if (!inCycle) {
			mat4 mod(1.0f);

			mod = translate(mod, vec3(posA.x, posA.y, -4));
			float x = cos(radians(glfwGetTime())* speed) * rotationRadius;
			float y = sin(radians(glfwGetTime())* speed) * rotationRadius;

			mod = translate(mod, vec3(x, y, 0));
			mod = scale(mod, vec3(scaleA));
			basic.setUniform4m("u_model", mod);

			basic.bind();
			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		else {
			for (int i = 0; i < 180; i++) {
				mat4 mod(1.0f);

				mod = translate(mod, vec3(0, 0, -4));
				float x = cos(radians(i * 2.0f)) * rotationRadius;
				float y = sin(radians(i * 2.0f)) * rotationRadius;

				mod = translate(mod, vec3(x, y, 0));
				mod = scale(mod, vec3(scaleA));
				basic.setUniform4m("u_model", mod);
				
				
				basic.bind();
				glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

			}
		
		}
		
		


		//Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}