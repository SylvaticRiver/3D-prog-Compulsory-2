#include "Model.h"

Model::Model(VAO& vao, VBO& vbo, glm::vec3 pos)
{
	Model::vao = vao;
	Model::vbo = vbo;
	Model::pos = pos;
	this->visible = true;
}

void Model::unrender()
{
	vao.Delete();
	vbo.Delete();
	this->visible = false;
}

glm::vec3 Model::getPos()
{
	return this->pos;
}
