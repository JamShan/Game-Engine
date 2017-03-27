#include "stdafx.h"
#include "TerrainObject.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "../Utilities/utilities.h"
GLvoid TerrainObject::Init()
{

}

GLint TerrainObject::sign(GLfloat d)
{
	if (d > 0.0000000000) return 1;
	return -1;
}

GLvoid TerrainObject::draw()
{
	if (SceneManager::getInstance()->debugSettings.on == GL_TRUE)
		wiredFormat = WiredFormat::Wiredf;
	else
		wiredFormat = WiredFormat::Normalf;
	// we have 4 textures  3 small textures rock,dirt,grass(in this order loaded in textures map) and a blend map 
	
	// we have a proble with shader destructor
	Shaders sh = shader.get()->getShader();

	glUseProgram(sh.program);
	
	sendToShader(sh);
	
	// end light stuff
	glBindBuffer(GL_ARRAY_BUFFER, model.get()->getNormalVBO());


	// we link the position attribute from  vertex shader
	if (sh.positionAttribute != -1)
	{
		glEnableVertexAttribArray(sh.positionAttribute);
		glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexnou), 0);
	}
	if (sh.v_uv_small != -1)
	{
		glEnableVertexAttribArray(sh.v_uv_small);
		glVertexAttribPointer(sh.v_uv_small, 2, GL_FLOAT, GL_FALSE, sizeof(Vertexnou), (GLvoid*)(sizeof(Vector3)));
	}
	if (sh.v_uv_large != -1)
	{
		glEnableVertexAttribArray(sh.v_uv_large);
		glVertexAttribPointer(sh.v_uv_large, 2, GL_FLOAT, GL_FALSE, sizeof(Vertexnou), (GLvoid*)(sizeof(Vector3)+sizeof(Vector2)));
	}

	if (sh.a_normal != -1)
	{
		glEnableVertexAttribArray(sh.a_normal);
		glVertexAttribPointer(sh.a_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexnou), (GLvoid*)( sizeof(Vector3) + 2*sizeof(Vector2) ) );
	}
	// we link the uniforms
	if (sh.u_height != -1)
	{
		glUniform3f(sh.u_height, height.x, height.y, height.z);
	}


	// we link the uniform MVP matrix from vertex shader
	// model view projection matrix
	Matrix mvp;

	// terrain translation
	Vector3 posCamera = SceneManager::getInstance()->getMainCamera().get()->getPositon();
	if (sh.u_cam_coord != -1)
	{
		glUniform3f(sh.u_cam_coord, posCamera.x, posCamera.y, posCamera.z);
	}
	GLfloat dx = posCamera.x - Xcenter;
	GLfloat dz = posCamera.z - Zcenter;
	GLfloat signX = 0.0, signZ = 0.0;
	if (abs(dx) > cellWidth)
	{
		signX = sign(dx);
		Xcenter += signX*cellWidth;
	}
	if (abs(dz) > cellWidth)
	{
		signZ = sign(dz);
		Zcenter += signZ*cellWidth;
	}	
	// translate the texture indicies
	if(signX != 0 || signZ != 0)
		model.get()->updateTexture(signZ*oneOverN, signX * oneOverN);
	// we translate the model acordingly for X and Z axis
	modelMatrix.SetTranslation(Xcenter, 0, Zcenter);
	
	if (sh.u_model != -1)
	{
		glUniformMatrix4fv(sh.u_model,1,GL_FALSE, (GLfloat*)modelMatrix.m);
	}
	Matrix proj = SceneManager::getInstance()->getMainCamera()->getProjectionMatrix();
	Matrix view = SceneManager::getInstance()->getMainCamera()->getViewMatrix();
	mvp = modelMatrix*view*proj;
	if (sh.u_MVP != -1)
	{
		glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
	}
	
	// we active the texture we are working with rock,dirt,grass in this order because we also have rock,dirt,grass in scene manager xml
	{
		int id = 0;
		for (auto &texture : textures)
		{
			activeTextureUnitWithId(id);
			glBindTexture(GL_TEXTURE_2D, texture.get()->getTextureVboId());
			if (sh.u_terrainTex[id] != -1)
			{
				glUniform1i(sh.u_terrainTex[id], id);
			}
			id++;
		}
	}
	// we link the vertex buffer object for the indices

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.get()->getNormalEBO());

	if (wiredFormat == WiredFormat::Normalf)
		// and the draw triangles
		glDrawElements(GL_TRIANGLES, model.get()->getNormalNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);
	else
		glDrawElements(GL_LINES, model.get()->getNormalNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);
	
	// close the element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// close the array buffer 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind the texture 2d
	glBindTexture(GL_TEXTURE_2D, 0);
	// here we draw the debug axes
	if (SceneManager::getInstance()->debugSettings.on == GL_TRUE)
		drawDebug(mvp);
}

GLvoid TerrainObject::update()
{

	
}

TerrainObject::TerrainObject(
	ObjectType _objectType, 
	std::shared_ptr<Model> _model,
	std::shared_ptr<Shader> _shader, 
	std::vector<std::shared_ptr<Texture>> _textures, 
	std::string _name, 
	WiredFormat _wiredFormat,
	Vector3 _height,
	GLfloat _cellWidth,
	GLfloat _nrceclls,
	GLfloat _oneOverN,
	std::vector<GLuint> _lightsId,
	GLfloat _difCoef,
	GLfloat _specCoef):
	SceneObject(
		_objectType,
		_model,
		_shader,
		_textures,
		_name,
		_wiredFormat,
		std::move(_lightsId),
		_difCoef,
		_specCoef)
{
	// terrain height
	height = _height;
	// terrain position on OX OZ axes
	Xcenter = Zcenter = 0;
	Vector3 posCamera = SceneManager::getInstance()->getMainCamera().get()->getPositon();
	Xcenter = posCamera.x;
	Zcenter = posCamera.z;
	// cell width , numer of cells and 1/nr cells
	cellWidth = _cellWidth;
	nrCells = _nrceclls;
	oneOverN = _oneOverN;
}

TerrainObject::~TerrainObject()
{
}
