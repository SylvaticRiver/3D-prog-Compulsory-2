#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
	GLfloat* pos;
	GLfloat* color;
};

class VBO {
public:
	unsigned int ID;
	VBO();
	VBO(GLfloat* verticies, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();

};

#endif

