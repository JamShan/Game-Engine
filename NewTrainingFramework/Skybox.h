#pragma once
#include "SceneObject.h"

#include "HelperStructs.h"
class Skybox : public SceneObject
{
public:
	GLfloat offsetOY;
	// draw the skybox
	virtual GLvoid draw();

	// update the skybox
	virtual GLvoid update();
	Skybox() {};
	Skybox(
		ObjectType _objectType,
		std::shared_ptr<Model> _model,
		std::shared_ptr<Shader> _shader,
		std::vector<std::shared_ptr<Texture>> _textures,
		std::string _name,
		Vector3 _scale
	); 
	~Skybox();
};

