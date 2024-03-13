#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include "glad/glad.h"

class EBO {
public:
	unsigned int ID;
	EBO(GLuint* indicies, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();

};

#endif



