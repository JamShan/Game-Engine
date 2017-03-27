// NewTrainingFramework.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <conio.h>


#include "Vertex.h"
#include "ResourceManager.h"
#include "SceneManager.h"

#include "../FMOD/fmod.hpp"
#include "../FMOD/fmod_common.h"

#include "../Utilities/utilities.h" // if you use STL, please include this line AFTER all other include
#include "Globals.h"
	
POINT p;
float prag = 0.02;
float sumTime = 0.f;
std::shared_ptr<Camera> cam;
struct FrameBuffer
{
	GLuint framebuffer;
	GLuint depthRenderbuffer;
	GLuint texture;
	std::shared_ptr<Shader> shader;
	GLuint vbo;
	FrameBuffer()
	{
		shader = nullptr;
	}
	~FrameBuffer()
	{
		// cleanup
		glDeleteRenderbuffers(1, &depthRenderbuffer);
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &texture);
		glDeleteBuffers(1, &vbo);
	}
}fb1,fb2;
// we use fb2 to get the white/black stuff after using fb1 to get the rectangle
// then we use the texture got from fb1 and the one got from fb2 and add them togheter 

GLuint texWidth= Globals::screenWidth, texHeight= Globals::screenHeight;
GLint maxRenderbufferSize;

GLvoid InitializeFB(FrameBuffer &fb,int shaderId)
{

	fb.shader = ResourceManager::getInstance()->loadShader(shaderId);
	GLfloat data[] = {
		-1,1,0, 0,1,
		1,1,0, 1,1,
		1,-1,0,1,0,
		1,-1,0,1,0,
		-1,-1,0,0,0,
		-1,1,0, 0,1
	};
	glGenBuffers(1, &fb.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, fb.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glEnable(GL_DEPTH_TEST);// change the opengl es state by enabling the depth test
	
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
	// check if GL_MAX_RENDERBUFFER_SIZE is >= texWidth and texHeight
	if ((maxRenderbufferSize <= texWidth) ||
		(maxRenderbufferSize <= texHeight))
	{
		// cannot use framebuffer objects as we need to create
		// a depth buffer as a renderbuffer object
		// return with appropriate error
		std::cout << "error";
	}
	// generate the framebuffer, renderbuffer, and texture object names
	glGenFramebuffers(1, &fb.framebuffer);
	glGenRenderbuffers(1, &fb.depthRenderbuffer);
	glGenTextures(1, &fb.texture);
	// bind texture and load the texture mip-level 0
	// texels are RGB565
	// no texels need to be specified as we are going to draw into
	// the texture
	glBindTexture(GL_TEXTURE_2D, fb.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// bind renderbuffer and create a 16-bit depth buffer
	// width and height of renderbuffer = width and height of
	// the texture
	glBindRenderbuffer(GL_RENDERBUFFER, fb.depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texWidth, texHeight);
	// bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer);
	// specify texture as color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.texture, 0);
	// specify depth_renderbufer as depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb.depthRenderbuffer);
	glBindBuffer(GL_FRAMEBUFFER, 0);
}
int Init ( ESContext *esContext )
{
	ResourceManager::getInstance()->init();
	SceneManager::getInstance()->init();
	Vector3 vec = SceneManager::getInstance()->getBackgroundColor();
	glClearColor(vec.x, vec.y, vec.z, 0.0f);
	InitializeFB(fb1,101);
	InitializeFB(fb2, 102);
	return 0;
}

void Draw ( ESContext *esContext )
{
	// check for framebuffer complete
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status == GL_FRAMEBUFFER_COMPLETE)
	{
		// render to texture using FBO
		// clear color and depth buffer
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// catch the color in texture from fb1
		glBindFramebuffer(GL_FRAMEBUFFER, fb1.framebuffer);
		SceneManager::getInstance()->draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// catch the color in texture from fb2
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, fb2.framebuffer);

		{

			Shaders sha = fb1.shader->getShader();
			glUseProgram(sha.program);
			glBindBuffer(GL_ARRAY_BUFFER, fb1.vbo);

			// we link the position attribute from  vertex shader
			if (sha.positionAttribute != -1)
			{
				glEnableVertexAttribArray(sha.positionAttribute);
				glVertexAttribPointer(sha.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, 0);
			}

			// we link the uv attribute from vertex shader
			if (sha.uvAttribute != -1)
			{
				glEnableVertexAttribArray(sha.uvAttribute);
				glVertexAttribPointer(sha.uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(sizeof(GLfloat) * 3));
			}
			glBindTexture(GL_TEXTURE_2D, fb1.texture);
			// if we are using a normal texture we enter here
			if (sha.u_texture != -1)
			{
				// we active the texture we are working with, for the moment we are just using one texture
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fb1.texture);
				glUniform1i(sha.u_texture, 0);
			}
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// now draw 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// now do bloom using texture from fb1 and fb2 and shader from fb2
		{
			Shaders sha = fb2.shader->getShader();
			glUseProgram(sha.program);
			glBindBuffer(GL_ARRAY_BUFFER, fb2.vbo);

			// we link the position attribute from  vertex shader
			if (sha.positionAttribute != -1)
			{
				glEnableVertexAttribArray(sha.positionAttribute);
				glVertexAttribPointer(sha.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);
			}

			// we link the uv attribute from vertex shader
			if (sha.uvAttribute != -1)
			{
				glEnableVertexAttribArray(sha.uvAttribute);
				glVertexAttribPointer(sha.uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(sizeof(GLfloat) * 3));
			}
			glBindTexture(GL_TEXTURE_2D, fb2.texture);
			// if we are using a normal texture we enter here
			if (sha.u_texture != -1)
			{
				// we active the texture we are working with, for the moment we are just using one texture
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fb1.texture);
				glUniform1i(sha.u_texture, 0);
			}
			if (sha.u_texture2 != -1)
			{
				// we active the texture we are working with, for the moment we are just using one texture
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, fb2.texture);
				glUniform1i(sha.u_texture2, 1);
			}

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	
	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

void Update ( ESContext *esContext, float deltaTime )
{
	cam = SceneManager::getInstance()->getMainCamera();
	sumTime += deltaTime;
	if (sumTime >= prag)
	{
		cam->setDeltaTime(deltaTime);
		
		GetCursorPos(&p);
		ScreenToClient(esContext->hWnd, &p);
		cam->handleMouseEvent(p);
		
		if ((GetKeyState(VK_LEFT) & 0x80) != 0)
		{
			cam->rotateOy(1);
		}
		if ((GetKeyState(VK_RIGHT) & 0x80) != 0)
		{
			cam->rotateOy(-1);
		}
		if ((GetKeyState(VK_DOWN) & 0x80) != 0)
		{
			cam->rotateOx(1);
		}
		if ((GetKeyState(VK_UP) & 0x80) != 0)
		{
			cam->rotateOx(-1);
		}
		if ((GetKeyState(VK_UP) & 0x80) != 0)
		{
			cam->rotateOx(-1);
		}
		if ((GetKeyState(VK_OEM_PLUS) & 0x80) != 0)
		{
			cam->moveOy(-1);
		}
		if ((GetKeyState(VK_OEM_MINUS) & 0x80) != 0)
		{
			cam->moveOy(1);
		}
		sumTime = 0;
		SceneManager::getInstance()->update();
	}
}

void Key ( ESContext *esContext, unsigned char key, bool bIsPressed)
{
	cam = SceneManager::getInstance()->getMainCamera();
	if (bIsPressed)
	{
		if (key == 'A' || key == 'a')			// left
			cam->moveOx(-1);
		else if (key == 'D' || key == 'd')		// right
			cam->moveOx(1);
		else if (key == 'W' || key == 'w')		// forward
			cam->moveOz(-1);
		else if (key == 'S' || key == 's')		// back
			cam->moveOz(1);
		else if (key == 'Q' || key == 'q')		// rotation z
			cam->rotateOz(-1);
		else if (key == 'E' || key == 'e')		// rotation z
			cam->rotateOz(1);
		else if (key == 'P' || key == 'p')
			SceneManager::getInstance()->debugSettings.on = ! SceneManager::getInstance()->debugSettings.on;
		else if (key == VK_ESCAPE)
			exit(0);
	}
}

void CleanUp()
{
	ResourceManager::getInstance()->~ResourceManager();
	SceneManager::getInstance()->~SceneManager();
}

int _tmain(int argc, _TCHAR* argv[])
{
	ESContext esContext;

	esInitContext ( &esContext );

	esCreateWindow ( &esContext, "Engine", Globals::screenWidth, Globals::screenHeight, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	if ( Init ( &esContext ) != 0 )
		return 0;
	esRegisterDrawFunc ( &esContext, Draw );
	esRegisterUpdateFunc ( &esContext, Update );
	esRegisterKeyFunc ( &esContext, Key);
	esMainLoop ( &esContext );

	//releasing OpenGL resources
	CleanUp();

	printf("Press any key...\n");
	_getch();

	return 0;
}