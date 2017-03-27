#pragma once
#include "../rapidxml-1.13/rapidxml.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

#include "FireObject.h"
#include "TerrainObject.h"
#include "Skybox.h"
#include "Camera.h"
#include "HelperFunctions.h"
#include "../Utilities/utilities.h"
/*
	This class manages the objects in the scene, it parses the SceneManager.xml config
	file and creates the objects, cameras, etc.
*/
class SceneManager
{
private:

	// not sure if these pointers to scene objects and camera should be shared or unique
	using spso = std::shared_ptr<SceneObject>;
	using spto = std::shared_ptr<TerrainObject>;
	using spfo = std::shared_ptr<FireObject>;
	using spsb = std::shared_ptr<Skybox>;
	using spca = std::shared_ptr<Camera>;
	
	
	// pointer to the class instance
	static SceneManager* instance;
	
	// constructor
	SceneManager();

	// type of the scene object 
	ObjectType getType(const GLchar * s);

	// load a objects into the scene
	GLvoid loadNormalObject(rapidxml::xml_node<>* &pObject);
	GLvoid loadFireObject(rapidxml::xml_node<>*& pObject);
	GLvoid loadSkyboxObject(rapidxml::xml_node<>* &pObject);
	GLvoid loadTerrainObject(rapidxml::xml_node<>* &pObject);
	// id of the main camera
	GLuint mainCamera;

	// here we store the cameras with their id
	std::unordered_map<GLuint, spca> cameras;

	// here we store the objects from our scene
	std::vector<spso> objects;

	// light data
	// here we store the lights from our scene
	std::unordered_map<GLuint, std::shared_ptr<Light>> lights;
	///////////////////////////////////////////////////////////////////
	// fog data
	Fog fog;


	// ambiental light
	AmbientalLight ambientalLight;

	
	// background color 
	Vector3 backgroundColor;

	// normals
	GLfloat normalLength;
	Vector3 normalColor;
public:
	// debug axes
	DebugSettings debugSettings;
	
	
	// getters
	// get a reference to a map of pointers to light
	std::unordered_map<GLuint, std::shared_ptr<Light>>& getLights();
	// get an instance to the class 
	static SceneManager*getInstance();
	// get a pointer to main camera
	spca				getMainCamera();
	AmbientalLight		getAmbientalLight()	const;
	Fog					getFog()			const;
	Vector3				getBackgroundColor()const;
	GLfloat				getNormalLength()	const;
	Vector3				getNormalColor()	const;
	// data from SceneManager.xml configuration file
	GLvoid init();
	
	// draw the current frame 
	GLvoid draw();
	
	// update the scene status
	GLvoid update();

	// destructor
	~SceneManager();
private:
	/////////////////////////////////////////////////
	// helpers for parsing
	Vector3 getVector3(const rapidxml::xml_node<>* pObject)
	{
		const char * const xyz[] = { "x","y","z" };
		GLfloat v[3];
		for (int i = 0; i < 3; i++)
			v[i] = helperfunctions::stringToNumber<GLfloat>(pObject->first_node(xyz[i])->value());
		return Vector3(v);
	};
	Vector3 getVector3rgb(const rapidxml::xml_node<>* pObject)
	{
		const char * const rgb[] = { "r","g","b" };
		GLfloat v[3];
		for (int i = 0; i < 3; i++)
			v[i] = helperfunctions::stringToNumber<GLfloat>(pObject->first_node(rgb[i])->value());
		return Vector3(v);
	};
};
