#include "stdafx.h"
#include "ResourceManager.h"

#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "HelperFunctions.h"
#include "HelperStructs.h"
#include "../Utilities/utilities.h" // if you use STL, please include this line AFTER all other include

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager* ResourceManager::getInstance()
{
	if (instance == nullptr)
		instance = new ResourceManager();
	return instance;
}
// here we parse the xml file and add all data to our resource vectors
ResourceManager::ResourceManager()
{

}
/*
	Parse the ResourceManager.xml file and get all objects,cameras,etc loaded in memory.
*/
GLvoid ResourceManager::init()
{
	// sound init
	if(FMOD::System_Create(&fmodSystem)!= FMOD_OK)
	{
		// no sounds
		
	}
	else
		fmodSystem->init(36,FMOD_INIT_NORMAL,NULL);// initialy 36 channels
	
	rapidxml::xml_document<> doc;
	std::ifstream file("../Resources/ResourceManager.xml");
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string content(buffer.str());
	doc.parse<0>(&content[0]);

	rapidxml::xml_node<> *pRoot = doc.first_node()->first_node();
	for (rapidxml::xml_node<> *n = pRoot; n != nullptr; n = n->next_sibling())
	{
		if (strcmp(n->name(), "models") == 0)
			modelResources = std::move(getModels(n));
		else if (strcmp(n->name(), "shaders") == 0)
			shaderResources = std::move(getShaders(n)); 
		else if (strcmp(n->name(), "textures") == 0)
			textureResources = std::move(getTextures(n));
		else if (strcmp(n->name(), "sounds") == 0)
			soundResources = std::move(getSounds(n));
	}
	sounds = new FMOD::Sound*[soundResources.size()+1];
}

std::unordered_map<GLuint, ModelResource> ResourceManager::getModels(rapidxml::xml_node<>* pRoot)
{
	std::unordered_map<GLuint, ModelResource> modelss;
	GLuint id;
	for (rapidxml::xml_node<> * p = pRoot->first_node(); p != NULL; p = p->next_sibling())
	{
		id = helperfunctions::stringToNumber<GLuint>(p->first_attribute()->value());
		modelss.insert(
			std::make_pair(
				id,
				ModelResource(
					id, 
					p->first_node()->value()
				)
			)
		);
	}
	return std::move(modelss);
}

std::unordered_map<GLuint, ShaderResource> ResourceManager::getShaders(rapidxml::xml_node<>* pRoot)
{
	std::unordered_map<GLuint, ShaderResource> shaderss;
	GLuint id;
	for (rapidxml::xml_node<> * p = pRoot->first_node(); p != NULL; p = p->next_sibling())
	{
		id = helperfunctions::stringToNumber<GLuint>(p->first_attribute()->value());
		shaderss.insert(
			std::make_pair(
				id,
				ShaderResource(
					id,
					p->first_node()->value(),
					p->last_node()->value()
				)
			)
		);
	}
	return std::move(shaderss);
}

