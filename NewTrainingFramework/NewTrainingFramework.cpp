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

int Init ( ESContext *esContext )
{
	ResourceManager::getInstance()->init();
	SceneManager::getInstance()->init();
	Vector3 vec = SceneManager::getInstance()->getBackgroundColor();
	
	glEnable(GL_DEPTH_TEST);// change the opengl es state by enabling the depth test
	glClearColor(vec.x,vec.y,vec.z,0.0f);
	return 0;
}

void Draw ( ESContext *esContext )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	SceneManager::getInstance()->draw();
	
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
		else if (key == 'I' || key == 'i')
		{
			SceneManager::ob = (SceneManager::ob == 0) ? 1 : 0;
		};
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