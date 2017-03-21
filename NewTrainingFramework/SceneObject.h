#pragma once
//resources that an object contains
#include <memory>
#include <vector>

#include "Model.h"
#include "Texture.h"
#include "Shader.h"


#include "../Utilities/Math.h"
#include "../Utilities/utilities.h" // for stl
/*
	SceneObjects class represents an object in the scene and it can be Normal, Skybox, Terrain, etc.
*/
class SceneObject
{
protected:
	
	// unique id for the object
	GLuint id;

	// position, rotation and scale of the object to build the Model matrix

	Vector3 rotation;
	Vector3 position;
	Vector3 scale;
	Vector3 initialPosition;

	// model matrix
	Matrix modelMatrix;

	// an objects is made from a single model
	std::shared_ptr< Model > model;

	// an object can have multiple textures applied to it
	std::vector< std::shared_ptr< Texture > > textures;


	// not sure what it is for the moment
	GLboolean depthTest, blend;
	// self rotation
	GLfloat selfRotateSpeed;

	// object type normal, terrain, skybox etc, we will inherit the SceneObject class anyway
	ObjectType type;

	// display format weird or normal
	WiredFormat wiredFormat;
	
	// object name
	std::string name;
	// to draw a certain texture
	GLvoid activeTextureUnitWithId(GLuint id);
	// update the model matrix from the position, rotation , scale vectors
	GLvoid updateModelMatrix();

	TrajectoryInfo trajectoryInfo; // we assume an object has just one trajectory
	// conversion from angles to radians
	double toRadians(double angle)
	{
		return angle*PI / 180.0;
	}
	// lights ids associated
	std::vector<GLuint> lightsId;
	// light coeficients
	GLfloat diffCoef, specCoef;
public:
	// an object can have only a shader
	std::shared_ptr< Shader >shader;
	// draw the object
	virtual GLvoid draw();

	// update the object status
	virtual GLvoid update();
	// collision between 2 objects
	virtual GLboolean collides(std::shared_ptr<SceneObject>  other);
	GLboolean collides(AABB other);

	// ctr
	SceneObject();
	// this constructor is used by terrain object
	SceneObject(
		ObjectType _objectType,
		std::shared_ptr<Model> _model,
		std::shared_ptr<Shader> _shader,
		std::vector<std::shared_ptr<Texture>> _textures,
		std::string _name,
		WiredFormat _wiredFormat,
		std::vector<GLuint> _lightsId,
		GLfloat _difCoef,
		GLfloat _specCoef
	);
	// this constructor is used by skybox object
	SceneObject(
		ObjectType _objectType,
		std::shared_ptr<Model> _model,
		std::shared_ptr<Shader> _shader,
		std::vector<std::shared_ptr<Texture>> _textures,
		std::string _name,
		Vector3 _scale
	);
	// this constructor is used by fire object
	SceneObject(ObjectType _objectType,
		std::shared_ptr<Model> _model,
		std::shared_ptr<Shader> _shader,
		std::vector<std::shared_ptr<Texture>> _textures,
		Vector3 _position,
		Vector3 _initialPosition,
		Vector3 _scale,
		Vector3 _rotation,
		std::string _name,
		WiredFormat _wiredFormat,
		TrajectoryInfo _trajectoryInfo
	);
	// this constructor is used by default
	SceneObject(ObjectType _objectType, 
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
		std::vector<GLuint> _lightsId,
		GLfloat _difCoef,
		GLfloat _specCoef);
	// dtr
	~SceneObject();
};

