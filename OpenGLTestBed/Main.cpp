#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"
#include <iostream>
#include <string>
#include <assimp\Importer.hpp>
#include "Model.h"
#include "Mesh.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int loadTexture(char const * path, bool);


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		std::cout << "Error: %s\n" << glewGetErrorString(err);
	}
	else
	{
		std::cout << " Glew initialsed" << std::endl;
	}
	Shader containerShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\basic.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\basic.fs");
	Shader lightShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\lightBasic.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\lightBasic.fs");
	Shader transparentShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\transparentBasic.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\transparentBasic.fs");

	Model ourModel("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\nanosuit2.obj");
	Model podium("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\podium.obj");
	Model houseModel("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\houseModels.obj");


	//---transparent quad 
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//---------------------Quad With Texture
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);

	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glBindVertexArray(0);
	//------------

	unsigned int diffuseMap = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\cartoonyCrate.jpg", false);
	unsigned int specularMap = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\cartoonyCrate_Specular.jpg", false);
	unsigned int transparentTexture = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\flowers.png", true);

	transparentShader.use();
	transparentShader.setInt("transTexture", 0);

	containerShader.use();
	containerShader.setInt("diffuseTex", 0);
	containerShader.setInt("specularTex", 1);


	// transparent vegetation locations
	// --------------------------------
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};


	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
#pragma region Light Set Up

	// directional light
	containerShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	containerShader.setVec3("dirLight.ambient", 0.15f, 0.1f, 0.08f);
	containerShader.setVec3("dirLight.diffuse", 0.9f, 0.85f, 0.85f);
	containerShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
	// point light 1
	//containerShader.setVec3("pointLights[0].position", pointLightPositions[0]);
	containerShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	containerShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	containerShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	containerShader.setFloat("pointLights[0].constant", 1.0f);
	containerShader.setFloat("pointLights[0].linear", 0.19);
	containerShader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	//containerShader.setVec3("pointLights[1].position", pointLightPositions[1]);
	containerShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	containerShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	containerShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	containerShader.setFloat("pointLights[1].constant", 1.0f);
	containerShader.setFloat("pointLights[1].linear", 0.19);
	containerShader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	//containerShader.setVec3("pointLights[2].position", pointLightPositions[2]);
	containerShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	containerShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	containerShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	containerShader.setFloat("pointLights[2].constant", 1.0f);
	containerShader.setFloat("pointLights[2].linear", 0.19);
	containerShader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	//containerShader.setVec3("pointLights[3].position", pointLightPositions[3]);
	containerShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	containerShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	containerShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	containerShader.setFloat("pointLights[3].constant", 1.0f);
	containerShader.setFloat("pointLights[3].linear", 0.19);
	containerShader.setFloat("pointLights[3].quadratic", 0.032);

#pragma endregion
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);


		glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		containerShader.use();
		containerShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		containerShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		containerShader.setFloat("material.shininess", 20);

		containerShader.setVec3("viewPos", camera.Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		glm::vec3 ray = camera.screenPointToWorldRay(projection, glm::vec2(SCR_WIDTH, SCR_HEIGHT), glm::vec2(x, y));

		glUseProgram(NULL);
		glLineWidth(15);
		glColor3f(1, 1, 1);
		glBegin(GL_LINES);
		glVertex3f(camera.Position.x, camera.Position.y, camera.Position.z);
		glVertex3f(ray.x * 1000, ray.y * 1000, ray.z * 1000);
		glEnd();

		containerShader.use();
		containerShader.setMat4("projection", projection);
		containerShader.setMat4("view", view);

		// world transformation
		glm::mat4 model;
		containerShader.setMat4("model", model);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		containerShader.setMat4("model", glm::translate(model, glm::vec3(2, 0, 4)) * glm::scale(model, glm::vec3(0.2, 0.2, 0.2)) * glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0)));
		houseModel.Draw(containerShader);
		model = glm::mat4();
		containerShader.setMat4("model", glm::translate(model, glm::vec3(0, -1.25, 4)) * glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f)));
		model = glm::mat4();
		podium.Draw(containerShader);
		containerShader.setMat4("model", glm::translate(model, glm::vec3(0, 0, 4)) * glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)) *glm::rotate(model, currentFrame, glm::vec3(0, 1, 0)));

		ourModel.Draw(containerShader);
		containerShader.setMat4("model", glm::scale(model, glm::vec3()));
		// render the cube
		glBindVertexArray(cubeVAO);

		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			containerShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// also draw the lamp object(s)
		lightShader.use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4();
			glm::vec3 newPos = glm::vec3(sin(cos(currentFrame * 0.15f) *(i + 1) * currentFrame * 0.13f) * 8, cos(sin(currentFrame * 0.16)*(i + 1) * currentFrame * 0.12) * 3, sin(i * cos(i * currentFrame * 0.18)) * 8);
			containerShader.use();
			std::string uniformName = "pointLights[" + std::to_string(i) + "].position";
			containerShader.setVec3(uniformName, newPos);
			lightShader.use();
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.13f)); // Make it a smaller cube
			lightShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		transparentShader.use();
		transparentShader.setMat4("projection", projection);
		transparentShader.setMat4("view", view);

		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, transparentTexture);
		for (unsigned int i = 0; i < vegetation.size(); i++)
		{
			model = glm::mat4();
			model = glm::translate(model, vegetation[i]);
			transparentShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
unsigned int loadTexture(char const * path, bool clampTexture)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		GLenum type = (clampTexture == true) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}