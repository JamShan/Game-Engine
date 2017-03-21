#pragma once
#include "../Utilities/Math.h"
#include "../Utilities/utilities.h"
// struct that holds information related to a vertex 
struct Vertex 
{
	Vector3 pos; // position in 3D
	Vector3 color; // color rgb
	Vector3 norm; // normal 
	Vector3 binorm; // binormal
	Vector3 tgt; //tanget
	Vector2 uv; // texture coordinates 
	Vertex() {};
	Vertex(Vector3 _p, Vector3 _n, Vector3 _b, Vector3 _t, Vector2 _uv) :pos(_p), norm(_n), binorm(_b), tgt(_t), uv(_uv) {};
	Vertex(const Vertex& rhs) { 
		pos = rhs.pos; 
		color = rhs.color; 
		norm = rhs.norm; 
		binorm = rhs.binorm; 
		tgt = rhs.tgt; 
		uv.x = rhs.uv.x;
		uv.y = rhs.uv.y;
	};
};
// struct that holds information related to a vertex build for the terrain object
struct Vertexnou
{
	Vector3 pos; // position
	Vector2 uvsmall; // small texture
	Vector2 uvlarge;	// large texture
	Vector3 norm;// normal
	Vertexnou() {}
	// copy ctor
	Vertexnou(const Vertexnou& other) :pos(other.pos){
		uvsmall.x = other.uvsmall.x;
		uvsmall.y = other.uvsmall.y;
		uvlarge.x = other.uvlarge.x;
		uvlarge.y = other.uvlarge.y;
		norm = other.norm;
	}
	
};