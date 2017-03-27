#include "stdafx.h"
#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"
#include "SceneManager.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "../Utilities/Math.h"
#include "Model.h"
#include "HelperFunctions.h"
#include "ResourceManager.h"

#include "../Utilities/utilities.h"

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::getInstance()
{
	if (instance == nullptr)
		instance = new SceneManager();
	return instance;
}

SceneManager::SceneManager()
{
	// initilly we assume that we are not in debug mode
	debugSettings.on = GL_FALSE;
}

ObjectType SceneManager::getType(const GLchar * s)
{
	if (strcmp(s, "normal") == 0)
		return ObjectType::Normal;
	else if (strcmp(s, "skybox") == 0)
		return ObjectType::Skybox;
	else if (strcmp(s, "terrain") == 0)
		return ObjectType::Terrain;
	else if (strcmp(s, "fire") == 0)
		return ObjectType::Fire;
}

std::shared_ptr<Camera> SceneManager::getMainCamera()
{
	return cameras[mainCamera];
}

std::unordered_map<GLuint, std::shared_ptr<Light>>& SceneManager::getLights() 
{
	return lights;
}

AmbientalLight SceneManager::getAmbientalLight() const
{
	return ambientalLight;
}

Fog SceneManager::getFog() const
{
	return fog;
}

Vector3 SceneManager::getBackgroundColor() const
{
	return backgroundColor;
}

GLfloat SceneManager::getNormalLength() const
{
	return normalLength;
}

Vector3 SceneManager::getNormalColor() const
{
	return normalColor;
}

