#include "stdafx.h"
#include "Texture.h"

#include <string>
#include <vector>

#include "ResourceManager.h"
#include "HelperStructs.h"
#include "../Utilities/utilities.h"
Texture::TextureType Texture::getTextureType()
{
	return type;
}
GLuint& Texture::getTextureVboId()
{
	return  tvboId;
}
GLuint& Texture::getTextureId()
{
	return id;
}
GLvoid Texture::load()
{
	std::string path("../");
	path += tr.path;
	if (type == TextureType::_2D)
	{
		glGenTextures(1, &tvboId);

		glBindTexture(GL_TEXTURE_2D, tvboId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, prop.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, prop.magFilter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, prop.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, prop.wrapT);

		GLint width{}, height{}, bpp{};
		GLchar * s = LoadTGA(path.c_str(), &width, &height, &bpp);
		GLint g = (bpp == 24) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, g, width, height, 0, g, GL_UNSIGNED_BYTE, s);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if (type == TextureType::_Cube)
	{
		GLint err = glGetError();
		glGenTextures(1, &tvboId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tvboId);
		GLint W{}, H{}, bpp{};
		GLchar *s = LoadTGA(path.c_str(), &W, &H, &bpp);
		GLint g = (bpp == 24) ? GL_RGB : GL_RGBA;
		// 3 or 4 bytes
		GLuint bytes = (bpp == 24) ? 3 : 4;
		GLuint w = W / 4, h = H / 3;
		// here we keep the 6 texture vectors
		std::vector<std::vector<GLchar>> facets;
		// right facet (0)
		std::vector<GLchar> right;
		for (GLuint i = 0; i < h; i++)
		{
			GLuint offset = (W*bytes)*h ;
			offset += i*W*bytes ;
			offset += W*bytes / 2;
			for (GLuint j = 0; j < w*bytes; j++)
				right.push_back(s[offset + j]);
		}
		facets.push_back(right);
		// left facet (1)
		std::vector<GLchar>left;
		for (GLuint i = 0; i < h; i++)
		{
			GLuint offset = (W*bytes)*h;
			offset += i*W*bytes;
			for (GLuint j = 0; j < w*bytes; j++)
				left.push_back(s[offset + j]);
		}
		facets.push_back(left);
		// up facet (2)
		std::vector<GLchar>up;
		for (GLuint i = 0; i < h; i++)
		{
			GLuint offset = i*W*bytes;
			offset += w*bytes;
			for (GLuint j = 0; j < w*bytes; j++)
				up.push_back(s[offset + j]);
		}
		facets.push_back(up);
		// down facet (3)
		std::vector<GLchar>down;
		for (GLuint i = 0; i < h; i++)
		{
			GLuint offset = W*bytes*h * 2;
			offset += i*W*bytes;
			offset += w*bytes;
			for (GLuint j = 0; j < w*bytes; j++)
				down.push_back(s[offset + j]);
		}
		facets.push_back(down);
		
		// front facet (4)
		std::vector<GLchar>front;
		for (GLuint i = 0; i < h; i++)
		{
			GLuint offset = (W*bytes)*h;
			offset += i*W*bytes;
			offset += w*bytes;
			for (GLuint j = 0; j < w*bytes; j++)
				front.push_back(s[offset + j]);
		}
		facets.push_back(front);
		// back facet (5)
		std::vector<GLchar>back;
		for (GLuint i = 0; i < h; i++)
		{
			GLuint offset = (W*bytes)*h;
			offset += i*W*bytes;
			offset += (W -w)*bytes;
			for (GLuint j = 0; j < w*bytes; j++)
				back.push_back(s[offset + j]);
		}
		facets.push_back(back);
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, prop.minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, prop.magFilter);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, prop.wrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, prop.wrapT);
		for (GLuint f = GL_TEXTURE_CUBE_MAP_POSITIVE_X,i = 0; i< 6; i++)
		{

			glTexImage2D(f + i, 0, g, w, h, 0, g, GL_UNSIGNED_BYTE, facets[i].data());
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		err = glGetError();
	}
	
}

Texture::Texture(GLuint _id ,TextureResourceProperties _prop):prop(_prop),id(_id)
{
	if (prop.type == "2d")
		type = TextureType::_2D;
	else if (prop.type == "cube")
		type = TextureType::_Cube;
	else//default value
		type = TextureType::_2D;

	// find the texture resource based on the recieved id in the _id field
	tr = ResourceManager::getInstance()->textureResources[id];
}


Texture::~Texture()
{
	// after we finished using this texture we remove it 
	//if (glIsTexture(tvboId) == GL_TRUE)
		//glDeleteTextures(1, &tvboId);
}
