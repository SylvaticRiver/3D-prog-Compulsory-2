#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "Shader.h"

class Model {
public:
	glm::vec3 pos;
	VAO vao;
	VBO vbo;
	bool visible;

	Model(VAO& vao, VBO& vbo, glm::vec3 pos);
	void unrender();
	glm::vec3 getPos();
};

#endif

