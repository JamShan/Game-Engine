#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "../Utilities/Math.h"
#include "../../Utilities/utilities.h"

//this is used to load texture resource and send addition parameters trough loadResource method
struct TextureResourceProperties
{
	GLuint minFilter, magFilter, wrapS, wrapT;
	std::string type;
	TextureResourceProperties() {}
	TextureResourceProperties(std::string miF, std::string maF, std::string wS, std::string wT,std::string _type) : type(_type)
	{
		minFilter = (miF == "LINEAR") ? GL_LINEAR : GL_NEAREST;
		magFilter = (maF== "LINEAR") ? GL_LINEAR : GL_NEAREST;
		wrapS = (wS == "CLAMP_TO_EDGE") ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		wrapT = (wT == "CLAMP_TO_EDGE") ? GL_CLAMP_TO_EDGE : GL_REPEAT;
	}
};
// resources that are read from ResourceManager.xml configuration file
struct ShaderResource
{
	GLuint id;
	std::string fsPath, vsPath;
	ShaderResource() {}
	ShaderResource(GLuint _id, std::string _vspath, std::string _fspath) : id(_id), vsPath(_vspath), fsPath(_fspath) {}
};
// resources that are read from ResourceManager.xml configuration file
struct TextureResource
{
	GLuint id;
	std::string path;
	TextureResource() {}
	TextureResource(GLuint _id, std::string _path) : id(_id), path(_path) {}
};
// resources that are read from ResourceManager.xml configuration filet
struct SoundResource
{
	GLuint id;
	std::string path;
	SoundResource() {}
	SoundResource(GLuint _id, std::string _path) : id(_id), path(_path) {}
};
// resources that are read from ResourceManager.xml configuration file
struct ModelResource
{
	GLuint id;
	std::string path;
	ModelResource() {}
	ModelResource(GLuint _id, std::string _path) : id(_id), path(_path) {}
};
// used in loadResource method
enum class ObjectType
{
	Normal,
	Skybox,
	Terrain,
	Fire
};
// wired format or not
enum WiredFormat
{
	Normalf,
	Wiredf
};
// Axis aligned bounding box
struct AABB
{
	GLfloat Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
	GLboolean collides(AABB other)
	{
		return	
			Xmin <= other.Xmax && Xmax >= other.Xmin &&
			Ymin <= other.Ymax && Ymax >= other.Ymin &&
			Zmin <= other.Zmax && Zmax >= other.Zmin;
	}
};
// debug stuff
struct DebugSettings
{
	GLboolean on;
	struct Axes
	{
		Vector3 OXcolor, OYcolor, OZcolor;
	} objectAxes, camAxes;
};
// fog
struct Fog
{
	Vector3 color;
	GLfloat r, R;
};
// Sounds
enum Sounds
{
	// sound name and id from resource manager
	MOVE_UP = 2,
	MOVE_DOWN = 2,
	MOVE_LEFT = 2,
	MOVE_RIGHT = 2,
	_100STEPS = 1,
	COLLISION=3,
	DESTROYED_A,
	DESTROYED_BY_A
};
// light
struct Light
{
	GLuint associatedOject;
	Vector3 diffuseColor, specularColor;
	// if type = directiional we use direction
	// if type = point we use direction and point
	// if type = cone we use direction, position and angle
	Vector3 position, direction;
	GLfloat angle;
	GLfloat shiness;
	enum Type
	{
		Directional =0,
		Point = 1,
		Spot = 2,
		Area = 3
	} type;
	
};
// ambliental light
struct AmbientalLight
{
	Vector3 color;
	GLfloat ratio;
};
// Trajectory structs
struct Trajectory
{
	enum Type :GLbyte
	{
		Linear = 0,
		LineStrip = 1,
		LineLoop = 2,
		Circle = 3,
		Nr = 4
	};
	Type type;
	virtual GLvoid update(GLfloat) = 0;
	virtual GLboolean finished()const = 0;
	virtual Vector3 getPosition() = 0;
	virtual GLvoid reverse() {};
	virtual GLvoid reset() {};
};
// Trajectory struct that keeps the information related to an object's trajectory
struct TrajectoryInfo
{
	Vector3 position;
		
	GLfloat speed, radius; // speed of the object and radius if circle
	
	Vector3 endPoint;		// point of destination for linear

	std::vector<Vector3> points; // points if trajectory is line_strip or line_loop

	GLboolean has,			// if the object has trajectory
			  infinite;		// iteration if it's infinity or not


	GLfloat	  iterStep;		// how many times the iteration is repeated if it's not infinite

