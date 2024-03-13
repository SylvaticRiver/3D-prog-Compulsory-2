#include "test.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include "Shader.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "Camera.h"
#include "Model.h"

//for this project I used a tutorial for OpenGL in order to make a camera class, a separate shader class including VAO, VBO and EBO classes, and the camera class. 
//The AI path code is using the same code for calculations and printing onto file as used in the math compulsory 2, but modified to not be normalised
//The link to the tutorial used: https://youtu.be/45MIykWJ-C4?si=ovo-VxHv5dAdvnoN
//Also I tried to make a mesh class, or "Model" class as I called it, but it refused to actually render when I used it. No matter how many times I tried, even going back several steps in the tutorial
//Since the model/mesh class didn't work for me, I had to make pretty much everything in one class, which meant I had to opt for some inoptimal functions and structuring as I couldn't use object oriented programming for this
//If I had a functional mesh class I'd make AABB collision logic. I didn't here since making 13 separate collision box objects felt too chaotic given that I already had to make a bunch of variables for other functions

using namespace std;

int windowWidth = 1280;
int windowHeight = 720;

GLFWwindow* window;

double rotX = 0.0f;
double rotZ = 0.0f;

int cam = 1;

int npcind = 1;
bool npcReverse;
bool npcFirstPath = true;
std::vector<glm::vec3> positions;

array<glm::vec3, 1000> pathVerts1;
array<glm::vec3, 1000> pathVerts2;

float doorRot = 0.0f;
glm::vec3 doorTranslation = glm::vec3(0.0f);

string name("C:/Users/chris/Desktop/a/Render5/Render4/PathFiles/verticies1.txt");
string name2("C:/Users/chris/Desktop/a/Render5/Render4/PathFiles/verticies2.txt");

glm::vec3 playerPos(0.0f, 0.0f, 20.0f);

void createShape(Shader& shaderProgram, glm::vec3 positionOffset, glm::vec3 scale) {
	glm::vec3 shapePos = positionOffset;
	glm::mat4 shapeModel = glm::mat4(1.0f);
	shapeModel = glm::translate(shapeModel, shapePos);
	shapeModel = glm::scale(shapeModel, scale);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(shapeModel));
};

glm::mat4 createNewShape(Shader& shaderProgram, glm::vec3 positionOffset, glm::vec3 scale) {
	glm::vec3 lightPos = positionOffset;
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, scale);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));

	return lightModel;
};

bool isColliding(glm::vec3 startPos, glm::vec3 endPos, float marginX, float marginZ) {

	return abs(startPos[0] - endPos[0]) <= marginX && abs(startPos[2] - endPos[2]) <= marginZ;
}

glm::vec3 setPosFromCentreCollision(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 min, glm::vec3 max) {

	bool bol1 = abs(startPos[0] - endPos[0]) >= 2.3f;
	glm::vec3 finalPos;

	if (bol1) {
		finalPos[0] = startPos[0] > min[0] ? min[0] - 0.2f : max[0] + 0.2f;
		return finalPos;
	}
	finalPos[2] = startPos[2] > min[2] ? min[2] - 0.2f : max[2] + 0.2f;
	return finalPos;
}

glm::vec3 adjustPlayerPosForCamera(Camera camera) {
	return camera.position + (glm::vec3(4.0f) * camera.orientation) + glm::vec3(0.0f, -1.0f, 0.0f);
}

array<glm::vec3, 1000> readArray3DFile(string name) {
	ifstream in;
	array<glm::vec3, 1000> write;
	float amount;

	in.open(name);

	in >> amount;

	for (int i = 0; i < amount; i++) {
		glm::vec3 point;
		in >> point.x >> point.y >> point.z;
		write[i] = point;
	}

	in.close();

	return write;
}

void processInput(GLFWwindow* window, Camera& camera) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
		cout << "I have been closed" << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		cam = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		cam = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		npcFirstPath = false;
		npcind = 1;
		npcReverse = false;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		npcFirstPath = true;
		npcind = 1;
		npcReverse = false;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		playerPos[2] -= 0.01f;
		camera.position[2] -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		playerPos[2] += 0.01f;
		camera.position[2] += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		playerPos[0] -= 0.01f;
		camera.position[0] -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		playerPos[0] += 0.01f;
		camera.position[0] += 0.01f;
	}
	//camera.Inputs(window);
}
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	cout << "width = " << width << " height = " << height << endl;
}

