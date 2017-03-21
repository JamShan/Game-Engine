#pragma once
#include <vector>
#include <memory>


#include "Vertex.h"	
#include "HelperStructs.h"
#include "Shader.h"

#include "../Utilities/Math.h"
#include "../Utilities/utilities.h"// for stl use
/*
	Model class loads a 3D model or generated indicies for a terrain or for debug axes 
*/
class Model
{
	
public:
	AABB boundingBox;
	Vector3 AABBcolor;
private:
	/////////////////////////////////////////////////////////////////////
	// terrain stuff
	// number of cells
	GLuint cells;
	// width of a cell
	GLfloat acellWidth;
	// terrain data
	std::vector<Vertexnou> data;
public:
	// large texture.x coordinate +=dx same for y coordinate
	GLvoid updateTexture(GLfloat dx, GLfloat dy);
	// generate model with cellNumber cells and cellWith cell with
	GLvoid generate(GLint cellNumber, GLfloat cellWidth);
private:
	/////////////////////////////////////////////////////////////////////

	// axes data
	// object axes 
	Vector3 X = Vector3(1, 0, 0), Y = Vector3(0, 1, 0), Z = Vector3(0, 0, 1), O = Vector3(0, 0, 0);
	// buffer for axes
	GLuint VboObjAxes; //GL_ARRAY_BUFFER
	
	
public:
	// axes colors
	std::vector<Vector3> axesColors;
	// shader for axes
	std::shared_ptr<Shader> axesShader;
	// create axes for debug drawing
	GLvoid createAxes();
	GLvoid createNormal(GLfloat scale);
	GLvoid createAABB();
private:
	
	// id's used for indices of normal draw and wired draw, and vertexes
	GLuint
		ivboId,     // for normal GL_ELEMENT_ARRAY_BUFFER
		wiredvboId, // for wired GL_ELEMENT_ARRAY_BUFFER
		vvboId; // GL_ARRAY_BUFFER for default
				// normals GL_ARRAY_BUFFER
	GLuint nvboId;
	// AABB
	GLuint aabbvboId;
	GLuint aabbVerticies;


	//////////////////////////////////////////////////////////////////////
	// model resource location in Models folder
	ModelResource mr;
	// model id
	GLuint id;

	// functions used to load the model from a physical file
	GLvoid jump(std::string & s, int & p, char c);
	GLvoid skip(std::string & s, int & p);
	GLfloat getNumberLine(std::string & line, int & p);
	GLvoid parseVertexLine(std::string & line, std::vector<std::unique_ptr<Vertex>>& v);
	GLvoid parseIndiciesLine(std::string & line, std::vector<std::unique_ptr<GLushort>>& ind);
	GLvoid parseNFG(std::string fileName, std::vector<std::unique_ptr<Vertex>> &v, std::vector<std::unique_ptr<GLushort>> &ind);
	

	
	std::vector<Vertex> vertecsi;
	//getters
	// number of indices for normal GL_ELEMENT_ARRAY_BUFFER draw that means we should double them for wired and normals draw
	GLuint nrIndicies;
	GLuint nrVertecsiNormals;
public:

	// number of indicies for GL_ELEMENT_ARRAY_BUFFER
	GLuint getNormalNumberOfIndicies()const;
	GLuint getWiredNumberOfIndicies()const;
	GLuint getNormalsNumberOfVerticies() const;
	GLuint getAABBNumberOfVerticies()const;
	// GL_ELEMENT_ARRAY_BUFFER element buffer object
	GLuint getWiredEBO()const;
	GLuint getNormalEBO()const;
	// GL_ARRAY_BUFFER array buffer object
	GLuint getNormalsVBO()const; 
	GLuint getAABBVBO()const;
	GLuint getNormalVBO()const;
	GLuint getAxesVBO()const;

	// axes shader 
	std::shared_ptr<Shader> getAxesShader();
	// load the indices and verticies in memory and get the indicies and verticies vertex buffer object ids  
	GLvoid load();
	
	Model();
	// set the model id;
	Model(GLuint);
	~Model();
};

