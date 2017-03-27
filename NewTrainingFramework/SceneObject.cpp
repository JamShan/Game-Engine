#include "stdafx.h"
#include "SceneObject.h"

#include "HelperFunctions.h"
#include "SceneManager.h"

GLvoid SceneObject::sendToShader(Shaders &sh)
{
	glUseProgram(sh.program);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// lightss
	// we now can send light stuff
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
	if (sh.u_wired != -1)
	{
		GLfloat f = (wiredFormat == WiredFormat::Normalf) ? 0.f : 1.f;
		glUniform1f(sh.u_wired, f);
	}
}

GLvoid SceneObject::drawDebug(Matrix& mvp)
{
	Shaders sh = model.get()->getAxesShader().get()->getShader();

	glUseProgram(sh.program);
	//r g b axes
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
	////////////////////////////////////////////////////////////////////////////////
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
}

GLvoid SceneObject::draw()
{	
	
	// if debug settings is on we draw everything wired and we also draw the debug axes
	if (SceneManager::getInstance()->debugSettings.on == GL_TRUE)
		wiredFormat = WiredFormat::Wiredf;
	else
		wiredFormat = WiredFormat::Normalf;
	//we suppose the texture for the object is 2D and we are using just one texture 
	
	Shaders sh = shader.get()->getShader();
	glUseProgram(sh.program);
	sendToShader(sh);
	////////////////////////////////////////////////////
	glBindBuffer(GL_ARRAY_BUFFER, model.get()->getNormalVBO()); 
	
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
	
	if (sh.a_normal != -1)
	{
		glEnableVertexAttribArray(sh.a_normal);
		glVertexAttribPointer(sh.a_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (2*sizeof(Vector3) ) );
	}
	if (sh.a_binorm != -1)
	{
		glEnableVertexAttribArray(sh.a_binorm);
		glVertexAttribPointer(sh.a_binorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3* sizeof(Vector3)));
	}
	if (sh.a_tgt != -1)
	{
		glEnableVertexAttribArray(sh.a_tgt);
		glVertexAttribPointer(sh.a_tgt, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(4* sizeof(Vector3)));
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
	
	// if we are using a normal texture we enter here
	if (sh.u_texture != -1&& textures.size()>0)
	{
		// we active the texture we are working with, for the moment we are just using one texture
		activeTextureUnitWithId(0);
		glBindTexture(GL_TEXTURE_2D, textures[0].get()->getTextureVboId());
		glUniform1i(sh.u_texture, 0);
		
	}
	
	// if we are using a cube texture there are 2 cases 
	// 1 we either reflect this object completly from sybox
	// 2 we reflect from skybox and we also use it's texture and that's whhy use use an auto texture because in the second position
	// in the textures vector we store out cube texture and so we need to activate them both
	// if we are reflecting the object using the skybox
	if (textures.size() >= 1)
	{
		if (sh.u_cub_texture != -1)
		{
			// we active the texture we are working with, for the moment we are just using one texture
			activeTextureUnitWithId(1);
			if (textures.size() == 1)
				glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].get()->getTextureVboId());
			else
				glBindTexture(GL_TEXTURE_CUBE_MAP, textures[1].get()->getTextureVboId());
			glUniform1i(sh.u_cub_texture, 1);
		}
		else
		{
			// we active the texture we are working with, for the moment we are just using one texture
			activeTextureUnitWithId(1);
			if (textures.size() == 2)
			{
				glBindTexture(GL_TEXTURE_2D, textures[1].get()->getTextureVboId());
				glUniform1i(sh.u_normalMapTex, 1);
			}
		}
	}
	
	

	if (wiredFormat == WiredFormat::Normalf)
	{

		
		// we link the vertex buffer object for the indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.get()->getNormalEBO());
	
															   // and the draw triangles
		glDrawElements(GL_TRIANGLES, model.get()->getNormalNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);
		
		// close the element array buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// close the array buffer 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// unbind the texture 2d
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else // wired format
	{

		// we link the vertex buffer object for the indices
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.get()->getWiredEBO());
		
																   // and the draw triangles
		glDrawElements(GL_LINES, model->getWiredNumberOfIndicies(), GL_UNSIGNED_SHORT, 0);
		//glDrawElements()
		
		// close the element array buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// close the array buffer 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// here we draw the debug axes and normals and AABB
		drawDebug(mvp);
	}		
}

