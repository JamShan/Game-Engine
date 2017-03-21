#pragma once
#include "SceneObject.h"


class FireObject : public SceneObject
{
	GLfloat u_DispMax;
	GLfloat time;
public:
	
	FireObject(ObjectType _objectType,
		std::shared_ptr<Model> _model,
		std::shared_ptr<Shader> _shader,
		std::vector<std::shared_ptr<Texture>> _textures,
		Vector3 _position,
		Vector3 _initialPosition,
		Vector3 _scale,
		Vector3 _rotation,
		std::string _name,
		WiredFormat _wiredFormat,
		TrajectoryInfo _trajectoryInfo,
		GLfloat _u_DispMax);
	virtual GLvoid draw();
	virtual GLvoid update();
	 ~FireObject();
};

