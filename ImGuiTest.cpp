#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <chrono>
#include <iostream>
#include <vector>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shaderClass.h"

int start = 2;
int iterationen = 0;
double timeDiff;
float xOffset = 0, yOffset = 0;
double elapsedTime_ms = 0;

struct Vertex 
{
	GLfloat x;
	GLfloat y;
};

void handleInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		yOffset -= 0.1f * timeDiff;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		yOffset += 0.1f * timeDiff;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		xOffset += 0.1f * timeDiff;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		xOffset -= 0.1f * timeDiff;
	}
}

void windowSizeCallback(GLFWwindow *window, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
}

std::vector<Vertex> calculate(int maxIterations) 
{
	std::vector<Vertex> vertices;
	auto timeStart = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < maxIterations; i++)
	{
		int buffer = start;
		if (start % 2 == 0)
		{
			buffer = start / 2;
		}
		else if (start % 2 == 1)
		{
			buffer = 3 * start + 1;
		}
		if (start == 1)
		{
			iterationen = i;
			break;
		}
		vertices.push_back(Vertex());
		vertices[i].x = i * 0.04f;
		vertices[i].y = buffer * 0.001f;
		start = buffer;
		std::cout << i << std::endl;
		//std::cout << vertices[i].y << std::endl;
	}
	auto timeEnd = std::chrono::high_resolution_clock::now();
	elapsedTime_ms = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	return vertices;
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Collatz-Problem", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	gladLoadGL();
	
	glViewport(0, 0, width, height);

	glfwSetWindowSizeCallback(window, windowSizeCallback);

	Shader shaderProgram("standard.vert", "standard.frag");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	float xScale = 1.0f, yScale = 1.0f;
	float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<Vertex> newVertices = calculate(1999);
	glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(Vertex), &newVertices.front(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glfwSwapInterval(0);

	double prevTime = 0.0;
	double crntTime = 0.0;
	unsigned int counter = 0;

	//Haupt-Loop
	while (!glfwWindowShouldClose(window))
	{
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0)
		{
			prevTime = crntTime;
			counter = 0;
		}

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Neues Backbuffer
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		shaderProgram.Activate();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, iterationen);
		
		ImGui::Begin("Einstellungen");

		ImGui::Text("Ich hoffe mal es stuerzt nicht ab.");

		ImGui::InputInt("Start", &start);

		ImGui::SliderFloat("X-Achsenskalierung", &xScale, 0.01f, 10.0f);
		ImGui::SliderFloat("Y-Achsenskalierung", &yScale, 0.01f, 10.0f);

		ImGui::ColorEdit4("Farbe", color);

		if (ImGui::Button("Berechnen"))
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			std::vector<Vertex> newVertices = calculate(1999);
			
			glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(Vertex), &newVertices.front(), GL_DYNAMIC_DRAW);
		}
	
		ImGui::BeginChild("Scrolling", ImVec2(0, 0), true);
		ImGui::Text("Iterationen: %d", iterationen);
		ImGui::Text("Berechnungsdauer: %fms", elapsedTime_ms);
		ImGui::EndChild();
		
		ImGui::End();


		handleInput(window);

		// Uniforms
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "xScale"), xScale);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "yScale"), yScale);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "xOffset"), xOffset);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "yOffset"), yOffset);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), color[0], color[1], color[2], color[3]);

		//Render UI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Backbuffer mit Frontbuffer tauschen
		glfwSwapBuffers(window);
	
		glfwPollEvents();
	}

	/// <summary>
	/// Aufräumen
	/// </summary>
	/// <returns></returns>
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shaderProgram.Delete();
	
	glfwDestroyWindow(window);
	
	glfwTerminate();
	return 0;
}