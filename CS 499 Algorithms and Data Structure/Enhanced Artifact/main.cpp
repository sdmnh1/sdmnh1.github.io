// CS 499 - Milestone Three - Algorithms and Data Structures
//=============================================================================
// Created By : Maria Herring   Email: maria.herring@snhu.edu
// File Name : Enhancement Two
// Description : Enhancement of CS 330 Final Project of a 3D Scene
//=============================================================================
// References--
// cylinder.cpp from :  https://www.songho.ca/opengl/gl_cylinder.html

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Math Headers Inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

/* User defined function prototypes to set callbacks
* for the framebuffer, mouse, and scroll.
* Functions to process input and load textures
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// variables for the window width and height settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool isOrtho = false;


// add vectice
void addVertex(std::vector<float>& vertices, float x, float y, float z)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
}
// add normal
void addNormal(std::vector<float>& normals, float nx, float ny, float nz)
{
	normals.push_back(nx);
	normals.push_back(ny);
	normals.push_back(nz);
}
// add tex coordinate
void addTexCoord(std::vector<float>& texCoords, float s, float t)
{
	texCoords.push_back(s);
	texCoords.push_back(t);
}
// add 3 indices
void addIndices(std::vector<unsigned int>& indices, unsigned int i1, unsigned int i2, unsigned int i3)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}
/* Enhancement: Algorithmic function which calulates a surface normal. 
*  The cross product or vector product of two edges of the triangle 
*  are calucalated. After the cross product is computed the normal is calculated.
*  The algorithm improves the efficiency and accuracy of the surface normal calculations.
*  The function consist of constant-time operations.
   The total time complexity of the function can be measured as O(1).
*/ 
// Source : https://www.songho.ca/opengl/gl_cylinder.html
// return face normal of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector
std::vector<float> computeFaceNormal(float x1, float y1, float z1,  // v1
	float x2, float y2, float z2,  // v2
	float x3, float y3, float z3)  // v3
{
	const float EPSILON = 0.000001f;

	std::vector<float> normal(3, 0.0f);     // default return value (0,0,0)
	float nx, ny, nz;

	// find 2 edge vectors: v1-v2, v1-v3
	float ex1 = x2 - x1;
	float ey1 = y2 - y1;
	float ez1 = z2 - z1;
	float ex2 = x3 - x1;
	float ey2 = y3 - y1;
	float ez2 = z3 - z1;

	// cross product: e1 x e2
	nx = ey1 * ez2 - ez1 * ey2;
	ny = ez1 * ex2 - ex1 * ez2;
	nz = ex1 * ey2 - ey1 * ex2;

	// normalize only if the length is > 0
	float length = sqrtf(nx * nx + ny * ny + nz * nz);
	if (length > EPSILON)
	{
		// normalize
		float lengthInv = 1.0f / length;
		normal[0] = nx * lengthInv;
		normal[1] = ny * lengthInv;
		normal[2] = nz * lengthInv;
	}

	return normal;
}
// generates cylinder
int generateCylinder(GLuint* VAO, float baseRadius, float topRadius, float height, int sectorCount)
{
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	std::vector<unsigned int> indices;

	unsigned int baseIndex;                 // starting index of base
	unsigned int topIndex;                  // starting index of top

	int stackCount = 1;

	std::vector<float> unitCircleVertices;

	const float PI = acos(-1.0f);
	float sectorStep = 2 * PI / sectorCount;
	float sectorAngle;  // radian

	for (int i = 0; i <= sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(sin(sectorAngle)); // y
		unitCircleVertices.push_back(0);                // z
	}


	// tmp vertex definition (x,y,z,s,t)
	struct Vertex
	{
		float x, y, z, s, t;
	};
	std::vector<Vertex> tmpVertices;

	int i, j, k;    // indices
	float x, y, z, s, t, radius;

	// put tmp vertices of cylinder side to array by scaling unit circle
	//NOTE: start and end vertex positions are same, but texcoords are different
	//      so, add additional vertex at the end point
	for (i = 0; i <= stackCount; ++i)
	{
		z = -(height * 0.5f) + (float)i / stackCount * height;      // vertex position z
		radius = baseRadius + (float)i / stackCount * (topRadius - baseRadius);     // lerp
		t = 1.0f - (float)i / stackCount;   // top-to-bottom

		for (j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			x = unitCircleVertices[k];
			y = unitCircleVertices[k + 1];
			s = (float)j / sectorCount;

			Vertex vertex;
			vertex.x = x * radius;
			vertex.y = y * radius;
			vertex.z = z;
			vertex.s = s;
			vertex.t = t;
			tmpVertices.push_back(vertex);
		}
	}


	Vertex v1, v2, v3, v4;      // 4 vertex positions v1, v2, v3, v4
	std::vector<float> n;       // 1 face normal
	int vi1, vi2;               // indices
	int index = 0;

	// v2-v4 <== stack at i+1
	// | \ |
	// v1-v3 <== stack at i
	for (i = 0; i < stackCount; ++i)
	{
		vi1 = i * (sectorCount + 1);            // index of tmpVertices
		vi2 = (i + 1) * (sectorCount + 1);

		for (j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
		{
			v1 = tmpVertices[vi1];
			v2 = tmpVertices[vi2];
			v3 = tmpVertices[vi1 + 1];
			v4 = tmpVertices[vi2 + 1];

			n = computeFaceNormal(v1.x, v1.y, v1.z, v3.x, v3.y, v3.z, v2.x, v2.y, v2.z);

			// put quad vertices: v1-v2-v3-v4
			addVertex(vertices, v1.x, v1.y, v1.z);
			addVertex(vertices, v2.x, v2.y, v2.z);
			addVertex(vertices, v3.x, v3.y, v3.z);
			addVertex(vertices, v4.x, v4.y, v4.z);

			addTexCoord(texCoords, v1.s * 3.0f, v1.t * 1.0f);
			addTexCoord(texCoords, v2.s * 3.0f, v2.t * 1.0f);
			addTexCoord(texCoords, v3.s * 3.0f, v3.t * 1.0f);
			addTexCoord(texCoords, v4.s * 3.0f, v4.t * 1.0f);

			for (k = 0; k < 4; ++k)  // same normals for all 4 vertices
			{
				addNormal(normals, n[0], n[1], n[2]);
			}

			// put indices of a quad
			addIndices(indices, index, index + 2, index + 1);    // v1-v3-v2
			addIndices(indices, index + 1, index + 2, index + 3);    // v2-v3-v4

			index += 4;     // for next
		}
	}

	// remember where the base index starts
	baseIndex = (unsigned int)indices.size();
	unsigned int baseVertexIndex = (unsigned int)vertices.size() / 3;

	// put vertices of base of cylinder
	z = -height * 0.5f;
	addVertex(vertices, 0, 0, z);
	addNormal(normals, 0, 0, -1);
	addTexCoord(texCoords, 0.5f, 0.5f);
	for (i = 0, j = 0; i < sectorCount; ++i, j += 3)
	{
		x = unitCircleVertices[j];
		y = unitCircleVertices[j + 1];
		addVertex(vertices, x * baseRadius, y * baseRadius, z);
		addNormal(normals, 0, 0, -1);
		addTexCoord(texCoords, -x * 0.5f + 0.5f, -y * 0.5f + 0.5f); // flip horizontal
	}

	// put indices for base
	for (i = 0, k = baseVertexIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
			addIndices(indices, baseVertexIndex, k + 1, k);
		else
			addIndices(indices, baseVertexIndex, baseVertexIndex + 1, k);
	}

	// remember where the top index starts
	topIndex = (unsigned int)indices.size();
	unsigned int topVertexIndex = (unsigned int)vertices.size() / 3;

	// put vertices of top of cylinder
	z = height * 0.5f;
	addVertex(vertices, 0, 0, z);
	addNormal(normals, 0, 0, 1);
	addTexCoord(texCoords, 0.5f, 0.5f);
	for (i = 0, j = 0; i < sectorCount; ++i, j += 3)
	{
		x = unitCircleVertices[j];
		y = unitCircleVertices[j + 1];
		addVertex(vertices, x * topRadius, y * topRadius, z);
		addNormal(normals, 0, 0, 1);
		addTexCoord(texCoords, x * 0.5f + 0.5f, -y * 0.5f + 0.5f);
	}

	for (i = 0, k = topVertexIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
			addIndices(indices, topVertexIndex, k, k + 1);
		else
			addIndices(indices, topVertexIndex, k, topVertexIndex + 1);
	}

	// generate interleaved vertices
	std::vector<float> interleavedVertices;

	std::size_t i1, j1;
	std::size_t count = vertices.size();
	for (i1 = 0, j1 = 0; i1 < count; i1 += 3, j1 += 2)
	{
		interleavedVertices.insert(interleavedVertices.end(), &vertices[i1], &vertices[i1] + 3);

		interleavedVertices.insert(interleavedVertices.end(), &normals[i1], &normals[i1] + 3);

		interleavedVertices.insert(interleavedVertices.end(), &texCoords[j1], &texCoords[j1] + 2);
	}

	// send to GPU
	glGenVertexArrays(1, VAO);                                 // generate vertex array object
	glBindVertexArray(*VAO);                                   // binds the vertex array object

	GLuint crownVBO;
	glGenBuffers(1, &crownVBO);                                // generates the number of buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, crownVBO);                   // binds the buffer 
	glBufferData(GL_ARRAY_BUFFER,                              // specifies the target to which the buffer object is bound for
		(unsigned int)interleavedVertices.size() * sizeof(float),
		&interleavedVertices[0],
		GL_STATIC_DRAW);

	GLuint crownIBO;
	glGenBuffers(1, &crownIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crownIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int),
		&indices[0],
		GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	return indices.size();

}
// generates plane
int generatePlane(GLuint* VAO)
{
	std::vector<float> planeVertices = {
		// positions            // normals              // texture coords
		-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		2.0f, 0.0f,
		1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		2.0f, 2.0f,
		1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 2.0f,
	};

	std::vector<unsigned int> planeIndices = {
	   //indices
		0, 1, 2,
		0, 2, 3
	};

	unsigned int planeVBO, planeEBO;

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), &planeVertices[0], GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices.size() * sizeof(unsigned int), &planeIndices[0], GL_STATIC_DRAW);

	return planeIndices.size();
}
// generates cube
int generateCube(GLuint* VAO)
{
	std::vector<float> vertices = {
		// positions          // normals           // texture coords

		// back side
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		// front side
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,


		// left side
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		// right side
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		// bottom side
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		// top side
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	unsigned int cubeVBO;

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	return vertices.size() / 8;

}
// key callback resets camera 
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		// Key release event handling
		if (key == GLFW_KEY_P) {
			isOrtho = !isOrtho;

			if (isOrtho)
				camera.ResetCamera();
		}
	}
}
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Scene", NULL, NULL);
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
	glfwSetKeyCallback(window, keyCallback);


	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shader program
	Shader lightingShader("shaderfiles/lighting.vs", "shaderfiles/lighting.fs");

	GLuint planeVAO = 0;
	int planeVertexCount = generatePlane(&planeVAO);

	GLuint cubeVAO = 0;
	int cubeVertexCount = generateCube(&cubeVAO);

	GLuint cylinderVAO = 0;
	int cylinderVertexCount = generateCylinder(&cylinderVAO, 1.0f, 1.0f, 1.0f, 36);

	GLuint coneVAO = 0;
	int coneVertexCount = generateCylinder(&coneVAO, 1.0f, 0.1f, 1.0f, 36);

	auto floorTexture = loadTexture("wood.jpg");

	// shader configuration
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);

	glActiveTexture(GL_TEXTURE0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// render
		glClearColor(0.46f, 0.46f, 0.46f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		
		// here we set all the uniforms for the types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. 

		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("dirLight.intensity", 1.0f);

		// point light 1
		lightingShader.setVec3("pointLights[0].position", glm::vec3(0.0f, 3.0f, 0.0f));
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		lightingShader.setFloat("pointLights[0].intensity", 1.0f);

		// point light 2
		lightingShader.setVec3("pointLights[1].position", glm::vec3(-8.0f, 3.0f, -8.0f));
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.0f);
		lightingShader.setVec3("pointLights[1].specular", 0.8f, 0.8f, 0.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		lightingShader.setFloat("pointLights[1].intensity", 1.0f);

		// point light 3
		lightingShader.setVec3("pointLights[2].position", glm::vec3(8.0f, 3.0f, -8.0f));
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.0f, 0.0f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 0.0f, 0.0f, 0.8f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		lightingShader.setFloat("pointLights[2].intensity", 1.0f);

		// point light 4
		lightingShader.setVec3("pointLights[3].position", glm::vec3(-8.0f, 3.0f, 8.0f));
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.0f, 0.8f, 0.0f);
		lightingShader.setVec3("pointLights[3].specular", 0.0f, 0.8f, 0.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		lightingShader.setFloat("pointLights[3].intensity", 1.0f);

		// point light 5
		lightingShader.setVec3("pointLights[4].position", glm::vec3(8.0f, 3.0f, 8.0f));
		lightingShader.setVec3("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[4].diffuse", 0.8f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[4].specular", 0.8f, 0.0f, 0.0f);
		lightingShader.setFloat("pointLights[4].constant", 1.0f);
		lightingShader.setFloat("pointLights[4].linear", 0.09);
		lightingShader.setFloat("pointLights[4].quadratic", 0.032);
		lightingShader.setFloat("pointLights[4].intensity", 1.0f);


		// view/projection transformations
		glm::mat4 projection = glm::mat4(1.0);

		if (isOrtho)
		{
			float zoomFactor = 0.5f;

			float left = -zoomFactor * (float)SCR_WIDTH / 2.0f;
			float right = zoomFactor * (float)SCR_WIDTH / 2.0f;
			float bottom = -zoomFactor * (float)SCR_HEIGHT / 2.0f;
			float top = zoomFactor * (float)SCR_HEIGHT / 2.0f;

			glm::mat4 projection = glm::ortho(left, right, bottom, top, 0.1f, 1000.0f);
		}
		else
		{
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}

		// also draw the lamp object(s)
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);

		// plane translations- binds VAOs and textures, applies model and draws the triangles
		{
			glBindVertexArray(planeVAO);                     
			lightingShader.setInt("hasTexture", 1);
			glBindTexture(GL_TEXTURE_2D, floorTexture);

			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(8.0f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, planeVertexCount, GL_UNSIGNED_INT, 0);
		}

		// pen translations- binds VAOs and textures, applies model and draws the triangles
		{
			glBindVertexArray(cylinderVAO);
			lightingShader.setInt("hasTexture", 0);

			glm::mat4 pModel(1.0f);
			pModel = glm::translate(pModel, glm::vec3(0.0f, 0.02f, 0.0f));
			pModel = glm::rotate(pModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.5f));
			lightingShader.setVec3("meshColor", glm::vec3(11/255.0f, 73/255.0f, 60/255.0f));
			lightingShader.setMat4("model", pModel * model );
			glDrawElements(GL_TRIANGLES, cylinderVertexCount, GL_UNSIGNED_INT, (void*)0);

			glBindVertexArray(coneVAO);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.26f));
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			lightingShader.setVec3("meshColor", glm::vec3(0.2549f, 0.2666f, 0.1450f));
			lightingShader.setMat4("model", pModel * model);
			glDrawElements(GL_TRIANGLES, coneVertexCount, GL_UNSIGNED_INT, (void*)0);
		}

		// airpods translations- binds VAOs and textures, applies model and draws the triangles
		{
			glBindVertexArray(cubeVAO);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.25f, 0.05f, -0.4f));
			model = glm::scale(model, glm::vec3(0.2f, 0.1f, 0.15f));
			lightingShader.setMat4("model", model);
			lightingShader.setVec3("meshColor", glm::vec3(1.0f, 1.0f, 1.0f));
			glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
		}

		// case translations- binds VAOs and textures, applies model and draws the triangles
		{
			glBindVertexArray(cubeVAO);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.8f, 0.05f, -0.3f));
			model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.25f, 0.1f, 0.5f));
			lightingShader.setMat4("model", model);
			lightingShader.setVec3("meshColor", glm::vec3(217/255.0f, 98/255.0f, 18/255.0f));
			glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
		}

		// bottle translations- binds VAOs and textures, applies model and draws the triangles
		{
			glBindVertexArray(cylinderVAO);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-0.35f, 0.3f, -0.3f));
			model = glm::scale(model, glm::vec3(0.1f, 0.6f, 0.1f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			lightingShader.setVec3("meshColor", glm::vec3(58 / 255.0f, 9 / 255.0f, 15 / 255.0f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, cylinderVertexCount, GL_UNSIGNED_INT, (void*)0);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-0.35f, 0.6f, -0.3f));
			model = glm::scale(model, glm::vec3(0.07f, 0.1f, 0.07f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			lightingShader.setVec3("meshColor", glm::vec3(52 / 255.0f, 61 / 255.0f, 58 / 255.0f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, cylinderVertexCount, GL_UNSIGNED_INT, (void*)0);
		}
	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
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
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
// utility function for loading a texture from a file
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
        // failed texture load error message
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}