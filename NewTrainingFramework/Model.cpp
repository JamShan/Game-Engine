#include "stdafx.h"
#include "Model.h"

#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

#include <iostream>
#include "SceneManager.h"
#include "HelperFunctions.h"
#include "ResourceManager.h"

#include "../Utilities/utilities.h"


GLvoid Model::updateTexture(GLfloat dx, GLfloat dy)
{
	GLint index = 0;
	GLint half = cells / 2;
	for (GLint i = - half; i <= half; i++)
		for (GLint j = -half; j <= half; j++)
		{
			data[index].uvlarge.x += dx;
			data[index].uvlarge.y += dy;
			index++;
		}
	
	//glGenBuffers(1, &vvboId);
	glBindBuffer(GL_ARRAY_BUFFER, vvboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(Vertexnou), data.data());
	//glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertexnou), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// this function generates a terrain model having as input the number of cells and the cells width
GLvoid Model::generate(GLint cellNumber,GLfloat cellWidth)
{	
	cells = cellNumber;
	acellWidth = cellWidth;
	
	GLfloat currentIndexj = 0,currentIndexi=0;
	for (int i = -cellNumber/2; i <= cellNumber/2; i++,currentIndexi++)
	{
		currentIndexj = 0;
		for (int j = -cellNumber/2; j <= cellNumber/2; j++, currentIndexj++)
		{

			Vertexnou v;
			v.pos.y = 0;
			v.pos.x = i*cellWidth;
			v.pos.z = j*cellWidth;
			v.uvlarge.x = currentIndexj / cellNumber;
			v.uvlarge.y = currentIndexi / cellNumber;
			v.uvsmall.x = currentIndexj;
			v.uvsmall.y = currentIndexi;
			v.norm.x = 0;
			v.norm.y = 1;
			v.norm.z = 0;
			data.push_back(v);
		}
	}

	// debug axes data and bounding box
	boundingBox.Ymax = boundingBox.Zmax = boundingBox.Xmax = -1000000;
	boundingBox.Ymin = boundingBox.Zmin = boundingBox.Xmin = 1000000;

	for (int i = 0; i < data.size(); i++)
	{
		// AABB
		boundingBox.Xmin = min(boundingBox.Xmin, data[i].pos.x);
		boundingBox.Xmax = max(boundingBox.Xmax, data[i].pos.x);
		boundingBox.Ymin = min(boundingBox.Ymin, data[i].pos.y);
		boundingBox.Ymax = max(boundingBox.Ymax, data[i].pos.y);
		boundingBox.Zmin = min(boundingBox.Zmin, data[i].pos.z);
		boundingBox.Zmax = max(boundingBox.Zmax, data[i].pos.z);
		// axes
		O.x += data[i].pos.x;
		O.y += data[i].pos.y;
		O.z += data[i].pos.z;
		if (data[i].pos.x > X.x)
			X.x = data[i].pos.x;
		if (data[i].pos.y > Y.y)
			Y.y = data[i].pos.y;
		if (data[i].pos.z > Z.z)
			Z.z = data[i].pos.z;
	}
	O.x /= data.size(); O.y /= data.size(); O.z /= data.size();
	X.y = Z.y = O.y;
	Y.z = X.z = O.z;
	Y.x = Z.x = O.x;
	// end debug data
	std::vector<GLushort> terrainIndicies;
	// generate indicies
	for (int i = 0; i < cellNumber; i++)
	{
		for (int j = 0; j < cellNumber; j++)
		{
			// first triangle
			terrainIndicies.push_back((cellNumber+1)*i+j); // stanga sus
			terrainIndicies.push_back((cellNumber + 1)*(i + 1)+j); // stanga jos
			terrainIndicies.push_back((cellNumber + 1)*i+ j+ 1);// dreapta sus
			// second triangle
			terrainIndicies.push_back((cellNumber + 1)*(i + 1)+j);// stanga jos
			terrainIndicies.push_back((cellNumber + 1)*i +j + 1);// dreapta sus
			terrainIndicies.push_back((cellNumber + 1)*(i + 1)+j + 1);//dreapta jos
		}
	}

	nrIndicies = terrainIndicies.size();
	// buffer for vertexes
	glGenBuffers(1, &vvboId);
	glBindBuffer(GL_ARRAY_BUFFER, vvboId);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertexnou), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// buffer for model indices's
	glGenBuffers(1, &ivboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndicies.size() * sizeof(GLushort), terrainIndicies.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	GLint eroare = glGetError();
}
GLvoid Model::createAxes()
{
	// shader with id = 3 is axes shader
	axesShader = ResourceManager::getInstance()->loadShader(3);
	std::vector<Vector3> objAxesdata;
	objAxesdata.push_back(O);
	objAxesdata.push_back(axesColors[0]);
	objAxesdata.push_back(X);
	objAxesdata.push_back(axesColors[0]);
	objAxesdata.push_back(O);
	objAxesdata.push_back(axesColors[1]);
	objAxesdata.push_back(Y);
	objAxesdata.push_back(axesColors[1]);
	objAxesdata.push_back(O);
	objAxesdata.push_back(axesColors[2]);
	objAxesdata.push_back(Z);
	objAxesdata.push_back(axesColors[2]);

	glGenBuffers(1, &VboObjAxes);
	glBindBuffer(GL_ARRAY_BUFFER, VboObjAxes);
	glBufferData(GL_ARRAY_BUFFER, objAxesdata.size() * sizeof(Vector3), objAxesdata.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}
GLvoid Model::createNormal(GLfloat scale)
{
	// create normals
	std::vector<Vector3> normData;
	for (auto it = std::begin(vertecsi); it != std::end(vertecsi); ++it)
	{
		Vector3 begin = it->pos;
		Vector3 color = SceneManager::getInstance()->getNormalColor();
		Vector3 end = begin;
		end += it->norm * SceneManager::getInstance()->getNormalLength()/ scale;
		normData.push_back(begin);
		normData.push_back(color);
		normData.push_back(end);
		normData.push_back(color);
	}
	glGenBuffers(1, &nvboId);
	glBindBuffer(GL_ARRAY_BUFFER, nvboId);
	glBufferData(GL_ARRAY_BUFFER, normData.size() * sizeof(Vector3), normData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	nrVertecsiNormals = normData.size() / 2;
	normData.clear();
}
GLvoid Model::createAABB()
{
	// we need to draw 12 lines , 4 for each side and 4 for the lin between thoses 2 
	std::vector<Vector3> AABBdata;
	// first right side
	Vector3 A,B,C,D,E,F,G,H;

			/*			3	
				 G------------------B
				/|				   /|
			   / |				  / |
			  /  |				 /  |
then this->  /   |	 2	        /   |
			F----|-------------A    |
			|	 |		4	   |   	|
			|	 H------------------C  <-  we first make this face
			|    /   		   |	/     
			|   /              |   /      
			|  /			   |  /
			| /                | /
			|/       1         |D
			E-------------------
	and then the 4 lines , 2 bottom and 2 up numbered from 1 to 4
	*/



	A.x = boundingBox.Xmax;
	A.y = boundingBox.Ymax;
	A.z = boundingBox.Zmax;
	F = A; F.x = boundingBox.Xmin;
	B = A; B.z = boundingBox.Zmin;
	G = B; G.x = boundingBox.Xmin;
	C = B; C.y = boundingBox.Ymin;
	H = C; H.x = boundingBox.Xmin;
	D = C; D.z = boundingBox.Zmax;
	E = D; E.x = boundingBox.Xmin;
	Vector3 lines[] = { A,B,B,C,C,D,D,A,F,G,G,H,H,E,E,F,E,D,F,A,G,B,H,C };
	for (int i = 0; i < 24; i++)
	{
		AABBdata.push_back(lines[i]);
		AABBdata.push_back(AABBcolor);
	}
	aabbVerticies = 24;
	glGenBuffers(1, &aabbvboId);
	glBindBuffer(GL_ARRAY_BUFFER, aabbvboId);
	glBufferData(GL_ARRAY_BUFFER, AABBdata.size() * sizeof(Vector3), AABBdata.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
GLvoid Model::load() // load the model from file
{

	std::vector<GLushort> indicies;

	std::vector<std::unique_ptr<Vertex>> v;
	std::vector<std::unique_ptr<GLushort>> ind;
	std::vector<GLushort> wiredIndicies;
	std::string path("../");
	path += mr.path;
	parseNFG(path, v, ind);
	// put the date from parsing into 2  vectors
	for (auto it = ind.begin(); it != ind.end(); it++)
		indicies.push_back(*(*it).get());
	for (auto it = v.begin(); it != v.end(); ++it)
		vertecsi.push_back(*(*it).get());
	// find data for debug axes
	
	boundingBox.Ymax = boundingBox.Zmax = boundingBox.Xmax = -1000000;
	boundingBox.Ymin = boundingBox.Zmin = boundingBox.Xmin = 1000000;
	for (int i = 0; i < vertecsi.size(); i++)
	{
		// AABB
		boundingBox.Xmin = min(boundingBox.Xmin, vertecsi[i].pos.x);
		boundingBox.Xmax = max(boundingBox.Xmax, vertecsi[i].pos.x);
		boundingBox.Ymin = min(boundingBox.Ymin, vertecsi[i].pos.y);
		boundingBox.Ymax = max(boundingBox.Ymax, vertecsi[i].pos.y);
		boundingBox.Zmin = min(boundingBox.Zmin, vertecsi[i].pos.z);
		boundingBox.Zmax = max(boundingBox.Zmax, vertecsi[i].pos.z);
		//
		O.x += vertecsi[i].pos.x;
		O.y += vertecsi[i].pos.y;
		O.z += vertecsi[i].pos.z;
		if (vertecsi[i].pos.x > X.x)
			X.x = vertecsi[i].pos.x;
		if (vertecsi[i].pos.y > Y.y)
			Y.y = vertecsi[i].pos.y;
		if (vertecsi[i].pos.z > Z.z)
			Z.z = vertecsi[i].pos.z;
	}
	O.x /= vertecsi.size(); O.y /= vertecsi.size(); O.z /= vertecsi.size();
	X.y = Z.y = O.y;
	Y.z = X.z = O.z;
	Y.x = Z.x = O.x;
	// wired indicies
	for (auto it = ind.begin(); it != ind.end();it+=3)
	{
		wiredIndicies.push_back( *(*it).get());
		it++;
		wiredIndicies.push_back(*(*it).get());
		wiredIndicies.push_back(*(*it).get());
		it++;
		wiredIndicies.push_back(*(*it).get());
		wiredIndicies.push_back(*(*it).get());
		it -= 2;
		wiredIndicies.push_back(*(*it).get());
	}
	// buffer for wired draw
	
	glGenBuffers(1, &wiredvboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wiredvboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, wiredIndicies.size()* sizeof(GLushort), wiredIndicies.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	nrIndicies = ind.size();
	// buffer for model indices's
	glGenBuffers(1,&ivboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLushort), indicies.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// buffer for vertexes
	glGenBuffers(1, &vvboId);
	glBindBuffer(GL_ARRAY_BUFFER, vvboId);
	glBufferData(GL_ARRAY_BUFFER, vertecsi.size() * sizeof(Vertex), vertecsi.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// clear the vertexes and indexes from vectors
	for (auto it = ind.begin(); it != ind.end(); it++)
	{
		it->reset();
		*it = nullptr;
	}
	ind.clear();
	for (auto it = v.begin(); it != v.end(); ++it)
	{
		it->reset();
		*it = nullptr;
	}
	v.clear();

	wiredIndicies.clear();
	indicies.clear();
	
}

///////////////////////////////////////////////////////////////////////////////////////////
void Model::jump(std::string &s, int &p, char c) // skip after character c
{
	while (s[p] != c)
		p++;
	p++;
}

void Model::skip(std::string &s, int&p)	// skip spaces
{
	while (s[p] == ' ')
		p++;
}

GLfloat Model::getNumberLine(std::string &line, int &p)		// take a number from a line
{

	while (!((bool)isdigit((int)line[p]) || line[p] == '-')) // while not at the beginning of a number
		p++;
	GLfloat toReturn = helperfunctions::stringToNumber<GLfloat>(std::string(line, p));
	while ((bool)isdigit((int)line[p]) || line[p] == '-' || line[p] == '.') // while i don't have
		p++;
	return toReturn;
}

void Model::parseVertexLine(std::string &line, std::vector<std::unique_ptr<Vertex>> &v)	// parse a line for each vertex
{
#define numbermodelparser  getNumberLine(line, p)
	using v3 = Vector3;
	using v2 = Vector2;
	int p = 0;
	jump(line, p, ':');
	GLfloat vec3[14];
	for (int i = 0; i < 14; i++)
		vec3[i] = numbermodelparser;
	v3 ver[4];
	for (int i = 0; i < 4; i++)
		ver[i] = v3(vec3[i * 3], vec3[i * 3 + 1], vec3[i * 3 + 2]);
	v2 uv(vec3[12], vec3[13]);
	v.push_back(std::make_unique<Vertex>(ver[0], ver[1], ver[2], ver[3], uv));
}

void Model::parseIndiciesLine(std::string &line, std::vector<std::unique_ptr<GLushort>> &ind)
{
#define addAnotherIndex ind.push_back(std::make_unique<GLushort>(helperfunctions::stringToNumber<GLushort>( std::string(line, p))));
#define skipDigits while ((bool)isdigit((int)line[p])) p++;
	int p = 0;
	jump(line, p, '.');
	for (int i = 0; i < 3; i++)
	{
		p++;
		skip(line, p);
		addAnotherIndex
			skipDigits
	}
}

void Model::parseNFG(std::string fileName, std::vector<std::unique_ptr<Vertex>> &v, std::vector<std::unique_ptr<GLushort>> &ind) // parse  .nfg
{
	std::ifstream in(fileName);
	std::string line;
	std::getline(in, line);				// read first line
	int p = 0;
	jump(line, p, ':');
	int verticiesNr = helperfunctions::stringToNumber<int>(std::string(line, p)); // take number from it
	for (int i = 0; i < verticiesNr; i++)			// for each line
	{
		std::getline(in, line);
		parseVertexLine(line, v);
	}
	std::getline(in, line);
	jump(line, p = 0, ':');
	int indiciesNr = helperfunctions::stringToNumber<int>(std::string(line, p));
	for (int i = 0; i < indiciesNr / 3; i++)
	{
		std::getline(in, line);
		parseIndiciesLine(line, ind);
	}
}


Model::Model()
{

}
Model::Model(GLuint _id):id(_id)
{
	mr = ResourceManager::getInstance()->modelResources[id];
}

Model::~Model() // delete the used buffers
{
	if (glIsBuffer(ivboId)== GL_TRUE)
		glDeleteBuffers(1, &ivboId);
	if (glIsBuffer(vvboId) == GL_TRUE)
		glDeleteBuffers(1, &vvboId);
	if (glIsBuffer(wiredvboId) == GL_TRUE)
		glDeleteBuffers(1, &wiredvboId);
}

GLuint Model::getNormalNumberOfIndicies() const
{
	return nrIndicies;
}

GLuint Model::getWiredNumberOfIndicies() const
{
	return nrIndicies * 2;
}

GLuint Model::getNormalsNumberOfVerticies() const
{
	return nrVertecsiNormals;
}

GLuint Model::getAABBNumberOfVerticies() const
{
	return aabbVerticies;
}

GLuint Model::getWiredEBO() const
{
	return wiredvboId;
}

GLuint Model::getNormalEBO() const
{
	return ivboId;
}

GLuint Model::getNormalsVBO() const
{
	return nvboId;
}

GLuint Model::getAABBVBO() const
{
	return aabbvboId;
}

GLuint Model::getNormalVBO() const
{
	return vvboId;
}

GLuint Model::getAxesVBO() const
{
	return VboObjAxes;
}

std::shared_ptr<Shader> Model::getAxesShader()
{
	return axesShader;
}

