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
	//shader = ResourceManager::getInstance()->loadShader(4);
	// we have a proble with shader destructor
	Shaders sh = shader.get()->getShader();
	//sh.Init("../Resources/Shaders/TerrainVS.vs", "../Resources/Shaders/TerrainFS.fs");
	glUseProgram(sh.program);
	
	
	if (lightsId.size() > 4)
		throw std::string(name + "with id =" + std::to_string(id) + "has more than 4 lights associated to it");
	AmbientalLight amb = SceneManager::getInstance()->getAmbientalLight();
	if (sh.u_ambientalColor != -1)
	{
		glUniform3f(sh.u_ambientalColor, amb.color.x, amb.color.y, amb.color.z);
	}
	
	if (sh.u_ratio)
	{
		glUniform1f(sh.u_ratio, amb.ratio);
	}
	if (sh.u_diffuseCoeficient)
	{
		glUniform1f(sh.u_diffuseCoeficient, diffCoef);
	}
	if (sh.u_specularCoeficient)
	{
		glUniform1f(sh.u_specularCoeficient, specCoef);
	}
	for (int i = 0; i < lightsId.size(); i++)
	{
		Light l = *(SceneManager::getInstance()->getLights()[lightsId[i]].get());
		l.direction.Normalize();
		if (sh.u_lights[i].diffuseColor != -1)
		{
			glUniform3f(sh.u_lights[i].diffuseColor, l.diffuseColor.x, l.diffuseColor.y, l.diffuseColor.z);
		}
		if (sh.u_lights[i].specularColor != -1)
		{
			glUniform3f(sh.u_lights[i].specularColor, l.specularColor.x, l.specularColor.y, l.specularColor.z);
		}
		if (sh.u_lights[i].direction != -1)
		{
			glUniform3f(sh.u_lights[i].direction, l.direction.x, l.direction.y, l.direction.z);
		}
		if (sh.u_lights[i].position != -1)
		{
			glUniform3f(sh.u_lights[i].position, l.position.x, l.position.y, l.position.z);
		}
		if (sh.u_lights[i].shiness != -1)
		{

			glUniform1f(sh.u_lights[i].shiness, l.shiness);
		}
		if (sh.u_lights[i].Type != -1)
		{
			Vector4 type(0.0, 0.0, 0.0, 0.0);
			if (l.type == Light::Type::Directional)
				type.x = 1.0;
			else if (l.type == Light::Type::Point)
				type.y = 1.0;
			else if (l.type == Light::Type::Spot)
				type.z = 1.0;
			else if (l.type == Light::Type::Area)
				type.w = 1.0;
			glUniform4f(sh.u_lights[i].Type, type.x, type.y, type.z, type.w);
		}
	}
	
	// end light stuff
	glBindBuffer(GL_ARRAY_BUFFER, model.get()->getNormalVBO());


	/////////////////////////////////////////////////////
	
	

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
	Fog fog = SceneManager::getInstance()->getFog();
	if (sh.u_fog != -1)
	{
		glUniform3f(sh.u_fog, fog.color.x, fog.color.y, fog.color.z);
	}
	if (sh.u_r_radius != -1)
	{
		glUniform1f(sh.u_r_radius, fog.r);
	}
	if (sh.u_R_radius != -1)
	{
		glUniform1f(sh.u_R_radius, fog.R);
	}
	if (sh.u_wired != -1)
	{
		GLfloat f = (wiredFormat == WiredFormat::Normalf) ? 0.f : 1.f;
		glUniform1f(sh.u_wired, f);
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

	auto texture = textures.begin();
	int id = texture->get()->getTextureId();
	activeTextureUnitWithId(0);
	glBindTexture(GL_TEXTURE_2D, texture->get()->getTextureVboId());
	if (sh.u_rockTex != -1)
	{
		glUniform1i(sh.u_rockTex, 0);
	}
	texture++;
	id = texture->get()->getTextureId();
	activeTextureUnitWithId(1);
	glBindTexture(GL_TEXTURE_2D, texture->get()->getTextureVboId());
	if (sh.u_dirtTex != -1)
	{
		glUniform1i(sh.u_dirtTex, 1);
	}
	texture++;
	id = texture->get()->getTextureId();
	activeTextureUnitWithId(2);
	glBindTexture(GL_TEXTURE_2D, texture->get()->getTextureVboId());
	if (sh.u_grassTex != -1)
	{
		glUniform1i(sh.u_grassTex, 2);
	}
	
	texture++;
	id = texture->get()->getTextureId();
	activeTextureUnitWithId(3);
	glBindTexture(GL_TEXTURE_2D, texture->get()->getTextureVboId());
	if (sh.u_blendTex != -1)
	{
		glUniform1i(sh.u_blendTex, 3);
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
	{
		Shaders osh = model.get()->getAxesShader().get()->getShader();

		glUseProgram(osh.program);

		glBindBuffer(GL_ARRAY_BUFFER, model->getAxesVBO());
		// we link the position attribute from  vertex shader
		if (osh.positionAttribute != -1)
		{
			glEnableVertexAttribArray(osh.positionAttribute);
			glVertexAttribPointer(osh.positionAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
		}

		// we link the color attribute from vertex shader
		if (osh.colorAttribute != -1)
		{
			glEnableVertexAttribArray(osh.colorAttribute);
			glVertexAttribPointer(osh.colorAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), (void*)sizeof(Vector3));
		}
		if (osh.u_MVP != -1)
		{
			glUniformMatrix4fv(osh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
		}
		glDrawArrays(GL_LINES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		////////////////////////////////////////////////////////////////////////////////
		// AABB
		glUseProgram(osh.program);
		glBindBuffer(GL_ARRAY_BUFFER, model->getAABBVBO());
		// we link the position attribute from  vertex shader
		if (osh.positionAttribute != -1)
		{
			glEnableVertexAttribArray(osh.positionAttribute);
			glVertexAttribPointer(osh.positionAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
		}

		// we link the color attribute from vertex shader
		if (osh.colorAttribute != -1)
		{
			glEnableVertexAttribArray(osh.colorAttribute);
			glVertexAttribPointer(osh.colorAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), (void*)sizeof(Vector3));
		}
		if (osh.u_MVP != -1)
		{

			glUniformMatrix4fv(osh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
		}
		glDrawArrays(GL_LINES, 0, model.get()->getAABBNumberOfVerticies());

	}
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
