#include "stdafx.h"
#include "Skybox.h"
#include "SceneManager.h"

GLvoid Skybox::draw()
{
	
	// the texture used is a cubemap
	
	Shaders sh = shader.get()->getShader();

	glUseProgram(sh.program);

	glBindBuffer(GL_ARRAY_BUFFER, model->getNormalVBO());

	// we link the position attribute from  vertex shader
	if (sh.positionAttribute != -1)
	{
		glEnableVertexAttribArray(sh.positionAttribute);
		glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}


	// we link the uniform MVP matrix from vertex shader
	Matrix mvp;
	Matrix scalare,m,rotire,trans;
	rotire.SetRotationZ(PI);
	scalare.SetScale(scale);
	Vector3 camOffset = SceneManager::getInstance()->getMainCamera().get()->getPositon();
	trans.SetTranslation(Vector3(camOffset.x, camOffset.y+offsetOY, camOffset.z));
	m = rotire*scalare*trans;
	Matrix proj = SceneManager::getInstance()->getMainCamera()->getProjectionMatrix();
	Matrix view = SceneManager::getInstance()->getMainCamera()->getViewMatrix();

	mvp = m*view*proj;
	if (sh.u_MVP != -1)
	{
		glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
	}


	
	if (sh.u_cub_texture != -1)
	{
		// we active the texture we are working with, for the moment we are just using one texture
		activeTextureUnitWithId(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textures.begin()->get()->getTextureVboId());
		glUniform1i(sh.u_cub_texture, 0);
	}
	
	// we link the vertex buffer object for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.get()->getNormalEBO());

	// and the draw triangles
	glDrawElements(GL_TRIANGLES, model.get()->getNormalNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);

	// close the element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// close the array buffer 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind the texture 2d
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
}

GLvoid Skybox::update()
{
	
}

Skybox::Skybox(ObjectType _objectType,
	std::shared_ptr<Model> _model,
	std::shared_ptr<Shader> _shader,
	std::vector<std::shared_ptr<Texture>> _textures, 
	std::string _name,
	Vector3 _scale):
	SceneObject(
		_objectType,
		_model, 
		_shader,
		_textures, 
		_name,
		_scale)
{
}

Skybox::~Skybox()
{
}
