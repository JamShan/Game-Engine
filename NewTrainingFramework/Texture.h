#pragma once
#include "HelperStructs.h"
/*
	This class load a texture from file 
*/
class Texture
{

	
public:
	enum TextureType // we assume there are currently only 2 texture types available
	{
		_2D, // 2D texture
		_Cube // and cube texture for skybox
	};
private:
	TextureType type;
	// texture location 
	TextureResource tr; 
	// texture vertex buffer object id
	GLuint tvboId;
	// each texture type has some properties while drawing it, so we hold them in this variable
	TextureResourceProperties prop;
	// texture id
	GLuint id;
public:
	
	Texture::TextureType getTextureType();
	GLuint& getTextureVboId();
	GLuint& getTextureId();
	// load the texture from the file 
	GLvoid load();

	Texture(GLuint , TextureResourceProperties prop);
	~Texture();
};