std::unordered_map<GLuint, TextureResource> ResourceManager::getTextures(rapidxml::xml_node<>* pRoot)
{
	std::unordered_map<GLuint, TextureResource> texturess;
	GLuint id;
	for (rapidxml::xml_node<> * p = pRoot->first_node(); p != NULL; p = p->next_sibling())
	{
		id = helperfunctions::stringToNumber<GLuint>( p->first_attribute()->value() );
		texturess.insert(
			std::make_pair(
				id,
				TextureResource(
					id,
					p->first_node()->value()
				)
			)
		);
	}
	return std::move(texturess);
}
std::unordered_map<GLuint, SoundResource> ResourceManager::getSounds(rapidxml::xml_node<>* pRoot)
{
	std::unordered_map<GLuint, SoundResource> soundss;
	GLuint id;
	for (rapidxml::xml_node<> * p = pRoot->first_node(); p != NULL; p = p->next_sibling())
	{
		id = helperfunctions::stringToNumber<GLuint>( p->first_attribute()->value() );
		soundss.insert(
			std::make_pair(
				id,
				SoundResource(
					id,
					p->first_node()->value()
				)
			)
		);
	}
	return std::move(soundss);
}
/*
	Free the loaded resources in memory
*/
GLvoid ResourceManager::freeResource()
{
	// we have a map of pointers of shared pointers to objects so we need to make all pointers point to nullptr
	// because they are shared_ptr when we release all of them the destructor is called automaticly
	for (auto it = textures.begin(); it != textures.end(); ++it)
		it->second = nullptr;
	textures.clear();
	for (auto it = models.begin(); it != models.end(); ++it)
		it->second = nullptr;
	models.clear();
	for (auto it = shaders.begin(); it != shaders.end(); ++it)
		it->second = nullptr;
	shaders.clear();
	for (int i = 0; i < soundResources.size(); i++)
	{
		sounds[i]->release();
		sounds[i] = nullptr;
	}
}
/*
	Free everything from ResourceManager
*/
ResourceManager::~ResourceManager()
{
	// free the resources if not already freed
	freeResource();
	// and the rest of the vectors are released in a single clear
	textureResources.clear();
	shaderResources.clear();
	modelResources.clear();
	
	for (int i = 0; i < soundResources.size(); i++)
	{
		sounds[i]->release();
		sounds[i] = nullptr;
	}
	soundResources.clear();
}
/*
	Load a texture resource , if already loaded then return a shared_ptr pointing to it, otherwise make it , add it in the 
	textures map, and return a pointer pointing to it.
*/
std::shared_ptr<Texture> ResourceManager::loadTexture(GLuint id, TextureResourceProperties prop)
{
	
	auto rez = textures.find(id);
	if (rez != textures.end())
		return rez->second;		//we found the resource already loaded and created and just return a pointer
	auto exists = textureResources.find(id);
	
	if (exists != textureResources.end())// if the resource exists in resourceManager.xml but is not created we create it
	{
		sptrt resource = std::make_shared<Texture>(id, prop);
		resource->load();
		textures.insert(std::make_pair(id, std::move(resource)));
		return textures.find(id)->second; // we return a pointer to the newly created resource
	}
	return nullptr;// if there is no resource with that id
}
/*
	Load a sound resource , if already loaded then return a shared_ptr pointing to it, otherwise make it , add it in the 
	sound map, and return a pointer pointing to it.
*/
GLvoid ResourceManager::loadSound(GLuint id)
{
	std::unordered_map<GLuint, SoundResource>::iterator  exists = soundResources.find(id);
	if (exists != soundResources.end())// if the resource exists in resourceManager.xml but is not created we create it
	{
		std::string path("../" + exists->second.path);
		fmodSystem->createSound(path.c_str(), FMOD_LOOP_OFF, 0, &sounds[id]);
	}
	else
		throw "no sound found with that id";
}
GLvoid ResourceManager::playSound(GLuint id)
{
	if( id > soundResources.size())
		throw "no sound found with that id";
	else
		fmodSystem->playSound(sounds[id], 0, false, 0);
}
/*
Load a shader resource , if already loaded then return a shared_ptr pointing to it, otherwise make it , add it in the
shaders map, and return a pointer pointing to it.
*/
std::shared_ptr<Shader> ResourceManager::loadShader(GLuint id)
{
	auto rez = shaders.find(id);
	if (rez != shaders.end())
		return rez->second;		//we found the resource already loaded and just return a pointer
	auto exists = shaderResources.find(id);
	if (exists != shaderResources.end())
	{
		sptrs resource = std::make_shared<Shader>(id);
		resource->load();
		shaders.insert(std::make_pair(id, std::move(resource)));
		return shaders.find(id)->second; // we return a pointer to the newly created resource
	}
	return nullptr; // if there is no resource with that id
}
/*
Load a model resource , if already loaded then return a shared_ptr pointing to it, otherwise make it , add it in the
models map, and return a pointer pointing to it.
*/
std::shared_ptr<Model> ResourceManager::loadModel(GLuint id)
{
	auto rez = models.find(id);
	if (rez != models.end())
		return rez->second;		//we found the resource already loaded and just return a pointer
	auto exists = modelResources.find(id);
	if (exists != modelResources.end())
	{
		sptrm resource = std::make_shared<Model>(id);
		resource->load();
		models.insert(std::make_pair(id, std::move(resource)));
		return models.find(id)->second; // we return a pointer to the newly created resource
	}
	return nullptr; // if there is no resource with that id
}
