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
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}

std::vector<float> makeCircle(float cx, float cy, float r, int segments, float cr, float cg, float cb) {
	std::vector<float> vertices;
	vertices.insert(vertices.end(), { cx, cy, 0.0f, cr, cg, cb });
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * M_PI * i / segments;
		float x = cx + r * cos(angle);
		float y = cy + r * sin(angle);
		vertices.insert(vertices.end(), { x, y, 0.0f, cr, cg, cb });
	}
	return vertices;
}

std::vector<float> makeEllipse(float cx, float cy, float rx, float ry, int segments, float cr, float cg, float cb) {
	std::vector<float> vertices;
	vertices.insert(vertices.end(), { cx, cy, 0.0f, cr, cg, cb });
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * M_PI * i / segments;
		float x = cx + rx * cos(angle);
		float y = cy + ry * sin(angle);
		vertices.insert(vertices.end(), { x, y, 0.0f, cr, cg, cb });
	}
	return vertices;
}

std::vector<float> makeTriangle(float x1, float y1, float x2, float y2, float x3, float y3,
	float r, float g, float b) {
	return {
		x1, y1, 0.0f, r, g, b,
		x2, y2, 0.0f, r, g, b,
		x3, y3, 0.0f, r, g, b
	};
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(500, 500, "Task 2 Part 2 - Flower", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL()) return -1;

	glViewport(0, 0, 500, 500);
	GLuint shaderProgram = createProgram(vertexShaderSource, fragmentShaderSource);

	const int segments = 60;

	// Flower center
	std::vector<float> center = makeCircle(0.0f, 0.2f, 0.1f, segments, 1.0f, 1.0f, 0.0f);

	// Petals
	std::vector<std::vector<float>> petals;
	for (int i = 0; i < 6; i++) {
		float angle = i * (2 * M_PI / 6);
		float px = 0.2f * cos(angle);
		float py = 0.2f * sin(angle) + 0.2f;
		petals.push_back(makeEllipse(px, py, 0.15f, 0.07f, segments, 1.0f, 0.0f, 1.0f));
	}

	// Grass
	std::vector<std::vector<float>> grass;
	for (int i = -4; i <= 4; i++) {
		float gx = i * 0.2f;
		grass.push_back(makeTriangle(gx - 0.05f, -1.0f, gx + 0.05f, -1.0f, gx, -0.7f,
			0.0f, 0.8f, 0.0f));
	}

	auto setupVAO = [](const std::vector<float>& data) {
		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		return std::make_pair(VAO, (GLsizei)(data.size() / 6));
		};

	auto centerVAO = setupVAO(center);
	std::vector<std::pair<GLuint, GLsizei>> petalVAOs;
	for (auto& p : petals) petalVAOs.push_back(setupVAO(p));
	std::vector<std::pair<GLuint, GLsizei>> grassVAOs;
	for (auto& g : grass) grassVAOs.push_back(setupVAO(g));

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(centerVAO.first);
		glDrawArrays(GL_TRIANGLE_FAN, 0, centerVAO.second);

		for (auto& vao : petalVAOs) {
			glBindVertexArray(vao.first);
			glDrawArrays(GL_TRIANGLE_FAN, 0, vao.second);
		}

		for (auto& vao : grassVAOs) {
			glBindVertexArray(vao.first);
			glDrawArrays(GL_TRIANGLES, 0, vao.second);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
