#pragma once
#include "Shaders.h"
#include "HelperStructs.h"
#include "../Utilities/utilities.h"
class Shader
{
private:
	ShaderResource sr;
	Shaders s;
	GLuint id;
public:
	Shaders getShader();
	GLvoid load();
	Shader(GLuint);
	~Shader();
};

