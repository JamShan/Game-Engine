#include <stdafx.h>
#include "Shaders.h"

#include <string>

GLint Shaders::Init(GLchar * fileVertexShader, GLchar * fileFragmentShader)
{
	vertexShader = esLoadShader(GL_VERTEX_SHADER, fileVertexShader);

	if ( vertexShader == 0 )
		return -1;

	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fileFragmentShader);

	if ( fragmentShader == 0 )
	{
		glDeleteShader( vertexShader );
		return -2;
	}

	program = esLoadProgram(vertexShader, fragmentShader);

	//finding location of uniforms / attributes
	// attributees
	positionAttribute = glGetAttribLocation(program, "a_posL"); // vertex properties
	colorAttribute = glGetAttribLocation(program, "a_color");	//color
	uvAttribute = glGetAttribLocation(program, "a_uv");		// uv for texture coordinates
	v_uv_small = glGetAttribLocation(program, "a_uvsmall");
	v_uv_large = glGetAttribLocation(program, "a_uvlarge");
	a_normal = glGetAttribLocation(program, "a_norm");
	a_binorm = glGetAttribLocation(program, "a_binorm");
	a_tgt = glGetAttribLocation(program, "a_tgt");
	// uniforms

	// generic uniforms
	u_wired = glGetUniformLocation(program, "u_wired");
	u_MVP = glGetUniformLocation(program, "u_MVP");				
	u_texture = glGetUniformLocation(program, "u_texture");
	u_normalMapTex = glGetUniformLocation(program, "u_NormalMapTexture");
	// skybox uniform
	u_cub_texture = glGetUniformLocation(program, "u_cube_texture");
	
	
	// fog uniforms
	u_fog = glGetUniformLocation(program, "u_fog_color");
	u_r_radius = glGetUniformLocation(program, "u_r");
	u_R_radius = glGetUniformLocation(program, "u_R");
	u_cam_coord = glGetUniformLocation(program, "u_cameraCoord");
	u_model = glGetUniformLocation(program, "u_model");

	// terrain uniforms
	u_height = glGetUniformLocation(program, "u_height");
	u_blendTex = glGetUniformLocation(program, "u_tex_blend");
	u_grassTex= glGetUniformLocation(program, "u_tex_grass");
	u_dirtTex = glGetUniformLocation(program, "u_tex_dirt");
	u_rockTex = glGetUniformLocation(program, "u_tex_rock");
	// fire uniforms
	u_foc[0] = glGetUniformLocation(program, "u_fire");
	u_foc[1] = glGetUniformLocation(program, "u_disp");
	u_foc[2] = glGetUniformLocation(program, "u_mask");
	
	u_Time = glGetUniformLocation(program, "u_Time");
	u_DispMax = glGetUniformLocation(program, "u_DispMax");

	// light stuff
	u_ambientalColor = glGetUniformLocation(program, "u_ambientalColor");
	// per object data
	u_specularCoeficient = glGetUniformLocation(program, "u_specularCoeficient");
	u_diffuseCoeficient = glGetUniformLocation(program, "u_diffuseCoeficient");
	u_ratio = glGetUniformLocation(program, "u_ratio");

	std::string difCol("diffuseColor");
	std::string specCol("specularColor");
	std::string shines("shiness");
	std::string posi("position");
	std::string dire("direction");
	std::string type("Type");
	// we assume an object can have maximum 4 lights associated
	for (int i = 0; i < 4; i++)
	{
		std::string lights = "u_lights[" +std::to_string(i)+ + "].";
		u_lights[i].diffuseColor = glGetUniformLocation(program, (lights+difCol).data());
		u_lights[i].specularColor = glGetUniformLocation(program, (lights + specCol).data());
		u_lights[i].position = glGetUniformLocation(program, (lights + posi).data());
		u_lights[i].direction = glGetUniformLocation(program, (lights + dire).data());
		u_lights[i].shiness = glGetUniformLocation(program, (lights + shines).data());
		u_lights[i].Type = glGetUniformLocation(program, (lights + type).data());
	}

	
	return 0;
}

Shaders::~Shaders()
{
	//glDeleteProgram(program);
	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);
}