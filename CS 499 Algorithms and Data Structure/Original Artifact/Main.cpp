/* Maria Herring
CS-330 Comp and Visualization 23EW3
Module Seven 7-1 Final Project
Instuctor: Kurt Diesch
February 19, 2023
*/

#include <iostream>             // cout, cerr
#include <cstdlib>              // EXIT_FAILURE
#include <GL/glew.h>            // GLEW library
#include <GLFW/glfw3.h>         // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"       

 // GLM Math Header inclusions
#include <glm.hpp>      
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include "camera.h" 


using namespace std; // Standard namespace
/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

namespace

{
	float pi = 3.14159265359;
	float toRadians = (pi / 180);

	const char* const WINDOW_TITLE = " 7-1 Final Project "; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 800;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao[6]; // Handle for the vertex array object
		GLuint vbo[6]; // Handle for the vertex buffer object
		GLuint nVertices[6]; // Number of indices of the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;

	// Texture(s)
	GLuint pencilTextureId;
	GLuint planeTextureId;
	GLuint caseTextureId;
	GLuint bottleTextureId;
	GLuint bottleTexture2Id;
	GLuint podsTextureId;

	glm::vec2 gUVScale(1.0f, 1.0f);
	GLint gTexWrapMode = GL_REPEAT;

	// Triangle mesh data
	GLMesh gMesh;

	// Shader program
	GLuint gProgramId;
	GLuint gLampProgramId;

	// camera
	Camera gCamera(glm::vec3(1.0f, 1.5f, 8.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;
	bool isPerspective = true;

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;


	// Light: color, position, object, and scale
	glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f); // white
	glm::vec3 gLightColor(1.0f, 1.0f, 1.0f); // white
	glm::vec3 gcaseLightColor(1.0f, 1.0f, 0.85f); // light yellow
	glm::vec3 gcaseLightPosition(2.3f, 0.0f, 0.6f);
	glm::vec3 gLightPosition(2.5f, 6.5f, 3.5f); // above plane-center/right
	glm::vec3 gLightScale(0.5f);

}
/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */

bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint&
	programId);
void UDestroyShaderProgram(GLuint programId);

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position;      // Vertex data from VertexAttrib Pointer 0
layout(location = 1) in vec3 normal;            // Normal data
layout(location = 2) in vec2 textureCoordinate; // Color data from Vertex Attrib Pointer 2

// Normals, fragments, and texture coordinates
out vec3 vertexNormal;              // outgoing normals
out vec3 vertexFragmentPos;         // outgoing fragment positions
out vec2 vertexTextureCoordinate;   // outgoing texture coordinate

//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

	vertexFragmentPos = vec3(model * vec4(position, 1.0f));         // Gets fragment / pixel position in world space only 

	vertexNormal = mat3(transpose(inverse(model))) * normal;        // get normal vectors in world space
	vertexTextureCoordinate = textureCoordinate;
}
);

/* Shader Source Code is here ( fragment)*/
const GLchar* fragmentShaderSource = GLSL(440,
	in vec3 vertexNormal;              // Incoming normals
in vec3 vertexFragmentPos;             // Incoming fragment position
in vec2 vertexTextureCoordinate;       // Incoming texture coordinates

out vec4 fragmentColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 caseColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture;            // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
	/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

	//Calculate Ambient lighting*/
	float ambientStrength = 0.5f;                  // Set ambient or global lighting strength FIXME: adjust if too dark or bright
	vec3 ambient = ambientStrength * lightColor;   // Generate ambient light color

	//Calculate Diffuse lighting*/
	vec3 norm = normalize(vertexNormal);                           // Normalize vectors to 1 unit
	vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
	float impact = max(dot(norm, lightDirection), 0.0);            // Calculate diffuse impact by generating dot product of normal and light
	vec3 diffuse = impact * lightColor;                            // Generate diffuse light color

	//Calculate Specular lighting*/
	float specularIntensity = 0.8f;                                // Set specular light strength
	float highlightSize = 12.0f;                                   // Set specular highlight size
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos);    // Calculate view direction
	vec3 reflectDir = reflect(-lightDirection, norm);              // Calculate reflection vector

	//Calculate specular component
	float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
	vec3 specular = specularIntensity * specularComponent * lightColor;

	// Texture holds the color to be used for all three components
	vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

	// Calculate phong result
	vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

	fragmentColor = vec4(phong, 1.0);                         // Send lighting results to GPU
}
);

