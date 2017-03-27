#include "stdafx.h"
#include "Camera.h"

#include <iostream>
#include <vector>

#include "Globals.h"
#include "ResourceManager.h"
#include "HelperStructs.h"

#include "../Utilities/utilities.h"
Camera::Camera(
	Vector3 _position, 
	Vector3 _target, 
	Vector3 _localUp, 
	GLfloat moveSpeed, 
	GLfloat rotateSpeed, 
	GLfloat _farPlane, 
	GLfloat _nearPlane,
	GLfloat _fov):
	position(_position),
	target(_target),
	localUp(_localUp),
	up(_localUp),
	moveSpeed(moveSpeed),
	rotateSpeed(rotateSpeed),
	farPlane(_farPlane),
	nearPlane(_nearPlane),
	aspect((GLfloat)Globals::screenWidth / (GLfloat)Globals::screenHeight),
	fov(_fov), // between 30 and 90
	xRotation(0),//we intialy don't have rotations on the X axis
	yRotation(0), //we intialy don't have rotations on the Y axis
	prevMousePos{0,0} // we assume initialy the cursor is at position 0,0
{
	ResourceManager::getInstance()->loadSound(Sounds::MOVE_UP);
	ResourceManager::getInstance()->loadSound(Sounds::_100STEPS);
	dt = 0;
	projectionMatrix.SetPerspective(fov, aspect, nearPlane, farPlane);
	updateWorldView();
	boundingBox.Xmax = boundingBox.Ymax =  boundingBox.Zmax = 1;
	boundingBox.Xmin = boundingBox.Ymin = boundingBox.Zmin = -1;

}

AABB Camera::getAABB()
{
	AABB box = boundingBox;
	box.Xmax += position.x;
	box.Xmin += position.x;
	box.Ymin += position.y;
	box.Ymax += position.y;
	box.Zmax += position.z;
	box.Zmin += position.z;
	return box;
}

GLvoid Camera::moveOz(GLint directie)//oz 
{
	Vector3 deplasare = -(target - position).Normalize()* dt * moveSpeed * directie;
	position += deplasare;
	target += deplasare;
	updateWorldView();
	// sound play after each step
	_1CurStep += abs(deplasare.z);
	_100CurStep += abs(deplasare.z);
}

GLvoid Camera::moveOx(GLint directie)
{
	Vector4 oz = -(target - position).Normalize();
	Vector4 ox = oz.Cross(up).Normalize()* dt * moveSpeed * directie;
	position += ox.toVector3();
	target += ox.toVector3();
	updateWorldView();
	// sound play after each step
	_1CurStep += abs(ox.x);
	_100CurStep += abs(ox.x);
}

GLvoid Camera::moveOy(GLint directie)
{
	Vector4 oz = -(target - position).Normalize();
	Vector4 ox = oz.Cross(up).Normalize();
	Vector4 oy = ox.Cross(oz).Normalize()* dt * moveSpeed * directie;
	position += oy.toVector3();
	target += oy.toVector3();
	updateWorldView();
}

GLfloat Camera::toRadians(GLfloat angle)
{
	return angle*PI / 180.0;
}

GLvoid Camera::rotateOy(GLint directie)
{
	Matrix mRotateOY;
	mRotateOY.SetRotationY(toRadians(dt * rotateSpeed) * directie);

	Vector4 localTarget = Vector4(0.0f, 0.0f, -(target - position).Length(), 1.0f);
	Vector4 rotatedTarget = localTarget * mRotateOY ;
	       
	target = (rotatedTarget * worldMatrix).toVector3();
	
	updateWorldView();
}

GLvoid Camera::rotateOx(GLint directie)
{
	Matrix mRotateOx;
	mRotateOx.SetRotationX(toRadians(dt * rotateSpeed) * directie);

	Vector4 rotatedLocalUp = Vector4(localUp.x, localUp.y, localUp.z, 0) * mRotateOx;
	up = rotatedLocalUp * worldMatrix;
	up = up.Normalize();
	Vector4 localTarget = Vector4(0.0f, 0.0f, -(target - position).Length(), 1.0f);
	Vector4 rotatedTarget = localTarget * mRotateOx;
	target = (rotatedTarget * worldMatrix).toVector3();
	updateWorldView();
}

GLvoid Camera::rotateOz(GLint directie)
{
	Matrix mRotateOz;
	mRotateOz.SetRotationZ(toRadians(dt * rotateSpeed) * directie);

	Vector4 rotatedLocalUp = Vector4(localUp.x, localUp.y, localUp.z, 0)* mRotateOz;
	up = rotatedLocalUp * worldMatrix;
	up = up.Normalize();

	updateWorldView();
}

