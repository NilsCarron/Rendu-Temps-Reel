#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>
#include "stl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include "stl.h"

// Initialisation, inutile
static void error_callback(int /*error*/, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void APIENTRY opengl_error_callback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cout << message << std::endl;
}



struct TriangleWithNormal {
	glm::vec3 p0, n0, p1, n1, p2, n2;
};


//debut du code
int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(-1);
	}

	// Callbacks
	glDebugMessageCallback(opengl_error_callback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	// Shader
	// Recuperation des shaders, de couleur et emplacement
	const auto vertex = MakeShader(GL_VERTEX_SHADER, "resources/shaders/shader.vert");
	const auto fragment = MakeShader(GL_FRAGMENT_SHADER, "resources/shaders/shader.frag");
	//On lie Les deux elements qui compsoent le shader
	const auto program = AttachAndLink({ vertex, fragment });

	glUseProgram(program);


	// Buffers
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);



	//chargement de l'objet 3D
	const auto trisWithoutNormal = ReadStl("Kirby.stl");

	std::vector<TriangleWithNormal> tris;
	tris.reserve(trisWithoutNormal.size());
	for (int i = 0; i < trisWithoutNormal.size(); ++i) {
		auto& t = trisWithoutNormal[i];
		glm::vec3 n = glm::normalize(glm::cross(t.p0 - t.p1, t.p0 - t.p2));
		TriangleWithNormal triWithNormal{ t.p0 , n ,t.p1, n, t.p2, n };


			tris.push_back(triWithNormal);
	}

	const auto nTriangles = tris.size();


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nTriangles * sizeof(TriangleWithNormal), tris.data(), GL_STATIC_DRAW);

	// Bindings
	const auto index = glGetAttribLocation(program, "position");
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(index);

	 
	const auto indexnormal = glGetAttribLocation(program, "normal");
	glVertexAttribPointer(indexnormal, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (const void*) sizeof(glm::vec3));
	glEnableVertexAttribArray(indexnormal);




	unsigned int transformLoc = glGetUniformLocation(program, "trans");
	unsigned int transformColor = glGetUniformLocation(program, "colorPerTime"); // determine le nom de la reference, avec un emplacement memoire

	
	unsigned int albedoLoc = glGetUniformLocation(program, "albedo");
	unsigned int positionLightLoc = glGetUniformLocation(program, "positionLight");
	unsigned int LeLoc = glGetUniformLocation(program, "Le");

	const glm::vec3 positionLight{ 50,50,50 };
	const glm::vec3 albedo{ 0.5,0.5,0.5 };
	const glm::vec3 Le{ 10000,5000,10000 };


	//On va de 3 en 3 pour afficher l'element



	glEnableVertexAttribArray(index);

	// glPointSize(20.f);
	//
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{

		
		//taille de l ecran
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const glm::vec3 positionLight{ 50 * std::cos(glfwGetTime()),50 * std::sin(glfwGetTime()) ,50 };
		float time = sin((float)glfwGetTime());
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.5f)*sin(time));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
		trans = glm::scale(trans, glm::vec3(5, 5, 5));


		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniform4f(transformColor, 1, 0.5f, 1.0f, 1.0f); // envois vers shader.frag sous la reference  "colorPerTime"

		glUniform3fv(positionLightLoc, 1, glm::value_ptr(positionLight));
		glUniform3fv(LeLoc, 1, glm::value_ptr(Le));
		glUniform3fv(albedoLoc, 1, glm::value_ptr(albedo));




		glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