/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position;                    // VAP position 0 for vertex position data

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

	out vec4 fragmentColor;                   // For outgoing lamp color (smaller cube) to the GPU

void main()
{
	fragmentColor = vec4(1.0f);               // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j) {
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;
		for (int i = width * channels; i > 0; --i) {
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}
int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the mesh
	UCreateMesh(gMesh);

	// Create the shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

	if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
		return EXIT_FAILURE;

	// Load texture(s)
	const char* pencilFilename = "pencil_tex.jpg";
	const char* planeFilename = "wood_texture.jpg";
	const char* caseFilename = "orange_texture.jpg";
	const char* bottleFilename = "red_texture.jpg";
	const char* bottle2Filename = "black_texture.jpg";
	const char* podsFilename = "white_texture.jpg";

	if (!UCreateTexture(pencilFilename, pencilTextureId)) {
		cout << "Failed to load texture " << pencilFilename << endl;
		return EXIT_FAILURE;
	}
	if (!UCreateTexture(planeFilename, planeTextureId)) {
		cout << "Failed to load texture " << planeFilename << endl;
		return EXIT_FAILURE;
	}
	if (!UCreateTexture(caseFilename, caseTextureId)) {
		cout << "Failed to load texture " << caseFilename << endl;
		return EXIT_FAILURE;
	}
	if (!UCreateTexture(bottleFilename, bottleTextureId)) {
		cout << "Failed to load texture " << bottleFilename << endl;
		return EXIT_FAILURE;
	}
	if (!UCreateTexture(bottle2Filename, bottleTexture2Id)) {
		cout << "Failed to load texture " << bottle2Filename << endl;
		return EXIT_FAILURE;
	}
	if (!UCreateTexture(podsFilename, podsTextureId)) {
		cout << "Failed to load texture " << podsFilename << endl;
		return EXIT_FAILURE;
	}

	glUseProgram(gProgramId);                                        // tell opengl for each sampler to which texture unit it belongs to
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);    // We set the texture as texture unit 0


	// Loop for render
	while (!glfwWindowShouldClose(gWindow))
	{
		// Timing

		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;
		// Input
		// -----
		UProcessInput(gWindow);

		// Render this frame
		URender();
		glfwPollEvents();
	}

	// Release mesh data
	UDestroyMesh(gMesh);

	// Release shader program
	UDestroyShaderProgram(gProgramId);
	UDestroyShaderProgram(gLampProgramId);

	// Release Texture 
	UDestroyTexture(pencilTextureId);
	UDestroyTexture(planeTextureId);
	UDestroyTexture(caseTextureId);
	UDestroyTexture(bottleTextureId);
	UDestroyTexture(bottleTexture2Id);
	UDestroyTexture(podsTextureId);

	exit(EXIT_SUCCESS);                           // Terminates the program successfully
}

// Creation of Window and Initialize GLFW, GLEW
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// GLFW: window sizes

	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// Inform GLFW to capture our mouse
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW: initialize
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}
	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
	return true;
}

// Process all input
void UProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		isPerspective = !isPerspective;

}
// Glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}
	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos;                    // Reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}
// Mouse scroll
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// Mouse button handle
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
	break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;
	}
	break;
	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
	break;
	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}
}

