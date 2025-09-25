// task2_part1.cpp
#include <iostream>
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main() {
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

GLuint compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}

GLuint createProgram(const char* vsSource, const char* fsSource) {
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(500, 500, "Assignment 1", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 500, 500);
	GLuint shaderProgram = createProgram(vertexShaderSource, fragmentShaderSource);

	const int segments = 120;

	std::vector<float> ellipse;
	float ellipse_cx = -0.6f, ellipse_cy = 0.5f;
	float ellipse_rx = 0.25f;
	float ellipse_ry = ellipse_rx * 0.6f;
	ellipse.insert(ellipse.end(), { ellipse_cx, ellipse_cy, 0.0f, 1.0f, 0.0f, 0.0f });
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * M_PI * i / segments;
		float x = ellipse_cx + ellipse_rx * cos(angle);
		float y = ellipse_cy + ellipse_ry * sin(angle);
		float red = 1.0f;
		ellipse.insert(ellipse.end(), { x, y, 0.0f, red, 0.0f, 0.0f });
	}

	GLuint VAO_ellipse, VBO_ellipse;
	glGenVertexArrays(1, &VAO_ellipse);
	glGenBuffers(1, &VBO_ellipse);
	glBindVertexArray(VAO_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ellipse);
	glBufferData(GL_ARRAY_BUFFER, ellipse.size() * sizeof(float), ellipse.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	std::vector<float> triangle;
	float triR = 0.25f;
	float tri_cx = 0.0f, tri_cy = 0.5f;
	float tri_angles[3] = { M_PI / 2.0f, 7.0f * M_PI / 6.0f, 11.0f * M_PI / 6.0f };
	float colors[3][3] = { {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} };

	for (int i = 0; i < 3; ++i) {
		float x = tri_cx + triR * cos(tri_angles[i]);
		float y = tri_cy + triR * sin(tri_angles[i]);
		triangle.insert(triangle.end(), { x, y, 0.0f, colors[i][0], colors[i][1], colors[i][2] });
	}

	GLuint VAO_triangle, VBO_triangle;
	glGenVertexArrays(1, &VAO_triangle);
	glGenBuffers(1, &VBO_triangle);
	glBindVertexArray(VAO_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle);
	glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(float), triangle.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	std::vector<float> circle;
	float circle_cx = 0.6f, circle_cy = 0.5f;
	float circle_r = 0.25f;
	circle.insert(circle.end(), { circle_cx, circle_cy, 0.0f, 0.0f, 0.0f, 0.0f });
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * M_PI * i / segments;
		float x = circle_cx + circle_r * cos(angle);
		float y = circle_cy + circle_r * sin(angle);
		float red = (cos(angle) + 1.0f) / 2.0f;
		circle.insert(circle.end(), { x, y, 0.0f, red, 0.0f, 0.0f });
	}

	GLuint VAO_circle, VBO_circle;
	glGenVertexArrays(1, &VAO_circle);
	glGenBuffers(1, &VBO_circle);
	glBindVertexArray(VAO_circle);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_circle);
	glBufferData(GL_ARRAY_BUFFER, circle.size() * sizeof(float), circle.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	std::vector<float> squares;
	const int numLayers = 8;
	float sq_cx = 0.0f, sq_cy = -0.4f;
	for (int layer = 0; layer < numLayers; ++layer) {
		float size = 0.45f * (1.0f - (float)layer / numLayers);
		float gray = (layer % 2 == 0) ? 1.0f : 0.0f;

		float sq_angles[4] = { M_PI / 4, 3 * M_PI / 4, 5 * M_PI / 4, 7 * M_PI / 4 };
		float corners[4][2];
		for (int i = 0; i < 4; ++i) {
			corners[i][0] = sq_cx + size * cos(sq_angles[i]);
			corners[i][1] = sq_cy + size * sin(sq_angles[i]);
		}
		int indices[] = { 0, 1, 2, 0, 2, 3 };
		for (int i : indices) {
			squares.insert(squares.end(), { corners[i][0], corners[i][1], 0.0f, gray, gray, gray });
		}
	}

	GLuint VAO_squares, VBO_squares;
	glGenVertexArrays(1, &VAO_squares);
	glGenBuffers(1, &VBO_squares);
	glBindVertexArray(VAO_squares);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_squares);
	glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO_ellipse);
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

		glBindVertexArray(VAO_triangle);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(VAO_circle);
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

		glBindVertexArray(VAO_squares);
		glDrawArrays(GL_TRIANGLES, 0, numLayers * 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO_circle);
	glDeleteBuffers(1, &VBO_circle);
	glDeleteVertexArrays(1, &VAO_ellipse);
	glDeleteBuffers(1, &VBO_ellipse);
	glDeleteVertexArrays(1, &VAO_squares);
	glDeleteBuffers(1, &VBO_squares);
	glDeleteVertexArrays(1, &VAO_triangle);
	glDeleteBuffers(1, &VBO_triangle);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}