	std::string type,		// type of trajectory line, line_loop, circular, line_strip, etc
		        direction;			// direction normal or alternate
	Vector3 rotationPlane;
	Matrix mrotateY180;// matrix for alternate direction if trajectory is linear or line strip
	// a pointer to an object that is our type of trajectory with all the necesary functions
	Trajectory * trajectory;
	TrajectoryInfo()
	{
		mrotateY180.SetIdentity();
	}
	Vector3 getPosition()const
	{
		return position;
	}
	void execute(GLfloat dt)
	{

		if (infinite == GL_TRUE) // if it's infinite we set iteration step each time to 1 so it enters every time in if statement
			iterStep = 1;
		if (iterStep > 0)
		{
			if (!trajectory->finished())
			{
				trajectory->update(speed*dt);
				position = trajectory->getPosition();
			}
			else // trajectory finished so we need to turn 180 degreees (rotated 180 degrees around Y axis) if linear or linestrip
			{
				// we have direction for linear or linestrip trajectories
				if (trajectory->type == Trajectory::Type::Linear ||
					trajectory->type == Trajectory::Type::LineStrip)
				{
					// here we handle rotation
					if (direction == "normal")
						trajectory->reset();
					else// alternate
					{
						Matrix rotate;
						rotate.SetRotationY(PI);
						mrotateY180 = mrotateY180 * rotate;
						trajectory->reverse();
					}
				}
				else// line loop or circle
				{
					trajectory->reset();
				}
				iterStep--;
			}
		}
	}
			
};
struct LinearTrajectory : public Trajectory
{
	LinearTrajectory& operator = (const LinearTrajectory& other)
	{
		speed = other.speed;
		begin = other.begin;
		end = other.end;
		direction = other.direction;
		position = other.position;
		length = other.length;
		distance = other.distance;
		return *this;
	}
	GLfloat speed;
	Vector3 begin, end, direction,position;
	GLfloat length;
	GLfloat distance; // this represents how much distance has been already made from begin to end point
	LinearTrajectory(Vector3 _begin, Vector3 _end) :
		begin(_begin), end(_end),
		length((_end - _begin).Length()),
		direction((_end - _begin).Normalize()),
		distance(0),
		position(begin) 
	{
		type = Type::Linear;
	};
	GLvoid reset()override
	{
		distance = 0;
		position = begin;
	}
	GLvoid update(GLfloat dt)override
	{
		distance += dt;
		position += direction*dt;
	}
	GLboolean finished() const override
	{
		return distance >= length;
	}
	Vector3 getPosition() override
	{
		return position;
	};
	// reverse the direction and start again this is called if direction = alternate
	GLvoid reverse()override
	{
		distance = 0;
		Vector3 t = begin;
		begin = end;
		end = t;
		position = begin;
		direction = (end - begin).Normalize();
	}
};
struct LineStripTrajectory : public Trajectory
{
	std::vector<LinearTrajectory> lines;
	// current line we are at
	GLuint lineIndex;
	LineStripTrajectory(std::vector<Vector3> points):lineIndex(0)
	{
		type = Type::LineStrip;
		for (int i = 0; i < points.size() - 1; i++)
			lines.push_back(LinearTrajectory(points[i], points[i + 1]));
			
	}
	GLvoid reset()override
	{
		lineIndex = 0;
		for (int i = 0; i < lines.size(); i++)
			lines[i].reset();
	}
	GLvoid update(GLfloat dt)override
	{
		if (lines[lineIndex].finished())
			lineIndex = (lineIndex + 1) % lines.size();
		lines[lineIndex].update(dt);
	}
	GLboolean finished() const override
	{
		for (int i = 0; i <lines.size(); i++)
			if (!lines[i].finished())
				return GL_FALSE;
		return GL_TRUE;
	}
	Vector3 getPosition()override
	{
		return lines[lineIndex].getPosition();
	}
	// this is called if direction = alternate
	GLvoid reverse()override
	{
		lineIndex = 0;
		for(GLuint i = 0 ; i<lines.size();i++)
			lines[i].reverse();
		std::reverse(lines.begin(), lines.end());
	}
};
struct LineLoopTrajectory : public Trajectory
{
	std::vector<LinearTrajectory> lines;
	// current line we are at
	GLuint lineIndex;
	LineLoopTrajectory(std::vector<Vector3> points) :lineIndex(0)
	{
		type = Type::LineLoop;
		for (int i = 0; i < points.size() - 1; i++)
			lines.push_back(LinearTrajectory(points[i], points[i + 1]));
		// a line strip and we close the loop
		lines.push_back(LinearTrajectory(points[points.size() - 1], points[0]));
	}
	GLvoid reset()override
	{
		for (GLuint i = 0; i<lines.size(); i++)
			lines[i].reset();
		lineIndex = 0;
	}
	GLvoid update(GLfloat dt)override
	{
		if (lines[lineIndex].finished())
			lineIndex = (lineIndex + 1) % lines.size() ;
		lines[lineIndex].update(dt);
	}
	GLboolean finished()const override
	{
		for(GLuint i = 0 ; i <lines.size();i++)
			if (!lines[i].finished())
				return GL_FALSE;
		return GL_TRUE;
	}
	Vector3 getPosition()override
	{
		return lines[lineIndex].getPosition();
	}
};
struct CircleTrajectory : public Trajectory
{
	Vector3 center;//center of the circle 
	Vector3 position;
	GLfloat radius;
	Matrix rotVec;
	GLfloat maxAngle,curAngle;
	CircleTrajectory(
		// circle center
		Vector3 _center,
		// rotation angles X Y Z 
		GLfloat _rotX,
		GLfloat _rotY,
		GLfloat _rotZ,
		
		// circle radius
		GLfloat _radius):
		center(_center),
		radius(_radius),
		maxAngle(360),
		curAngle(0)
	{
		type = Type::Circle;
		Matrix Rz, Ry, Rx;
		Rz.SetRotationZ(_rotZ*PI/180.0);
		Ry.SetRotationY(_rotY*PI/180.0);
		Rx.SetRotationX(_rotX*PI/180.0);
		rotVec = Rz * Ry * Rx;
		
	}
	GLvoid update(GLfloat dt)override
	{
		curAngle += dt;
		GLfloat degInRad = curAngle * PI / 180.0;
		position.x = center.x + radius*cos(degInRad);
		position.y = center.y + radius*sin(degInRad);
	}
	GLvoid reset()
	{
		curAngle = 0;
	}
	GLboolean finished()const override
	{
		return curAngle >= maxAngle;
	}
	Vector3 getPosition() override
	{
		return position;
	}
};