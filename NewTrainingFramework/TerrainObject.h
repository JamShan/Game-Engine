#pragma once
#include "SceneObject.h"

class TerrainObject : public SceneObject
{
	GLfloat Xcenter, Zcenter;
	GLint sign(GLfloat d);
	Vector3 height;
public:
	GLfloat oneOverN;
	GLint nrCells, cellWidth;
	// initialization
	GLvoid Init();
	// virtual data
	virtual void draw();
	virtual void update();
	// constructor
	TerrainObject(
		ObjectType _objectType,
		std::shared_ptr<Model> _model,
		std::shared_ptr<Shader> _shader,
		std::vector<std::shared_ptr<Texture>> _textures,
		std::string _name,
		WiredFormat _wiredFormat,
		Vector3 _height,
		GLfloat _cellWidth,
		GLfloat _nrceclls,
		GLfloat _oneOverN,
		std::vector<GLuint> _lightsId,
		GLfloat _difCoef,
		GLfloat _specCoef
	);
	~TerrainObject();
};

