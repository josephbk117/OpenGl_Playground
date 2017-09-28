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

//---Physics--//
#include "BoundingSphere.h"
#include "IntersectData.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(std::vector<std::string> faces);

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
	BoundingSphere sphere1(glm::vec3(0, 0, 0), 3);
	BoundingSphere sphere2(glm::vec3(0, 0, 3), 3);
	IntersectData iData = sphere1.intersectBoundingSphere(sphere2);

	std::cout << "Sphere 1 is intersecting : " << iData.getDoesIntersect() << " , distance : " << iData.getDistance();

	glfwInit();
	//glfwWindowHint(GLFW_SAMPLES, 8); -- Multisampling wont work until i set samples for the frame buffer

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_MULTISAMPLE);

	GLenum err = glewInit();

	if (GLEW_OK != err)
		std::cout << "Error: %s\n" << glewGetErrorString(err);
	else
		std::cout << " Glew initialsed" << std::endl;

	Shader containerShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\basic.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\basic.fs");
	Shader lightShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\lightBasic.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\lightBasic.fs");
	Shader transparentShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\transparentBasic.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\transparentBasic.fs");
	Shader screenShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\screenShader.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\screenShader.fs");
	Shader cubeMapShader("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\cubeMap.vs",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\cubeMap.fs");


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

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							 // positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
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

	//________Frame buffer stuff___________
	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//_______________________________________________

	unsigned int diffuseMap = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\cartoonyCrate.jpg", false);
	unsigned int specularMap = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\cartoonyCrate_Specular.jpg", false);
	unsigned int transparentTexture = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\flowers.png", true);
	unsigned int translucentText = loadTexture("C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\semiTrans.png", true);

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

	std::vector<glm::vec3> translucentPositions
	{
		glm::vec3(-2.5f, 0.0f, -0.7f),
		glm::vec3(3.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 1.6f),
		glm::vec3(-0.6f, 0.0f, -3.3f),
		glm::vec3(0.9f, 0.0f, -1.6f)
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

	//_______________________________________________
	// framebuffer configuration
	// -------------------------
	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
																								  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//______CREATING CUBEMAP______
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	unsigned int cubemapVAO, cubemapVBO;
	glGenVertexArrays(1, &cubemapVAO);
	glGenBuffers(1, &cubemapVBO);
	glBindVertexArray(cubemapVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	std::vector<std::string> faces
	{
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\right.jpg",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\left.jpg",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\top.jpg",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\bottom.jpg",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\back.jpg",
		"C:\\Users\\josep_000\\Documents\\Visual Studio 2017\\Projects\\OpenGl_Playground\\Debug\\front.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);
	cubeMapShader.use();
	cubeMapShader.setInt("skybox", 0);

	glEnable(GL_DEPTH_TEST);
#pragma endregion
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		processInput(window);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		// bind to framebuffer and draw scene as we normally would to color texture 
		
		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

								 // make sure we clear the framebuffer's content
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// draw skybox as last
		//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		cubeMapShader.use();
		
		cubeMapShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
		cubeMapShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(cubemapVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//glDepthFunc(GL_LESS); // set depth function back to default

		// be sure to activate shader when setting uniforms/drawing objects
		containerShader.use();
		containerShader.setVec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.4f));
		containerShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		containerShader.setFloat("material.shininess", 20);

		containerShader.setVec3("viewPos", camera.Position);

		// view/projection transformations

		containerShader.setMat4("projection", projection);
		containerShader.setMat4("view", view);
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
		glBindTexture(GL_TEXTURE_2D, translucentText);
		for (unsigned int i = 0; i < translucentPositions.size(); i++)
		{
			model = glm::mat4();
			model = glm::translate(model, translucentPositions[i]);
			transparentShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//_______________________
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		//glClear(GL_COLOR_BUFFER_BIT);

		//screenShader.use();
		//glBindVertexArray(quadVAO);
		//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//________SKYBOX______
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &transparentVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &transparentVBO);
	glDeleteBuffers(1, &quadVBO);

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
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}