#include "stdafx.h"
#include "FireObject.h"
#include "SceneManager.h"

#include <iostream>

FireObject::FireObject(ObjectType _objectType,
	std::shared_ptr<Model> _model, 
	std::shared_ptr<Shader> _shader,
	std::vector<std::shared_ptr<Texture>> _textures,
	Vector3 _position,
	Vector3 _initialPosition, 
	Vector3 _scale,
	Vector3 _rotation, 
	std::string _name,
	WiredFormat _wiredFormat, 
	TrajectoryInfo _trajectoryInfo, 
	GLfloat _u_DispMax) :
	SceneObject(
		_objectType,
		std::move( _model),
		std::move( _shader),
		std::move( _textures),
		_position,
		 _initialPosition,
		_scale,
		_rotation,
		_name,
		_wiredFormat,
		_trajectoryInfo
	) 
{
	u_DispMax = _u_DispMax;
	time = 0;
	update();
}

GLvoid FireObject::draw()
{
	
	// if debug settings is on we draw everything wired and we also draw the debug axes
	if (SceneManager::getInstance()->debugSettings.on == GL_TRUE)
		wiredFormat = WiredFormat::Wiredf;
	else
		wiredFormat = WiredFormat::Normalf;
	//we suppose the texture for the object is 2D and we are using just one texture 

	Shaders sh = shader.get()->getShader();

	glUseProgram(sh.program);

	glBindBuffer(GL_ARRAY_BUFFER, model->getNormalVBO());

	// we link the position attribute from  vertex shader
	if (sh.positionAttribute != -1)
	{
		glEnableVertexAttribArray(sh.positionAttribute);
		glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}

	// we link the uv attribute from vertex shader
	if (sh.uvAttribute != -1)
	{
		glEnableVertexAttribArray(sh.uvAttribute);
		glVertexAttribPointer(sh.uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(Vector3)));

	}
	
	// fog uniforms
	{
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
		Vector3 posCamera = SceneManager::getInstance()->getMainCamera().get()->getPositon();
		if (sh.u_cam_coord != -1)
		{
			glUniform3f(sh.u_cam_coord, posCamera.x, posCamera.y, posCamera.z);
		}
	}
	
	// we link the uniform MVP matrix from vertex shader
	Matrix mvp;
	Matrix proj = SceneManager::getInstance()->getMainCamera()->getProjectionMatrix();
	Matrix view = SceneManager::getInstance()->getMainCamera()->getViewMatrix();
	mvp = modelMatrix*view*proj;
	if (sh.u_model != -1)
	{
		glUniformMatrix4fv(sh.u_model, 1, GL_FALSE, (GLfloat*)modelMatrix.m);
	}
	if (sh.u_MVP != -1)
	{
		glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
	}

	if (sh.u_wired != -1)
	{
		GLfloat f = (wiredFormat == WiredFormat::Normalf) ? 0.f : 1.f;
		glUniform1f(sh.u_wired, f);
	}
	if (sh.u_Time != -1)
	{
		glUniform1f(sh.u_Time, time);
	}
	if (sh.u_DispMax != -1)
	{
		glUniform1f(sh.u_DispMax, u_DispMax);
	}
	int iii=0;
	for ( auto tex = textures.begin(); tex != textures.end(); tex++,iii++)
	{
		activeTextureUnitWithId(iii);
		glBindTexture(GL_TEXTURE_2D, (*tex).get()->getTextureVboId());
		if (sh.u_foc[iii] != -1)
		{
			glUniform1i(sh.u_foc[iii] ,iii);
		}
	}
	if (wiredFormat == WiredFormat::Normalf)
	{
		
		// we link the vertex buffer object for the indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.get()->getNormalEBO());
		int erro = glGetError();
		// and the draw triangles
		glDrawElements(GL_TRIANGLES, model.get()->getNormalNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);
		erro = glGetError();
		// close the element array buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// close the array buffer 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// unbind the texture 2d
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else // wired format
	{

		// we link the vertex buffer object for the indices


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.get()->getWiredEBO());

		// and the draw triangles
		glDrawElements(GL_LINES, model.get()->getWiredNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);
		//glDrawElements()

		// close the element array buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// close the array buffer 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	// here we draw the debug axes
	if (SceneManager::getInstance()->debugSettings.on == GL_TRUE)
	{
		//axes
		sh = model.get()->getAxesShader().get()->getShader();

		glUseProgram(sh.program);

		glBindBuffer(GL_ARRAY_BUFFER, model->getAxesVBO());
		// we link the position attribute from  vertex shader
		if (sh.positionAttribute != -1)
		{
			glEnableVertexAttribArray(sh.positionAttribute);
			glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
		}

		// we link the color attribute from vertex shader
		if (sh.colorAttribute != -1)
		{
			glEnableVertexAttribArray(sh.colorAttribute);
			glVertexAttribPointer(sh.colorAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), (void*)sizeof(Vector3));
		}
		if (sh.u_MVP != -1)
		{
			glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
		}
		glDrawArrays(GL_LINES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// normals
		glUseProgram(sh.program);
		glBindBuffer(GL_ARRAY_BUFFER, model->getNormalsVBO());
		// we link the position attribute from  vertex shader
		if (sh.positionAttribute != -1)
		{
			glEnableVertexAttribArray(sh.positionAttribute);
			glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
		}

		// we link the color attribute from vertex shader
		if (sh.colorAttribute != -1)
		{
			glEnableVertexAttribArray(sh.colorAttribute);
			glVertexAttribPointer(sh.colorAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), (void*)sizeof(Vector3));
		}
		if (sh.u_MVP != -1)
		{

			glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
		}
		glDrawArrays(GL_LINES, 0, model->getNormalsNumberOfVerticies());
		////////////////////////////////////////////////////////////////////////////////
		// AABB
		glUseProgram(sh.program);
		glBindBuffer(GL_ARRAY_BUFFER, model->getAABBVBO());
		// we link the position attribute from  vertex shader
		if (sh.positionAttribute != -1)
		{
			glEnableVertexAttribArray(sh.positionAttribute);
			glVertexAttribPointer(sh.positionAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
		}

		// we link the color attribute from vertex shader
		if (sh.colorAttribute != -1)
		{
			glEnableVertexAttribArray(sh.colorAttribute);
			glVertexAttribPointer(sh.colorAttribute, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), (void*)sizeof(Vector3));
		}
		if (sh.u_MVP != -1)
		{

			glUniformMatrix4fv(sh.u_MVP, 1, GL_FALSE, (GLfloat*)mvp.m);
		}
		glDrawArrays(GL_LINES, 0, model.get()->getAABBNumberOfVerticies());
	}
}

GLvoid FireObject::update()
{
	time += 0.01;
	if (time > 1)
	{
		time = 0;
		
	}
	Matrix Rz, Ry, Rx, T, S;
	Rz.SetRotationZ(toRadians(rotation.z));
	Ry.SetRotationY(toRadians(rotation.y));
	Rx.SetRotationX(toRadians(rotation.x));
	T.SetTranslation(position);
	S.SetScale(scale);
	// step 1 S,R (xml)
	modelMatrix = Rz * Ry * Rx * S * T;
}


FireObject::~FireObject()
{
}