GLvoid SceneObject::activeTextureUnitWithId(GLuint id)
{
	//glActiveTexture(GL_TEXTURE + id);
	//return;
	switch (id)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1: glActiveTexture(GL_TEXTURE1);
		break;
	case 2:glActiveTexture(GL_TEXTURE2);
		break;
	case 3:glActiveTexture(GL_TEXTURE3);
		break;
	case 4: glActiveTexture(GL_TEXTURE4);
		break;
	case 5: glActiveTexture(GL_TEXTURE5);
		break;
	case 6: glActiveTexture(GL_TEXTURE6);
		break;
	case 7:glActiveTexture(GL_TEXTURE7);
		break;
	case 8: glActiveTexture(GL_TEXTURE8);
		break;
	case 9: glActiveTexture(GL_TEXTURE9);
		break;
	case 10: glActiveTexture(GL_TEXTURE10);
		break;
	case 11: glActiveTexture(GL_TEXTURE11);
		break;
	case 12: glActiveTexture(GL_TEXTURE12);
		break;
	case 13: glActiveTexture(GL_TEXTURE13);
		break;
	case 14: glActiveTexture(GL_TEXTURE14);
		break;
	case 15: glActiveTexture(GL_TEXTURE15);
		break;
	case 16: glActiveTexture(GL_TEXTURE16);
		break;
		//for now this is it
	}
}

GLvoid SceneObject::updateModelMatrix()
{
	Matrix Rz,Ry,Rx,T,S;
	Rz.SetRotationZ(toRadians(rotation.z));
	Ry.SetRotationY(toRadians(rotation.y));
	Rx.SetRotationX(toRadians(rotation.x));
	T.SetTranslation(position);
	S.SetScale(scale);
	// step 1 S,R (xml)
	modelMatrix = Rz * Ry * Rx * S;
	if (trajectoryInfo.has)
	{
		if (trajectoryInfo.trajectory->type == Trajectory::Type::Circle)
		{
			CircleTrajectory* cr = (CircleTrajectory*)(trajectoryInfo.trajectory);
			// step 2 translate
			
			modelMatrix = modelMatrix*(cr->rotVec);// this should rotate the circle this making the points of the circle on a certain plane but just rotates the model
			
		}
		else if (trajectoryInfo.trajectory->type == Trajectory::LineStrip || // for direction alternate we rotate around Y axis with 180 degrees
			     trajectoryInfo.trajectory->type == Trajectory::Linear)
		{
			if (trajectoryInfo.direction != "normal")
			{
				
				modelMatrix = modelMatrix *trajectoryInfo.mrotateY180;
			}
		}
	}
	// step 4 translate circle center
	modelMatrix = modelMatrix * T;
}

GLvoid SceneObject::update()
{
	
	GLfloat deltaTime = SceneManager::getInstance()->getMainCamera()->getDeltaTime();
	// trajectory stuff
	if (trajectoryInfo.has)
	{
		trajectoryInfo.execute(deltaTime);
		position = trajectoryInfo.getPosition();
	}
	
	updateModelMatrix();
}

GLboolean SceneObject::collides(std::shared_ptr<SceneObject> other)
{
	Vector4 unit1(1.0, 1.0, 1.0, 1.0), unit2 = unit1;
	unit1 = unit1*modelMatrix;
	AABB box1 = model.get()->boundingBox;
	box1.Xmax += unit1.x;
	box1.Xmin += unit1.x;
	box1.Ymax += unit1.y;
	box1.Ymin += unit1.y;
	box1.Zmax += unit1.z;
	box1.Zmin += unit1.z;
	
	AABB box2 = other->model.get()->boundingBox;
	unit2 = unit2*other->modelMatrix;
	box2.Xmax += unit2.x;
	box2.Xmin += unit2.x;
	box2.Ymax += unit2.y;
	box2.Ymin += unit2.y;
	box2.Zmax += unit2.z;
	box2.Zmin += unit2.z;
	return box1.collides(box2);
}

GLboolean SceneObject::collides(AABB other)
{
	Vector4 unit1(1.0, 1.0, 1.0, 1.0);
	unit1 = unit1*modelMatrix;
	AABB box1 = model.get()->boundingBox;
	box1.Xmax += unit1.x;
	box1.Xmin += unit1.x;
	box1.Ymax += unit1.y;
	box1.Ymin += unit1.y;
	box1.Zmax += unit1.z;
	box1.Zmin += unit1.z;
	return box1.collides(other);
}