// Called to render a frame function
void URender()
{
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.46f, 0.46f, 0.46f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the shader to be used
	glUseProgram(gProgramId);

	// Camera/view transformation
	glm::mat4 view = gCamera.GetViewMatrix();


	// Creates a projection THEN create perspective or orthographic view
	glm::mat4 projection;

	// NOW check if 'isPerspective' and THEN set the perspective
	if (isPerspective) {
		projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else {
		float scale = 90;
		projection = glm::ortho((800.0f / scale), -(800.0f / scale), -(600.0f / scale), (600.0f / scale), -2.5f, 6.5f);
	}

	// Plane 
	glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f));
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 model = translation * rotation * scale;

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
	GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
	GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");


	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Pass data to case light
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColorLoc, gcaseLightColor.r, gcaseLightColor.g, gcaseLightColor.b);
	glUniform3f(lightPositionLoc, gcaseLightPosition.x, gcaseLightPosition.y, gcaseLightPosition.z);



	GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao[0]);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTextureId);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[0]);


	// Pencil object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	rotation = glm::rotate(2.0f * (toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-1.0f, 0.0f, 1.7f));
	model = translation * rotation * scale;
	modelLoc = glGetUniformLocation(gProgramId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//Activate the VBOs
	glBindVertexArray(gMesh.vao[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pencilTextureId);
	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[1]);


	// Eyeglass Case
	scale = glm::scale(glm::vec3(2.5f, 0.5f, 1.0f));
	rotation = glm::rotate(75.0f * (toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(2.6f, 0.0f, 1.0f));
	model = translation * rotation * scale;
	modelLoc = glGetUniformLocation(gProgramId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//Activate the VBOs
	glBindVertexArray(gMesh.vao[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, caseTextureId);
	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[2]);


	// Water Bottle
	scale = glm::scale(glm::vec3(0.6f, 2.7f, 1.0f));
	rotation = glm::rotate(25.0f * (toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-1.5f, 0.0f, -1.0f));
	model = translation * rotation * scale;
	modelLoc = glGetUniformLocation(gProgramId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//Activate the VBOs
	glBindVertexArray(gMesh.vao[3]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bottleTextureId);
	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[3]);


	// Water Bottle Cap
	scale = glm::scale(glm::vec3(0.35f, 0.5f, 1.0f));
	rotation = glm::rotate(25.0f * (toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-1.5f, 2.7f, -1.0f));
	model = translation * rotation * scale;
	modelLoc = glGetUniformLocation(gProgramId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//Activate the VBOs
	glBindVertexArray(gMesh.vao[4]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bottleTexture2Id);
	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[4]);


	// Airpod Case
	scale = glm::scale(glm::vec3(0.5f, 0.2f, 0.8f));
	rotation = glm::rotate(90.0f * (toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(0.85f, 0.0f, -0.5f));
	model = translation * rotation * scale;
	modelLoc = glGetUniformLocation(gProgramId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//Activate the VBOs
	glBindVertexArray(gMesh.vao[5]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, podsTextureId);
	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[5]);


	// LAMP: draw light
	glUseProgram(gLampProgramId);

	// Transform the smaller cube used as a visual que for the light source
	model = glm::translate(gLightPosition) * glm::scale(gLightScale);

	// Reference matrix uniforms from the Lamp Shader program
	modelLoc = glGetUniformLocation(gLampProgramId, "model");
	viewLoc = glGetUniformLocation(gLampProgramId, "view");
	projLoc = glGetUniformLocation(gLampProgramId, "projection");

	// Pass matrix data to the Lamp Shader program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices[2]);


	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	glfwSwapBuffers(gWindow);                    // Flips the the back buffer with the front buffer every frame.
}

// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
	// Vertex data
	GLfloat planeVerts[] = {
		// Vertex Positions  // Normals        // Texture
		-5.0f, -0.1f,  5.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 5.0f, -0.1f, -5.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-5.0f, -0.1f, -5.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

		-5.0f, -0.1f,  5.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 5.0f, -0.1f, -5.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		 5.0f, -0.1f,  5.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	};

	GLfloat pencilVerts[] = {
		// Pencil body
		   // Vertex Positons     // Normals     // Texture
			// Back
			-1.0f,  0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
			 1.0f,  0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
			-1.0f, -0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

			 1.0f, -0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
			-1.0f, -0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
			 1.0f,  0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

			 // Front 
			 -1.0f,  0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
			  1.0f,  0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
			 -1.0f, -0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,

			  1.0f, -0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
			 -1.0f, -0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
			  1.0f,  0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,

			  // Left 
			 -1.0f,  0.1f,  0.1f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
			 -1.0f,  0.1f, -0.1f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			 -1.0f, -0.1f, -0.1f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

			 -1.0f, -0.1f, -0.1f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
			 -1.0f, -0.1f,  0.1f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			 -1.0f,  0.1f,  0.1f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

			 // Bottom 
			-1.0f, -0.1f,  0.1f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			-1.0f, -0.1f, -0.1f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f, -0.1f, -0.1f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,

			 1.0f, -0.1f,  0.1f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			-1.0f, -0.1f,  0.1f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			 1.0f, -0.1f, -0.1f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,

			 // Top 
			-1.0f,  0.1f,  0.1f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
			-1.0f,  0.1f, -0.1f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
			 1.0f,  0.1f, -0.1f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

			 1.0f,  0.1f,  0.1f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
			-1.0f,  0.1f,  0.1f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  0.1f, -0.1f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

			 // Pencil tip
			 // Front 
			 1.0f,  0.1f,  0.1f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
			 1.5f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
			 1.0f, -0.1f,  0.1f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,

			 // Back 
			 1.0f,  0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
			 1.5f,  0.0f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
			 1.0f, -0.1f, -0.1f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,

			 // Top 
			 1.0f, 0.1f,  0.1f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, 0.1f, -0.1f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
			 1.5f, 0.0f,  0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,

			 // Bottom 
			 1.0f, -0.1f,  0.1f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -0.1f, -0.1f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			 1.5f,  0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f
	};

	// Vertex Data
	GLfloat caseVerts[] = {
		// Vertex Positions     // Normals           // Texture 
		// Base (Facing Y-)
	   -0.5f,  0.0f, -0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
		0.5f,  0.0f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
		0.5f,  0.0f,  0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
	   -0.5f,  0.0f, -0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	   -0.5f,  0.0f,  0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
		0.5f,  0.0f,  0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

		// Top (Facing Y+)
	   -0.5f,  1.0f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		0.5f,  1.0f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		0.5f,  1.0f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
	   -0.5f,  1.0f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
	   -0.5f,  1.0f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
		0.5f,  1.0f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,

		// Side #1 (Facing Z-)
	   -0.5f,  0.0f, -0.5f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
		0.5f,  0.0f, -0.5f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		0.5f,  1.0f, -0.5f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
	   -0.5f,  0.0f, -0.5f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
	   -0.5f,  1.0f, -0.5f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
		0.5f,  1.0f, -0.5f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f,

		// Side #2 (Facing X-)
	   -0.5f,  1.0f, -0.5f,   1.0f,  0.0f, 0.0f,  0.0f, 0.0f,
	   -0.5f,  0.0f, -0.5f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
	   -0.5f,  0.0f,  0.5f,   1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
	   -0.5f,  1.0f, -0.5f,   1.0f,  0.0f, 0.0f,  0.0f, 0.0f,
	   -0.5f,  1.0f,  0.5f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,
	   -0.5f,  0.0f,  0.5f,   1.0f,  0.0f, 0.0f,  1.0f, 1.0f,

	   // Side #3 (Facing X+)
	   0.5f,  0.0f, -0.5f,    -1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
	   0.5f,  1.0f, -0.5f,    -1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
	   0.5f,  1.0f,  0.5f,    -1.0f,  0.0f, 0.0f,  0.0f, 0.0f,
	   0.5f,  0.0f, -0.5f,    -1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
	   0.5f,  0.0f,  0.5f,    -1.0f,  0.0f, 0.0f,  0.0f, 1.0f,
	   0.5f,  1.0f,  0.5f,    -1.0f,  0.0f, 0.0f,  0.0f, 0.0f,

	   // Side #4 (Facing Z+)
	  -0.5f,  0.0f,  0.5f,    0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	   0.5f,  0.0f,  0.5f,    0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	   0.5f,  1.0f,  0.5f,    0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	  -0.5f,  0.0f,  0.5f,    0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	  -0.5f,  1.0f,  0.5f,    0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	   0.5f,  1.0f,  0.5f,    0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	};

	GLfloat bottleVerts[] = {
		// Vertex Positions       //  Normals        // Texture 
		// Base (Facing Y-)
		-1.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		-0.7f,  0.0f, -0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		-0.7f,  0.0f, -0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		 0.0f,  0.0f, -1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		 0.0f,  0.0f, -1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		 0.7f,  0.0f, -0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		 0.7f,  0.0f, -0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		 1.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		 1.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		 0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		 0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		 0.0f,  0.0f,  1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		 0.0f,  0.0f,  1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		-0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		-0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		-1.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,

		 // Side #1
		-1.0f,  0.0f,  0.0f,   -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f,   -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,
		-0.7f,  1.0f, -0.7f,   -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,

		-1.0f,  0.0f,  0.0f,   -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,
		-0.7f,  0.0f, -0.7f,   -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,
		-0.7f,  1.0f, -0.7f,   -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,

		// Side #2
	   -0.7f,  0.0f, -0.7f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
	   -0.7f,  1.0f, -0.7f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
		0.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,

	   -0.7f,  0.0f, -0.7f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
		0.0f,  0.0f, -1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
		0.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,

		// Side #3
	   0.0f,  0.0f, -1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
	   0.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
	   0.7f,  1.0f, -0.7f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,

	   0.0f,  0.0f, -1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
	   0.7f,  0.0f, -0.7f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
	   0.7f,  1.0f, -0.7f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,

	   // Side #4
	   0.7f,  0.0f, -0.7f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	   0.7f,  1.0f, -0.7f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	   1.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

	   0.7f,  0.0f, -0.7f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	   1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	   1.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

	   // Side #5
	  1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	  1.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	  0.7f,  1.0f,  0.7f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

	  1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	  0.7f,  0.0f,  0.7f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	  0.7f,  1.0f,  0.7f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

	  // Side #6
	 0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	 0.7f,  1.0f,  0.7f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	 0.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

	 0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	 0.0f,  0.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	 0.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

	 // Side #7
	 0.0f,  0.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	 0.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	-0.7f,  1.0f,  0.7f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

	 0.0f,  0.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	-0.7f,  0.0f,  0.7f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
	-0.7f,  1.0f,  0.7f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

	// Side #8
	-0.7f,  0.0f,  0.7f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	-0.7f,  1.0f,  0.7f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

	-0.7f,  0.0f,  0.7f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	-1.0f,  0.0f,  0.0f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

	// Top (Facing Y+)
   -1.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
   -0.7f,  1.0f, -0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

   -0.7f,  1.0f, -0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	0.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

	0.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	0.7f,  1.0f, -0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

	0.7f,  1.0f, -0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	1.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

	1.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	0.7f,  1.0f,  0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

	0.7f,  1.0f,  0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
	0.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

	0.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
   -0.7f,  1.0f,  0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

   -0.7f,  1.0f,  0.7f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
   -1.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
	0.0f,  1.0f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	mesh.nVertices[0] = sizeof(planeVerts) / (sizeof(planeVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nVertices[1] = sizeof(pencilVerts) / (sizeof(pencilVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nVertices[2] = sizeof(caseVerts) / (sizeof(caseVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nVertices[3] = sizeof(bottleVerts) / (sizeof(bottleVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nVertices[4] = sizeof(bottleVerts) / (sizeof(bottleVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nVertices[5] = sizeof(caseVerts) / (sizeof(caseVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

	// Plane Mesh
	glGenVertexArrays(1, &mesh.vao[0]);                  // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(1, &mesh.vbo[0]);
	glBindVertexArray(mesh.vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);          // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW);       // Sends vertex or coordinate data

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);



	// Pencil Mesh
	glGenVertexArrays(1, &mesh.vao[1]);                  // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(1, &mesh.vbo[1]);
	glBindVertexArray(mesh.vao[1]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[1]);          // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(pencilVerts), pencilVerts, GL_STATIC_DRAW);     // Sends vertex or coordinate data

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);



	// Eyeglass Case Mesh
	glGenVertexArrays(1, &mesh.vao[2]);                 // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(1, &mesh.vbo[2]);
	glBindVertexArray(mesh.vao[2]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[2]);         // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(caseVerts), caseVerts, GL_STATIC_DRAW);         // Sends vertex or coordinate data

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);



	// Bottle  Mesh
	glGenVertexArrays(1, &mesh.vao[3]);                 // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(1, &mesh.vbo[3]);
	glBindVertexArray(mesh.vao[3]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[3]);         // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottleVerts), bottleVerts, GL_STATIC_DRAW);       // Sends vertex or coordinate data

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);



	// Bottle  Cap Mesh
	glGenVertexArrays(1, &mesh.vao[4]);                  // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(1, &mesh.vbo[4]);
	glBindVertexArray(mesh.vao[4]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[4]);          // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottleVerts), bottleVerts, GL_STATIC_DRAW);         // Sends vertex or coordinate data

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);



	// Airpods Case Mesh
	glGenVertexArrays(1, &mesh.vao[5]);                  // We can also generate multiple VAOs or buffers at the same time
	glGenBuffers(1, &mesh.vbo[5]);
	glBindVertexArray(mesh.vao[5]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[5]);           // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(caseVerts), caseVerts, GL_STATIC_DRAW);             // Sends vertex or coordinate data

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void UDestroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(4, mesh.vao);
	glDeleteBuffers(4, mesh.vbo);
}


// Generate and load the texture
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image) {
		flipImageVertically(image, width, height, channels);
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, image);
		else {
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);                 // Unbind the texture
		return true;
	}
	return false;                                        // Error loading the image
}
void UDestroyTexture(GLuint textureId) {
	glGenTextures(1, &textureId);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];
	// Create a Shader program object.
	programId = glCreateProgram();
	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);
	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId);                      // Compile the vertex shader
	// Check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}
	glCompileShader(fragmentShaderId);                  // Compile the fragment shader
	// Check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);                         // Links the shader program
	// Check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}
	glUseProgram(programId);                         // Uses the shader program
	return true;
}
void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}