GLvoid Camera::updateWorldView()
{
	R.SetZero();
	Zaxis = (position - target).Normalize();
	Yaxis = up.toVector3().Normalize();
	Xaxis = (Zaxis.Cross(Yaxis)).Normalize();
	R.m[0][0] = Xaxis.x; R.m[0][1] = Xaxis.y; R.m[0][2] = Xaxis.z;
	R.m[1][0] = Yaxis.x; R.m[1][1] = Yaxis.y; R.m[1][2] = Yaxis.z;
	R.m[2][0] = Zaxis.x; R.m[2][1] = Zaxis.y; R.m[2][2] = Zaxis.z;
	R.m[3][3] = 1;
	T = T.SetTranslation(position);
	worldMatrix = R*T; // Cam = (World)^-1 => (RT)^-1 = T^-1 * R^-1  

	viewMatrix = T.SetTranslation(-position)*R.Transpose(); // T^-1*R^-1
	//we also update the mvp matrix
	MVP = viewMatrix*projectionMatrix;
	if (_1CurStep > _1Step)
	{
		_1CurStep = 0;
		ResourceManager::getInstance()->playSound(Sounds::MOVE_UP);
	}
	if (_100CurStep > _100Step)
	{
		_100CurStep = 0;
		ResourceManager::getInstance()->playSound(Sounds::_100STEPS);
	}

	
}

Camera::~Camera()
{
}

GLvoid Camera::createAxes()
{
	axesShader = ResourceManager::getInstance()->loadShader(3);
	std::vector<Vector3> objAxesdata;
	O = target;
	objAxesdata.push_back(O);
	objAxesdata.push_back(axesColors[0]);
	objAxesdata.push_back(X+O);
	objAxesdata.push_back(axesColors[0]);
	objAxesdata.push_back(O);
	objAxesdata.push_back(axesColors[1]);
	objAxesdata.push_back(Y+O);
	objAxesdata.push_back(axesColors[1]);
	objAxesdata.push_back(O);
	objAxesdata.push_back(axesColors[2]);
	objAxesdata.push_back(Z+O);
	objAxesdata.push_back(axesColors[2]);
	// buffer for vertexes
	glGenBuffers(1, &VboObjaAxes);
	glBindBuffer(GL_ARRAY_BUFFER, VboObjaAxes);
	glBufferData(GL_ARRAY_BUFFER, objAxesdata.size()* sizeof(Vector3), objAxesdata.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLvoid Camera::drawAxes()
{
	
	Shaders sh = axesShader.get()->getShader();

	glUseProgram(sh.program);

	glBindBuffer(GL_ARRAY_BUFFER,VboObjaAxes);

	// we link the position attribute from  vertex shader
	if (sh.positionAttribute != -1)
	{
		glEnableVertexAttribArray(sh.positionAttribute);
		glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Vector3), 0);
	}

	// we link the color attribute from vertex shader
	if (sh.colorAttribute != -1)
	{
		glEnableVertexAttribArray(sh.colorAttribute);
		glVertexAttribPointer(sh.colorAttribute, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Vector3), (void*)sizeof(Vector3));
	}


	// we link the uniform MVP matrix from vertex shader
	Matrix mvp = projectionMatrix;
	if (sh.u_MVP != -1)
	{
		glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
	}
	// and the draw triangles
	glDrawArrays(GL_LINES,0,6);
	// close the array buffer 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLvoid Camera::handleMouseEvent(POINT p)
{
	//if (prevMousePos.x != p.x || prevMousePos.y !=p.y)
		//std::cout << " old = (" << prevMousePos.x << " , " << prevMousePos.y << ") new = (" << p.x << " , " << p.y << ")\n";
	// can we rotate the Y axis
	LONG oy = p.x - prevMousePos.x;
	GLint oysign = (oy > 0) ? 1 : -1; // we take the sign so we know in witch direction we rotate
	if (oysign*oy > mouseRotationStep.y)// if we moved the mouse coursor enough to be consider as a rotation
	{
		if (oysign*yRotation < maxyRotation) // if we can still rotate
		{
			rotateOy(-oysign*mouseRotationStep.y);
			prevMousePos.x = p.x;
			yRotation += oysign;
		}
	}
	// can we rotate the X axis
	LONG ox = p.y - prevMousePos.y;
	GLint oxsign = (ox > 0) ? 1 : -1; // we take the sign so we know in witch direction we rotate
	if (oxsign*ox > mouseRotationStep.x)// if we moved the mouse coursor enough to be consider as a rotation
	{
		if (oxsign* xRotation < maxxRotation) // if we can still rotate
		{
			rotateOx(-oxsign*mouseRotationStep.x);
			prevMousePos.y = p.y;
			xRotation += oxsign;
		}
	}

}

GLvoid Camera::setDeltaTime(GLfloat dt)
{
	this->dt = dt;
}

GLvoid Camera::setRotationSpeed(GLfloat speed)
{
	this->rotateSpeed = speed;
}

GLvoid Camera::setMoveSpeed(GLfloat speed)
{
	moveSpeed = speed;
}

GLvoid Camera::setPosition(Vector3 pos)
{
	position = pos;
}

GLvoid Camera::setTarget(Vector3 tar)
{
	target = tar;
}

GLfloat Camera::getRotationSpeed()
{
	return rotateSpeed;
}

GLfloat Camera::getMoveSpeed()
{
	return moveSpeed;
}

Vector3 Camera::getPositon()
{
	return position;
}

Vector3 Camera::getTarget()
{
	return target;
}

Matrix& Camera::getMVP()
{
	return MVP;
}

Matrix & Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

Matrix & Camera::getViewMatrix()
{
	return viewMatrix;
}

GLfloat Camera::getDeltaTime()
{
	return this->dt;
}