SceneObject::SceneObject()
{
}
// terrain ctor
SceneObject::SceneObject(
	ObjectType _objectType, 
	std::shared_ptr<Model> _model,
	std::shared_ptr<Shader> _shader, 
	std::vector<std::shared_ptr<Texture>> _textures, 
	std::string _name, 
	WiredFormat _wiredFormat,
	std::vector<GLuint> _lightsId,
	GLfloat _difCoef,
	GLfloat _specCoef):
	type(_objectType),
	model(_model),
	shader(std::move(_shader)),
	textures(std::move(_textures)),
	name(_name),
	wiredFormat(_wiredFormat),
	lightsId(std::move(_lightsId)),
	diffCoef(_difCoef),
	specCoef(_specCoef)
{
	model.get()->createAxes();
	model.get()->createNormal(scale.x);
	model.get()->createAABB();
}
// skybox ctor
SceneObject::SceneObject(ObjectType _objectType,
	std::shared_ptr<Model> _model, 
	std::shared_ptr<Shader> _shader, 
	std::vector<std::shared_ptr<Texture>> _textures,
	std::string _name,
	Vector3 _scale):
	type(_objectType),
	model(_model),
	shader(std::move(_shader)),
	textures(std::move(_textures)),
	name(_name),
	scale(_scale)
{
}
// fire object ctor
SceneObject::SceneObject(
	ObjectType _objectType,
	std::shared_ptr<Model> _model,
	std::shared_ptr<Shader> _shader, 
	std::vector<std::shared_ptr<Texture>> _textures,
	Vector3 _position,
	Vector3 _initialPosition,
	Vector3 _scale, 
	Vector3 _rotation,
	std::string _name,
	WiredFormat _wiredFormat,
	TrajectoryInfo _trajectoryInfo
	):
	type(_objectType),
	model(_model),
	shader(std::move(_shader)),
	textures(std::move(_textures)),
	position(_position),
	initialPosition(_position),
	scale(_scale),
	rotation(_rotation),
	name(_name),
	wiredFormat(_wiredFormat)
{
	model.get()->createAxes();
	model.get()->createNormal(scale.x);
	model.get()->createAABB();
}
// scene object ctor
SceneObject::SceneObject(
	ObjectType _objectType,
	std::shared_ptr< Model > _model,
	std::shared_ptr< Shader > _shader,
	std::vector< std::shared_ptr< Texture > > _textures,
	Vector3 _position,
	Vector3 _initialPosition,
	Vector3 _scale,
	Vector3 _rotation,
	std::string _name,
	WiredFormat _wiredFormat,
	TrajectoryInfo _trajectoryInfo,
	std::vector<GLuint> _lightsId,
	GLfloat _difCoef,
	GLfloat _specCoef
	):
	type(_objectType),
	model(_model),
	shader(std::move(_shader)),
	textures(std::move(_textures)),
	position(_position),
	initialPosition(_position),
	scale(_scale),
	rotation(_rotation),
	name(_name),
	wiredFormat(_wiredFormat),
	trajectoryInfo(_trajectoryInfo)
{
	model.get()->createNormal(scale.x);
	model.get()->createAxes();
	model.get()->createAABB();
	lightsId = std::move(_lightsId);
	diffCoef = _difCoef;
	specCoef = _specCoef;
	if (trajectoryInfo.type == "linear")
	{
		trajectoryInfo.trajectory= new LinearTrajectory(position, trajectoryInfo.endPoint);
	}
	else if (trajectoryInfo.type == "line_strip")
	{
		trajectoryInfo.trajectory = new LineStripTrajectory(trajectoryInfo.points);
	}
	else if (trajectoryInfo.type == "line_loop")
	{
		trajectoryInfo.trajectory = new LineLoopTrajectory(trajectoryInfo.points);
	}
	else if (trajectoryInfo.type == "circle")
	{
		trajectoryInfo.trajectory = new CircleTrajectory(position,
			trajectoryInfo.rotationPlane.x,
			trajectoryInfo.rotationPlane.y,
			trajectoryInfo.rotationPlane.z,
			trajectoryInfo.radius);
		CircleTrajectory* traj = (CircleTrajectory*)trajectoryInfo.trajectory;
		traj->center = position;
		trajectoryInfo.trajectory = traj;
	}
	
	updateModelMatrix();
}
SceneObject::~SceneObject()
{
	name.clear();
	for (auto it = textures.begin(); it != textures.end(); it++)
	{
		it->reset();
		*it = nullptr;
	}
	shader.reset();
	shader = nullptr;
	model.reset();
	model = nullptr;
}