int main() {
	//Init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Hello again", NULL, NULL);
	if (window == NULL) {
		cout << "Window was never created" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "failed to init GLAD" << endl;
		glfwTerminate();
	}
	//main

	float verticies[] = {
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,
	};

	unsigned int indices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
	};

	float cubeVert[] =
	{
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f
	};

	unsigned int cubeInd[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};

	float groundverts[] = {
		-0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f
	};

	float wallVerts[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	float wallVerts2[] = {
		0.0f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f
	};

	unsigned int groundInd[] = {
		0, 1, 3,
		1, 2, 3
	};

	pathVerts1 = readArray3DFile(name);
	pathVerts2 = readArray3DFile(name2);

	vector<glm::vec3> collectiblePoints = {
		glm::vec3(1.0f, -0.5f, 1.0f),
		glm::vec3(2.0f, -0.5f, 4.0f),
		glm::vec3(4.0f, -0.5f, 3.0f),
		glm::vec3(6.0f, -0.5f, 3.0f),
		glm::vec3(10.0f, -0.5f, 6.0f),
		glm::vec3(-5.0f, -0.5f, 7.0f)
	};

	Shader shaderProgram("C:/Users/chris/Desktop/a/Render5/Render4/Resources/Vertexshader.vert", "C:/Users/chris/Desktop/a/Render5/Render4/Resources/FragmentShader.frag");

	//pyramid
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(verticies, sizeof(verticies));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkVBO(VBO1, 0);
	VAO1.LinkVBO(VBO1, 1);
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//box
	VAO VAO2;
	VAO2.Bind();

	VBO VBO2(cubeVert, sizeof(cubeVert));
	EBO EBO2(cubeInd, sizeof(cubeInd));

	VAO2.LinkVBO(VBO2, 0);
	VAO2.LinkVBO(VBO2, 1);
	VAO2.Unbind();
	VBO2.Unbind();
	EBO2.Unbind();

	//ground
	VAO VAO3;
	VAO3.Bind();

	VBO VBO3(groundverts, sizeof(groundverts));
	EBO EBO3(groundInd, sizeof(groundInd));

	VAO3.LinkVBO(VBO3, 0);
	VAO3.LinkVBO(VBO3, 1);
	VAO3.Unbind();
	VBO3.Unbind();
	EBO3.Unbind();

	//collectibles
	VAO VAO4;
	VAO4.Bind();

	VBO VBO4(cubeVert, sizeof(cubeVert));
	EBO EBO4(cubeInd, sizeof(cubeInd));

	VAO4.LinkVBO(VBO4, 0);
	VAO4.LinkVBO(VBO4, 1);
	VAO4.Unbind();
	VBO4.Unbind();
	EBO4.Unbind();

	VAO VAO6;
	VAO6.Bind();

	VBO VBO6(cubeVert, sizeof(cubeVert));
	EBO EBO6(cubeInd, sizeof(cubeInd));

	VAO6.LinkVBO(VBO6, 0);
	VAO6.LinkVBO(VBO6, 1);
	VAO6.Unbind();
	VBO6.Unbind();
	EBO6.Unbind();

	VAO VAO7;
	VAO7.Bind();

	VBO VBO7(cubeVert, sizeof(cubeVert));
	EBO EBO7(cubeInd, sizeof(cubeInd));

	VAO7.LinkVBO(VBO7, 0);
	VAO7.LinkVBO(VBO7, 1);
	VAO7.Unbind();
	VBO7.Unbind();
	EBO7.Unbind();

	VAO VAO8;
	VAO8.Bind();

	VBO VBO8(cubeVert, sizeof(cubeVert));
	EBO EBO8(cubeInd, sizeof(cubeInd));

	VAO8.LinkVBO(VBO8, 0);
	VAO8.LinkVBO(VBO8, 1);
	VAO8.Unbind();
	VBO8.Unbind();
	EBO8.Unbind();

	VAO VAO9;
	VAO9.Bind();

	VBO VBO9(cubeVert, sizeof(cubeVert));
	EBO EBO9(cubeInd, sizeof(cubeInd));

	VAO9.LinkVBO(VBO9, 0);
	VAO9.LinkVBO(VBO9, 1);
	VAO9.Unbind();
	VBO9.Unbind();
	EBO9.Unbind();

	VAO VAO10;
	VAO10.Bind();

	VBO VBO10(cubeVert, sizeof(cubeVert));
	EBO EBO10(cubeInd, sizeof(cubeInd));

	VAO10.LinkVBO(VBO10, 0);
	VAO10.LinkVBO(VBO10, 1);
	VAO10.Unbind();
	VBO10.Unbind();
	EBO10.Unbind();

	vector<Model> collectibles;

	Model coll1(VAO4, VBO4, collectiblePoints[0]);
	Model coll2(VAO6, VBO6, collectiblePoints[1]);
	Model coll3(VAO7, VBO7, collectiblePoints[2]);
	Model coll4(VAO8, VBO8, collectiblePoints[3]);
	Model coll5(VAO9, VBO9, collectiblePoints[4]);
	Model coll6(VAO10, VBO10, collectiblePoints[5]);

	//npc
	VAO VAO5;
	VAO5.Bind();

	VBO VBO5(cubeVert, sizeof(cubeVert));
	EBO EBO5(cubeInd, sizeof(cubeInd));

	VAO5.LinkVBO(VBO5, 0);
	VAO5.LinkVBO(VBO5, 1);
	VAO5.Unbind();
	VBO5.Unbind();
	EBO5.Unbind();

	//house
	VAO VAO11;
	VAO11.Bind();

	VBO VBO11(wallVerts, sizeof(wallVerts));
	EBO EBO11(cubeInd, sizeof(cubeInd));

	VAO11.LinkVBO(VBO11, 0);
	VAO11.LinkVBO(VBO11, 1);
	VAO11.Unbind();
	VBO11.Unbind();
	EBO11.Unbind();

	VAO VAO12;
	VAO12.Bind();

	VBO VBO12(wallVerts2, sizeof(wallVerts2));
	EBO EBO12(cubeInd, sizeof(cubeInd));

	VAO12.LinkVBO(VBO12, 0);
	VAO12.LinkVBO(VBO12, 1);
	VAO12.Unbind();
	VBO12.Unbind();
	EBO12.Unbind();

	glEnable(GL_DEPTH_TEST);

	Camera camera(windowWidth, windowHeight, playerPos + glm::vec3(0.0f, 1.0f, 4.0f));
	Camera camera2(windowWidth, windowHeight, glm::vec3(2.5f, 0.5f, -25.0f));
	camera2.orientation = glm::rotate(camera2.orientation, glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	while (!glfwWindowShouldClose(window)) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glClearColor(0.533, 0.996, 1, 0.78);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();
		if (cam == 1) {
			camera.Matrix(shaderProgram, "cameraMatrix");
			camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
		}
		else if (cam == 2) {
			camera2.Matrix(shaderProgram, "cameraMatrix");
			camera2.UpdateMatrix(45.0f, 0.1f, 100.0f);
		}

		processInput(window, camera);

		VAO1.Bind();
		createShape(shaderProgram, glm::vec3(0.0f, -0.5f, -23.0f), glm::vec3(1.0f));
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		VAO2.Bind();
		createShape(shaderProgram, playerPos, glm::vec3(0.5f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);

		VAO3.Bind();
		createShape(shaderProgram, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f));
		glDrawElements(GL_TRIANGLES, sizeof(groundInd) / sizeof(int), GL_UNSIGNED_INT, 0);

		if (coll1.visible) {
			VAO4.Bind();
			glm::vec3 collPos1 = collectiblePoints[0];
			glm::mat4 collModel1 = glm::mat4(1.0f);
			collModel1 = glm::translate(collModel1, collPos1);
			collModel1 = glm::scale(collModel1, glm::vec3(0.25f, 0.5f, 0.25f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(collModel1));
			glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
			if (isColliding(playerPos, collPos1, 0.75f, 0.75f)) {
				VAO4.Delete();
				VBO4.Delete();
				coll1.visible = false;
			}
		}

		if (coll2.visible) {
			VAO6.Bind();
			glm::vec3 collPos2 = collectiblePoints[1];
			glm::mat4 collModel2 = glm::mat4(1.0f);
			collModel2 = glm::translate(collModel2, collPos2);
			collModel2 = glm::scale(collModel2, glm::vec3(0.25f, 0.5f, 0.25f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(collModel2));
			glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
			if (isColliding(playerPos, collPos2, 0.75f, 0.75f)) {
				VAO6.Delete();
				VBO6.Delete();
				coll2.visible = false;
			}
		}

		if (coll3.visible) {
			VAO7.Bind();
			glm::vec3 collPos3 = collectiblePoints[2];
			glm::mat4 collModel3 = glm::mat4(1.0f);
			collModel3 = glm::translate(collModel3, collPos3);
			collModel3 = glm::scale(collModel3, glm::vec3(0.25f, 0.5f, 0.25f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(collModel3));
			glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
			if (isColliding(playerPos, collPos3, 0.75f, 0.75f)) {
				VAO7.Delete();
				VBO7.Delete();
				coll3.visible = false;
			}
		}

		if (coll4.visible) {
			VAO8.Bind();
			glm::vec3 collPos4 = collectiblePoints[3];
			glm::mat4 collModel4 = glm::mat4(1.0f);
			collModel4 = glm::translate(collModel4, collPos4);
			collModel4 = glm::scale(collModel4, glm::vec3(0.25f, 0.5f, 0.25f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(collModel4));
			glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
			if (isColliding(playerPos, collPos4, 0.75f, 0.75f)) {
				VAO8.Delete();
				VBO8.Delete();
				coll4.visible = false;
			}
		}

		if (coll5.visible) {
			VAO9.Bind();
			glm::vec3 collPos5 = collectiblePoints[4];
			glm::mat4 collModel5 = glm::mat4(1.0f);
			collModel5 = glm::translate(collModel5, collPos5);
			collModel5 = glm::scale(collModel5, glm::vec3(0.25f, 0.5f, 0.25f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(collModel5));
			glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
			if (isColliding(playerPos, collPos5, 0.75f, 0.75f)) {
				VAO9.Delete();
				VBO9.Delete();
				coll5.visible = false;
			}
		}

		if (coll6.visible) {
			VAO10.Bind();
			glm::vec3 collPos6 = collectiblePoints[5];
			glm::mat4 collModel6 = glm::mat4(1.0f);
			collModel6 = glm::translate(collModel6, collPos6);
			collModel6 = glm::scale(collModel6, glm::vec3(0.25f, 0.5f, 0.25f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(collModel6));
			glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
			if (isColliding(playerPos, collPos6, 0.75f, 0.75f)) {
				VAO10.Delete();
				VBO10.Delete();
				VAO11.Bind();
				coll6.visible = false;
			}
		}

		VAO5.Bind();
		createShape(shaderProgram, npcFirstPath ? pathVerts1[npcind] : pathVerts2[npcind], glm::vec3(0.5f, 1.0f, 0.5f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
		if (npcind < pathVerts1.size() - 1 && npcind > 0) {
			npcind += npcReverse ? -1 : 1;
		}
		else {
			npcind += npcReverse ? 1 : -1;
			npcReverse = !npcReverse;
		}

		VAO11.Bind();
		glm::vec3 wall1Pos = glm::vec3(0.0f, 0.0f, -25.0f);
		glm::vec3 wall2Pos = glm::vec3(1.5f, 0.0f, -20.0f);
		glm::vec3 wall3Pos = glm::vec3(-1.5f, 0.0f, -20.0f);
		glm::vec3 wall4Pos = glm::vec3(0.0f, 2.0f, -20.01f);
		glm::vec3 wall5Pos = glm::vec3(2.5f, 0.0f, -22.5f);
		glm::vec3 wall6Pos = glm::vec3(-2.5f, 0.0f, -22.5f);

		createShape(shaderProgram, wall1Pos, glm::vec3(5.0f, 5.0f, 1.0f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
		createShape(shaderProgram, wall2Pos, glm::vec3(2.0f, 5.0f, 1.0f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
		createShape(shaderProgram, wall3Pos, glm::vec3(2.0f, 5.0f, 1.0f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
		createShape(shaderProgram, wall4Pos, glm::vec3(2.0f, 1.0f, 1.0f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);

		if (isColliding(playerPos, wall1Pos, 2.5f, 0.2f)) {
			playerPos[2] = playerPos[2] > wall1Pos[2] ? wall1Pos[2] + 0.2f : wall1Pos[2] - 0.2f;
			camera.position = playerPos + glm::vec3(0.0f, 1.0f, 4.0f);
		}

		if (isColliding(playerPos, wall2Pos, 1.25f, 0.2f)) {
			playerPos[2] = playerPos[2] > wall2Pos[2] ? wall2Pos[2] + 0.2f : wall2Pos[2] - 0.2f;
			camera.position = playerPos + glm::vec3(0.0f, 1.0f, 4.0f);
		}

		if (isColliding(playerPos, wall3Pos, 1.25f, 0.2f)) {
			playerPos[2] = playerPos[2] > wall3Pos[2] ? wall3Pos[2] + 0.2f : wall3Pos[2] - 0.2f;
			camera.position = playerPos + glm::vec3(0.0f, 1.0f, 4.0f);
		}

		if (isColliding(playerPos, wall5Pos, 0.2f, 2.5f)) {
			playerPos[0] = playerPos[0] > wall5Pos[0] ? wall5Pos[0] + 0.2f : wall5Pos[0] - 0.2f;
			camera.position = playerPos + glm::vec3(0.0f, 1.0f, 4.0f);
		}

		if (isColliding(playerPos, wall6Pos, 0.2f, 2.5f)) {
			playerPos[0] = playerPos[0] > wall6Pos[0] ? wall6Pos[0] + 0.2f : wall6Pos[0] - 0.2f;
			camera.position = playerPos + glm::vec3(0.0f, 1.0f, 4.0f);
		}

		glm::vec3 doorPos = glm::vec3(0.0f, 0.5f, -20.01f) + doorTranslation;
		glm::mat4 doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, doorPos);
		doorModel = glm::scale(doorModel, glm::vec3(1.0f, 2.0f, 1.0f));
		doorModel = glm::rotate(doorModel, glm::radians(doorRot), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(doorModel));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
		if (isColliding(playerPos, doorPos, 2.0f, 2.0f)) {
			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
				doorRot = 90.0f;
				doorTranslation = glm::vec3(-0.5f, 0.0f, -0.5f);
			}
		}
		if (isColliding(playerPos, doorPos, doorRot != 90.0f ? 0.5f : 0.2f, doorRot != 90.0f ?  0.2f : 0.5f)) {
			if (doorRot != 90.0f) {
				playerPos[2] = playerPos[2] > doorPos[2] ? doorPos[2] + 0.15f : doorPos[2] - 0.15f;
			}
			else {
				playerPos[0] = playerPos[0] > doorPos[0] ? doorPos[0] + 0.15f : doorPos[0] - 0.15f;
			}
			camera.position = playerPos + glm::vec3(0.0f, 1.0f, 4.0f);
		}
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);

		VAO12.Bind();
		createShape(shaderProgram, glm::vec3(2.5f, 0.0f, -22.5f), glm::vec3(1.0f, 5.0f, 5.0f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);
		createShape(shaderProgram, glm::vec3(-2.5f, 0.0f, -22.5f), glm::vec3(1.0f, 5.0f, 5.0f));
		glDrawElements(GL_TRIANGLES, sizeof(cubeInd) / sizeof(int), GL_UNSIGNED_INT, 0);

		VAO3.Bind();
		createShape(shaderProgram, glm::vec3(0.0f, 2.5f, -22.5f), glm::vec3(5.0f, 1.0f, 5.0f));
		glDrawElements(GL_TRIANGLES, sizeof(groundInd) / sizeof(int), GL_UNSIGNED_INT, 0);

		if (isColliding(playerPos, glm::vec3(0.0f, 0.5f, -22.01f), 1.0f, 1.0f)) {
			cam = 2;
		}
		if (isColliding(playerPos, glm::vec3(0.0f, 0.5f, -19.01f), 1.0f, 1.0f)) {
			cam = 1;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	VAO2.Delete();
	VBO2.Delete();
	VAO3.Delete();
	VBO3.Delete();
	VAO4.Delete();
	VBO4.Delete();
	VAO5.Delete();
	VBO5.Delete();
	VAO6.Delete();
	VBO6.Delete();
	VAO7.Delete();
	VBO7.Delete();
	VAO8.Delete();
	VBO8.Delete();
	VAO9.Delete();
	VBO9.Delete();
	VAO10.Delete();
	VBO10.Delete();
	shaderProgram.Delete();
	glfwTerminate();

	return 0;
};