GLvoid SceneManager::init()
{
	ResourceManager::getInstance()->loadSound(Sounds::COLLISION);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	// parse the file
	rapidxml::file<> xmlFile("../Resources/SceneManager.xml"); // Default template is char
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	rapidxml::xml_node<> *pRoot = doc.first_node();
	// take position , scale , rotation
	
	// background data
	backgroundColor = getVector3rgb(pRoot->first_node("backgroundColor"));
	// normal data
	normalColor = getVector3rgb(pRoot->first_node("normalColor"));
	normalLength = helperfunctions::stringToNumber<GLfloat>(pRoot->first_node("normalLength")->value());
	
	rapidxml::xml_node<> *pDebug ;
	// check debug axes
	pDebug = pRoot->first_node("debugSettings");
	if (pDebug != nullptr)
	{
		rapidxml::xml_node<>* pObject = pDebug->first_node("objectAxes");
		debugSettings.on = GL_TRUE;
		debugSettings.objectAxes.OXcolor = getVector3rgb(pObject->first_node("OXColor"));
		debugSettings.objectAxes.OYcolor = getVector3rgb(pObject->first_node("OYColor"));
		debugSettings.objectAxes.OZcolor = getVector3rgb(pObject->first_node("OZColor"));
		pObject = pDebug->first_node("camAxes");
		debugSettings.camAxes.OXcolor = getVector3rgb(pObject->first_node("OXColor"));
		debugSettings.camAxes.OYcolor = getVector3rgb(pObject->first_node("OYColor"));
		debugSettings.camAxes.OZcolor = getVector3rgb(pObject->first_node("OZColor"));
	}
	debugSettings.on = GL_FALSE;
	// take position , scale , rotation
	
	rapidxml::xml_node<> *pObjects;
	// take fog data
	pObjects = pRoot->first_node("fog");
	fog.color = getVector3rgb(pObjects->first_node("color"));
	fog.r = helperfunctions::stringToNumber<GLfloat>(pObjects->first_node("begin")->value());
	fog.R = helperfunctions::stringToNumber<GLfloat>(pObjects->first_node("end")->value());
	// take cameras
	pObjects = pRoot->first_node("cameras");

	
	// id of the main camera
	mainCamera = helperfunctions::stringToNumber<GLuint>(pObjects->first_attribute("main")->value());

	for (rapidxml::xml_node<> *pObject = pObjects->first_node(); pObject != nullptr; pObject = pObject->next_sibling())
	{
		spca cam;
		Vector3 pos, tar, up;
		GLfloat fov, nearF, farF, rotSp, trSp;
		GLuint id = helperfunctions::stringToNumber<GLuint>(pObject->first_attribute("id")->value());
		pos = getVector3(pObject->first_node("position"));
		tar = getVector3(pObject->first_node("target"));
		up = getVector3(pObject->first_node("up"));
		fov = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("fov")->value());
		nearF = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("near")->value());
		farF = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("far")->value());
		trSp = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("translationSpeed")->value());
		rotSp = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("rotationSpeed")->value());
		cam = std::make_shared<Camera>(pos, tar, up, trSp, rotSp, farF, nearF, fov);
		std::vector<Vector3> axes;
		axes.push_back(debugSettings.camAxes.OXcolor);
		axes.push_back(debugSettings.camAxes.OYcolor);
		axes.push_back(debugSettings.camAxes.OZcolor);
		cam->axesColors = axes;
		cam->createAxes();
		cameras.insert(std::make_pair(id, std::move(cam)));
		cam.reset();
	}
	//take objects
	// take each object
	pObjects = pRoot->first_node("objects");

	for (auto pObject = pObjects->first_node(); pObject != nullptr; pObject = pObject->next_sibling())
	{
		// we get the type of the object 
		ObjectType type = getType(pObject->first_node("type")->value());
		//curently we only handle normal objects
		
		switch (type)
		{
		case ObjectType::Normal:
			loadNormalObject(pObject);
			break;
		case ObjectType::Skybox:
			loadSkyboxObject(pObject);
			break;
		case ObjectType::Terrain:
			loadTerrainObject(pObject);
			break;
		case ObjectType::Fire:
			loadFireObject(pObject);
			break;
		}
	}
	
	// check lights
	pObjects = pRoot->first_node("lights");
	std::shared_ptr<Light> light;
	auto lightType = [=](const std::string type)->Light::Type {
		if (type == "point")
		{
			return Light::Type::Point;
		}
		else if (type == "directional")
		{
			return Light::Type::Directional;
		}
		else if (type == "area")
		{
			return Light::Type::Area;
		}
		else if (type == "spot")
		{
			return Light::Type::Spot;
		}
	};
	
	GLuint lightId;
	for (rapidxml::xml_node<>* pObject = pObjects->first_node(); pObject != nullptr; pObject = pObject->next_sibling())
	{
		light = std::make_shared<Light>();
		light->associatedOject = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("associatedObject")->value());
		light->diffuseColor = getVector3rgb(pObject->first_node("diffuseColor"));
		light->specularColor = getVector3rgb(pObject->first_node("specularColor"));
		if (pObject->first_node("direction") != nullptr)
			light->direction = getVector3(pObject->first_node("direction"));
		if (pObject->first_node("position") != nullptr)
			light->position = getVector3(pObject->first_node("position"));
		if (pObject->first_node("angle") != nullptr)// used for spot light
			light->angle = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("angle")->value());
		light->shiness = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("shininess")->value());

		light->type = lightType(pObject->first_node("type")->value());
		lightId = helperfunctions::stringToNumber<GLfloat>(pObject->first_attribute()->value());
		lights[lightId] = std::move(light);
		light = nullptr;
	}

	// check ambiental light
	pObjects = pRoot->first_node("ambientalLight");
	if (pObjects != nullptr)
	{
		ambientalLight.color = getVector3rgb(pObjects->first_node("color"));
		ambientalLight.ratio = helperfunctions::stringToNumber<GLfloat>(pObjects->first_node("ratio")->value());
	}
	
	
}
// load a normal object
GLvoid SceneManager::loadNormalObject(rapidxml::xml_node<>* &pObject)
{
	
	//make a pointer to a resource manager and we must make sure this is made nullptr after we finish with it so there is no dangling pointer
	ResourceManager *resManager = ResourceManager::getInstance();

	std::shared_ptr<Model> model;

	model = resManager->loadModel(
		helperfunctions::stringToNumber<GLuint>(
			pObject->first_node("model")->value()
				
		)
	);
	// create the debug axes
	model.get()->axesColors.push_back(debugSettings.objectAxes.OXcolor);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OYcolor);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OZcolor);

	model.get()->AABBcolor = Vector3(1.0, 1.0, 0.0);
	if (pObject->first_node("AABBColor") != nullptr)
	{
		model.get()->AABBcolor = getVector3rgb(pObject->first_node("AABBColor"));
	}
	// take the shader 
	std::shared_ptr<Shader> shader;
	shader=	resManager->loadShader(
			helperfunctions::stringToNumber<GLuint>(
				pObject->first_node("shader")->value()
			)
		);
	std::vector<std::shared_ptr<Texture>> textures;
	// take the textures
	for (const rapidxml::xml_node<>* pTex = pObject->first_node("textures")->first_node(); pTex != nullptr; pTex = pTex->next_sibling())
	{
		TextureResourceProperties prop( 
				pTex->first_node("min_filter")->value(),
				pTex->first_node("mag_filter")->value(),
				pTex->first_node("wrap_s")->value(),
		pTex->first_node("wrap_t")->value(),
		pTex->first_attribute("type")->value()
		);
		textures.push_back(
			resManager->loadTexture(
				helperfunctions::stringToNumber<GLuint>(
					pTex->first_attribute("id")->value()
					),
				prop
			)
		);
	}
	// take position , scale , rotation

	Vector3 position, initialPosition, scale, rotation;
	position = getVector3(pObject->first_node("position"));
	initialPosition = position;
	scale = getVector3(pObject->first_node("scale"));
	rotation = getVector3(pObject->first_node("rotation"));

	// get the name
	std::string name = pObject->first_node("name")->value();
	std::string wired = pObject->first_node("wired")->value();
	WiredFormat wiredFormat =
		(wired == "normal") ?
		WiredFormat::Normalf :
		WiredFormat::Wiredf;

	//trajectory properties
	TrajectoryInfo trajectory;
	if (pObject->first_node("trajectory") != nullptr)
	{
		trajectory.has = GL_TRUE;
		trajectory.type = pObject->first_node("trajectory")->first_attribute("type")->value();
		std::string iter = pObject->first_node("trajectory")->first_attribute("iteration")->value();
		trajectory.speed = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("trajectory")->first_node("speed")->value());
		if (trajectory.type == "linear")
		{
			// we assume position is beginPoint
			trajectory.endPoint = getVector3(pObject->first_node("trajectory")->first_node("endPoint"));
			trajectory.direction = pObject->first_node("trajectory")->first_attribute("direction")->value();
		}
		else if (trajectory.type == "line_strip")
		{
			// we assume position is first point
			trajectory.points.push_back(position);
			trajectory.direction = pObject->first_node("trajectory")->first_attribute("direction")->value();
			for (rapidxml::xml_node<>* pPoint = pObject->first_node("trajectory")->first_node("points")->first_node("point"); pPoint != nullptr; pPoint = pPoint->next_sibling())
				trajectory.points.push_back(getVector3(pPoint));

		}
		else if (trajectory.type == "line_loop")
		{
			// we assume position is first point
			trajectory.points.push_back(position);
			for (rapidxml::xml_node<>* pPoint = pObject->first_node("trajectory")->first_node("points")->first_node("point"); pPoint != nullptr; pPoint = pPoint->next_sibling())
				trajectory.points.push_back(getVector3(pPoint));
		}
		else if (trajectory.type == "circle")
		{
			trajectory.radius = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("trajectory")->first_node("radius")->value());
			trajectory.rotationPlane = getVector3(pObject->first_node("trajectory")->first_node("rot"));
		}

		if (iter == "infinite")
			trajectory.infinite = true;
		else
		{
			trajectory.infinite = false;
			trajectory.iterStep = helperfunctions::stringToNumber<GLfloat>(iter);
		}
	}
	else
		trajectory.has = GL_FALSE;

	// add light stuff
	std::vector<GLuint> lightids;
	for (const rapidxml::xml_node<>* pLight = pObject->first_node("lights")->first_node(); pLight != nullptr; pLight = pLight->next_sibling())
		lightids.push_back(helperfunctions::stringToNumber<GLfloat>(pLight->value()));
	
	GLfloat diffCoef, specCoef;
	diffCoef = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("diffCoef")->value());
	specCoef = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("specCoef")->value());
	//now we move our object to objects vector
	spso object = std::make_shared<SceneObject>(
		ObjectType::Normal,
		model,
		std::move(shader),
		std::move(textures),
		position,
		initialPosition,
		scale,
		rotation,
		name,
		wiredFormat,
		trajectory,
		std::move(lightids),
		diffCoef,
		specCoef
		);
	// push object
	objects.push_back(std::move(object));
	object = nullptr;
	resManager = nullptr;
}
// load a fire
GLvoid SceneManager::loadFireObject(rapidxml::xml_node<>* &pObject)
{
	//make a pointer to a resource manager and we must make sure this is made nullptr after we finish with it so there is no dangling pointer
	ResourceManager *resManager = ResourceManager::getInstance();

	std::shared_ptr<Model> model;

	model = resManager->loadModel(
		helperfunctions::stringToNumber<GLuint>(
			pObject->first_node("model")->value()
			)
	);
	// create the debug axes
	model.get()->axesColors.push_back(debugSettings.objectAxes.OXcolor);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OYcolor);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OZcolor);
	
	model.get()->AABBcolor = Vector3(1.0, 1.0, 0.0);
	if (pObject->first_node("AABBColor") != nullptr)
	{
		model.get()->AABBcolor = getVector3rgb(pObject->first_node("AABBColor"));
	}
	// take the shader 
	std::shared_ptr<Shader> shader;
	shader = resManager->loadShader(
		helperfunctions::stringToNumber<GLuint>(
			pObject->first_node("shader")->value()
			)
	);
	std::vector<std::shared_ptr<Texture>> textures;
	// take the textures
	for (const rapidxml::xml_node<>* pTex = pObject->first_node("textures")->first_node(); pTex != nullptr; pTex = pTex->next_sibling())
	{
		TextureResourceProperties prop(
			pTex->first_node("min_filter")->value(),
			pTex->first_node("mag_filter")->value(),
			pTex->first_node("wrap_s")->value(),
			pTex->first_node("wrap_t")->value(),
			pTex->first_attribute("type")->value()
		);
		textures.push_back(
			resManager->loadTexture(
				helperfunctions::stringToNumber<GLuint>(
					pTex->first_attribute("id")->value()
					),
				prop
			)
		);
	}
	// take position , scale , rotation
	GLfloat u_DispMax = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("u_DispMax")->value());
	Vector3 position, initialPosition, scale, rotation;
	position = getVector3(pObject->first_node("position"));
	initialPosition = position;
	scale = getVector3(pObject->first_node("scale"));
	rotation = getVector3(pObject->first_node("rotation"));

	// get the name
	std::string name = pObject->first_node("name")->value();
	std::string wired = pObject->first_node("wired")->value();
	WiredFormat wiredFormat =
		(wired == "normal") ?
		WiredFormat::Normalf :
		WiredFormat::Wiredf;

	//trajectory properties
	TrajectoryInfo trajectory;
	if (pObject->first_node("trajectory") != nullptr)
	{
		trajectory.has = GL_TRUE;
		trajectory.type = pObject->first_node("trajectory")->first_attribute("type")->value();
		std::string iter = pObject->first_node("trajectory")->first_attribute("iteration")->value();
		trajectory.speed = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("trajectory")->first_node("speed")->value());
		if (trajectory.type == "linear")
		{
			// we assume position is beginPoint
			trajectory.endPoint = getVector3(pObject->first_node("trajectory")->first_node("endPoint"));
			trajectory.direction = pObject->first_node("trajectory")->first_attribute("direction")->value();
		}
		else if (trajectory.type == "line_strip")
		{
			// we assume position is first point
			trajectory.points.push_back(position);
			trajectory.direction = pObject->first_node("trajectory")->first_attribute("direction")->value();
			for (rapidxml::xml_node<>* pPoint = pObject->first_node("trajectory")->first_node("points")->first_node("point"); pPoint != nullptr; pPoint = pPoint->next_sibling())
				trajectory.points.push_back(getVector3(pPoint));

		}
		else if (trajectory.type == "line_loop")
		{
			// we assume position is first point
			trajectory.points.push_back(position);
			for (rapidxml::xml_node<>* pPoint = pObject->first_node("trajectory")->first_node("points")->first_node("point"); pPoint != nullptr; pPoint = pPoint->next_sibling())
				trajectory.points.push_back(getVector3(pPoint));
		}
		else if (trajectory.type == "circle")
		{
			trajectory.radius = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("trajectory")->first_node("radius")->value());
			trajectory.rotationPlane = getVector3(pObject->first_node("trajectory")->first_node("rot"));
		}

		if (iter == "infinite")
			trajectory.infinite = true;
		else
		{
			trajectory.infinite = false;
			trajectory.iterStep = helperfunctions::stringToNumber<GLfloat>(iter);
		}
	}
	else
		trajectory.has = GL_FALSE;

	//now we move our object to objects vector

	spfo object = std::make_shared<FireObject>(
		ObjectType::Fire,
		model,
		std::move(shader),
		std::move(textures),
		position,
		initialPosition,
		scale,
		rotation,
		name,
		wiredFormat,
		trajectory,
		u_DispMax
		);
	objects.push_back(std::move(object));
	resManager = nullptr;
}
// load a skybox object
GLvoid SceneManager::loadSkyboxObject(rapidxml::xml_node<>*& pObject)
{
	//make a pointer to a resource manager and we must make sure this is made nullptr after we finish with it so there is no dangling pointer
	ResourceManager *resManager = ResourceManager::getInstance();

	//object->type = ObjectType::Normal;
	// depending on the type of the object we read different properties
	std::shared_ptr<Model> model;

	model = resManager->loadModel(
		helperfunctions::stringToNumber<GLuint>(
			pObject->first_node("model")->value()

			)
	);
	// take the shader 
	std::shared_ptr<Shader> shader;
	shader = resManager->loadShader(
		helperfunctions::stringToNumber<GLuint>(
			pObject->first_node("shader")->value()
			)
	);
	std::vector<std::shared_ptr<Texture>> textures;
	// take the textures
	for (const rapidxml::xml_node<>* pTex = pObject->first_node("textures")->first_node(); pTex != nullptr; pTex = pTex->next_sibling())
	{
		TextureResourceProperties prop(
			pTex->first_node("min_filter")->value(),
			pTex->first_node("mag_filter")->value(),
			pTex->first_node("wrap_s")->value(),
			pTex->first_node("wrap_t")->value(),
			pTex->first_attribute("type")->value()
		);
		textures.push_back(
			resManager->loadTexture(
				helperfunctions::stringToNumber<GLuint>(
					pTex->first_attribute("id")->value()
					),
				prop
			)
		);
	}

	// get the name
	std::string name = pObject->first_node("name")->value();

	
	// take position , scale , rotation
	
	Vector3 scale = getVector3(pObject->first_node("scale"));
	GLfloat offsetOY = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("offsetOY")->value());
	//now we move our object to objects vector
	spsb object = std::make_shared<Skybox>(
		ObjectType::Skybox,
		model,
		std::move(shader),
		std::move(textures),
		name,
		scale
		);
	object.get()->offsetOY = offsetOY;
	objects.push_back(std::move(object));
	resManager = nullptr;
}
// load a terrain object
GLvoid SceneManager::loadTerrainObject(rapidxml::xml_node<>*& pObject)
{
	//make a pointer to a resource manager and we must make sure this is made nullptr after we finish with it so there is no dangling pointer
	ResourceManager *resManager = ResourceManager::getInstance();

	// depending on the type of the object we read different properties
	std::shared_ptr<Model> model;
	model = std::make_shared<Model>();
	GLuint cells =  helperfunctions::stringToNumber<GLuint>(pObject->first_node("cells")->value());
	GLfloat cellsize = helperfunctions::stringToNumber<GLuint>(pObject->first_node("cellsize")->value());
	model.get()->generate(cells, cellsize);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OXcolor);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OYcolor);
	model.get()->axesColors.push_back(debugSettings.objectAxes.OZcolor);
	
	model.get()->AABBcolor = Vector3(1.0, 1.0, 0.0);
	if (pObject->first_node("AABBColor") != nullptr)
	{
		model.get()->AABBcolor = getVector3rgb(pObject->first_node("AABBColor"));
	}
	// take the shader 
	std::shared_ptr<Shader> shader;
	shader = resManager->loadShader(
		helperfunctions::stringToNumber<GLuint>(
			pObject->first_node("shader")->value()
			)
	);
	std::vector<std::shared_ptr<Texture>> textures;
	// take the textures
	for (const rapidxml::xml_node<>* pTex = pObject->first_node("textures")->first_node(); pTex != nullptr; pTex = pTex->next_sibling())
	{
		TextureResourceProperties prop(
			pTex->first_node("min_filter")->value(),
			pTex->first_node("mag_filter")->value(),
			pTex->first_node("wrap_s")->value(),
			pTex->first_node("wrap_t")->value(),
			pTex->first_attribute("type")->value()
		);
		textures.push_back(
			resManager->loadTexture(
				helperfunctions::stringToNumber<GLuint>(
					pTex->first_attribute("id")->value()
					),
				prop
			)
		);
	}


	Vector3 height = getVector3rgb(pObject->first_node("heights"));
	// get the name
	std::string name = pObject->first_node("name")->value();
	std::string wired = pObject->first_node("wired")->value();
	WiredFormat wiredFormat =
		(wired == "normal") ?
		WiredFormat::Normalf :
		WiredFormat::Wiredf;

	
	
	GLfloat oneOverN = 1.0 / (GLfloat)cells;
	/////////////// light stuff
	std::vector<GLuint> lightids;
	for (const rapidxml::xml_node<>* pLight = pObject->first_node("lights")->first_node(); pLight != nullptr; pLight = pLight->next_sibling())
		lightids.push_back(helperfunctions::stringToNumber<GLfloat>(pLight->value()));
	GLfloat diffCoef, specCoef;
	diffCoef = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("diffCoef")->value());
	specCoef = helperfunctions::stringToNumber<GLfloat>(pObject->first_node("specCoef")->value());
	

	//now we move our object to objects vector

	spto object = std::make_shared<TerrainObject>(
		ObjectType::Terrain,
		model,
		std::move(shader),
		std::move(textures),
		name,
		wiredFormat,
		height,
		cellsize,
		cells,
		oneOverN,
		std::move(lightids),
		diffCoef,
		specCoef
		);
	objects.push_back(std::move(object));
	object = nullptr;
	resManager = nullptr;
}
// draw the objects
GLvoid SceneManager::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw the objects
	for (auto it = objects.begin(); it != objects.end(); ++it)
		(*it)->draw();
	// draw the axes for main camera
	if(debugSettings.on == GL_TRUE)
		cameras[mainCamera].get()->drawAxes();
	
}
// update the objects before draw
GLvoid SceneManager::update()
{
	for (auto it = objects.begin(); it != objects.end(); ++it)
		(*it)->update();
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		if((*it).get()->collides(cameras[mainCamera].get()->getAABB()) )
			ResourceManager::getInstance()->playSound(Sounds::COLLISION);
	}
	/*for (auto it = objects.begin(); it != objects.end(); it++)
		for (auto it2 = it + 1; it2 != objects.end(); ++it2)
			if ((*it).get()->collides(*it2))
				 ResourceManager::getInstance()->playSound(Sounds::COLLISION);*/
}
// remove all objects and asociated resources
SceneManager::~SceneManager()
{
}
