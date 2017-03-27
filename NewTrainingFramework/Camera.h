#pragma once
#include <memory>

#include "Shader.h"
#include "HelperStructs.h"

#include "../FMOD/fmod.hpp"

#include "../Utilities/Math.h"
class Camera
{
	AABB boundingBox;
	const GLfloat _1Step = 300;
	const GLfloat _100Step = _1Step * 100;
	GLfloat _1CurStep = 0,
			_100CurStep = 0;
	
private:
	Matrix R, T;				
	Vector3
		Xaxis,					// camera X axis
		Yaxis,					// camera Y axis
		Zaxis;					// camera Y axis
	Matrix
		viewMatrix,				// view matrix that converts from 
		projectionMatrix,		// projection matrix usualy perspective 
		MVP,					// ModelViewProjection matrix
		worldMatrix;			// world matrix
	GLfloat
		rotateSpeed,			// rotation speed 
		moveSpeed,				// move speed
		nearPlane,				// near plane
		farPlane,				// far plane			
		aspect,					// screen ration width/height
		fov,					// field of view between 30 and 90
		dt;
	Vector3 target, position, localUp;	// target of the camera, position of the camera , localUp vector of the camera(0,1,0)
	Vector4 up;							// up vector of the camera, this can change
	void updateWorldView();
	
	// mouse related rotations 
	POINT prevMousePos; //previous mouse position
	const POINT mouseRotationStep = {1,1};
	// we use these values to know how much we rotated the camera to the right/left or 
	// up/down and if it's bigger then some specified max we clamp it
	LONG xRotation, yRotation;
	// the angles where we clamp the rotations for rotations around the X and Y axes
	const LONG maxxRotation = 90, maxyRotation = 90;
	
	GLfloat toRadians(GLfloat angle);
public:
	//////////////////////////////////////////
	// debug stuff
	// debug axes data
	Vector3 X = Vector3(100,0, 100), Y =Vector3(0,100,100), Z = Vector3(0,0,100+100), O;
	GLuint VboObjaAxes;
	std::shared_ptr<Shader> axesShader;
	std::vector<Vector3> axesColors;
	GLvoid createAxes();
	GLvoid drawAxes();

	// here we handle the mouse events for zoom in/out rotate X/Y axis and click Left/Right
	GLvoid handleMouseEvent(POINT p);
	// setters
	GLvoid setDeltaTime(GLfloat);
	GLvoid setRotationSpeed(GLfloat);
	GLvoid setMoveSpeed(GLfloat);
	GLvoid setPosition(Vector3);
	GLvoid setTarget(Vector3);
	// getters
	GLfloat getRotationSpeed();
	GLfloat getMoveSpeed();
	Vector3 getPositon();
	Vector3 getTarget();
	Matrix& getMVP();
	Matrix& getProjectionMatrix();
	Matrix& getViewMatrix();
	GLfloat getDeltaTime();
	AABB	getAABB();

	GLvoid moveOz(GLint);				// move on Oz axis
	GLvoid moveOx(GLint);				// move on Ox axis	
	GLvoid moveOy(GLint);				// move on Oy axis

	
	
	GLvoid rotateOx(GLint);			// rotate on Ox axis
	GLvoid rotateOy(GLint);			// rotate on Oy axis
	GLvoid rotateOz(GLint);			// rotate on Oz axis
	
	// ctor /dtor
	Camera(
		Vector3 _position,
		Vector3 _target,
		Vector3 _localUp,
		GLfloat moveSpeed,
		GLfloat rotateSpeed,
		GLfloat _farPlane,
		GLfloat _nearPlane,
		GLfloat _fov
	);

	
	
	~Camera();
};

