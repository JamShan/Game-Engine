#include "stdafx.h"
#include "Shader.h"

#include <cstring>

#include "ResourceManager.h"

Shaders Shader::getShader()
{
	return s;
}

GLvoid Shader::load()
{
	std::string vepath("../"),frpath("../");
	vepath += sr.vsPath;
	frpath += sr.fsPath;
	char * fileVertexShader, *fileFragmentShader;
	fileVertexShader   = new char[260];
	strcpy(fileVertexShader,vepath.c_str());
	fileFragmentShader = new char[260];
	strcpy(fileFragmentShader,  frpath.c_str());

	if (s.Init(fileVertexShader, fileFragmentShader) != 0)
		throw "Failed to init shader";
	delete[]fileFragmentShader;
	delete[]fileVertexShader;
}

Shader::Shader(GLuint _id) :id(_id)
{
	sr = ResourceManager::getInstance()->shaderResources[id];
}


Shader::~Shader()
{
	s.~Shaders();
}
