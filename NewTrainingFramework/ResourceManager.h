#pragma once
#include "../rapidxml-1.13/rapidxml.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>


#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "HelperStructs.h"

//fmod
#include "../FMOD/fmod.hpp"

#include "../Utilities/utilities.h"

/*
	This class manages the resources for objects in the scene.
	It is a singleton.
	It parses the ResourceManager.xml configuration file and adds all that data in some vectors then
	creates in different vectors the textures, models and shaders based on those initial information vectors
*/
class ResourceManager
{
private:
	
	// actual ,loaded to opengl information from the parsed information of the .xml config file
	typedef std::shared_ptr<Model> sptrm;
	typedef std::shared_ptr<Texture> sptrt;
	typedef std::shared_ptr<Shader> sptrs;

	// textures map where each Texture has an id mapped to it
	std::unordered_map< GLuint, std::shared_ptr<Texture>> textures;
	
	// models map where each Texture has an id mapped to it
	std::unordered_map< GLuint, std::shared_ptr<Model>> models;

	// shaders map where each Texture has an id mapped to it
	std::unordered_map< GLuint, std::shared_ptr<Shader>> shaders;
	// sounds map where each Sound has an id mapped to it
	FMOD::Sound ** sounds;
	
	static ResourceManager * instance;
	ResourceManager();
	// utilities used for parsing
	std::unordered_map<GLuint, ModelResource> getModels(rapidxml::xml_node<>*);
	std::unordered_map<GLuint, ShaderResource> getShaders(rapidxml::xml_node<>*);
	std::unordered_map<GLuint, TextureResource> getTextures(rapidxml::xml_node<>*);
	std::unordered_map<GLuint, SoundResource> getSounds(rapidxml::xml_node<>*);
	
	FMOD::System *fmodSystem;
public:
	
	// free the loaded resources
	GLvoid freeResource();

	static ResourceManager* getInstance();
	// information after parsing the ResourceManager.xml config file
	// here we keep the loaded information after parsing the xml file
	/*std::vector<TextureResource> textureResources;
	std::vector<ShaderResource> shaderResources;
	std::vector<ModelResource> modelResources;*/
	
	std::unordered_map<GLuint, TextureResource> textureResources;
	std::unordered_map<GLuint, ShaderResource> shaderResources;
	std::unordered_map<GLuint, ModelResource> modelResources;
	std::unordered_map<GLuint, SoundResource> soundResources;
	// parse xml file and load resources to  resources vectors
	GLvoid init();

	// dtr
	~ResourceManager();
	
	// get a shared pointer to a Texture with a specific id, that has spefici properties
	std::shared_ptr<Texture> loadTexture(GLuint id, TextureResourceProperties prop);
	// get a shared pointer to a Sound with a specific id
	GLvoid loadSound(GLuint id);
	GLvoid playSound(GLuint id);

	// get a shared pointer to a Shader with a specific id
	std::shared_ptr<Shader> loadShader(GLuint id);
	
	// get a shared pointer to a Model with a specific id
	std::shared_ptr<Model> loadModel(GLuint id);
};
