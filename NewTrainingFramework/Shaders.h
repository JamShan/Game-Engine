#pragma once
#include "../Utilities/utilities.h"
class Shaders 
{
public:
	GLuint program, vertexShader, fragmentShader;

	GLint positionAttribute,
		colorAttribute,
		u_MVP,
		modelMatrix,
		viewMatrix,
		projectionMatrix,
		u_texture,
		u_texture2,
		uvAttribute,
		u_wired,
		u_normalMapTex,
		// skybox data
		u_cub_texture,
		// terrain data
		u_height,
		u_rockTex,
		u_grassTex,
		u_dirtTex,
		u_blendTex,
		v_uv_large,
		v_uv_small,
		// fog data
		u_fog,
		u_r_radius,
		u_R_radius,
		u_cam_coord,
		u_model,
		// fire data
		u_foc[3],
		u_Time,
		u_DispMax,
		// data for reflexion
		a_normal,
		a_binorm,
		a_tgt;
	// light data
	GLint
		u_ambientalColor,
		// light structure
		u_diffuseColor,
		u_specularColor,
		u_shinesss,
		u_position,
		u_direction,
		u_type,
		// per object data
		u_specularCoeficient,
		u_diffuseCoeficient,
		u_ratio,
		u_terrainTex[4];

		
	struct Light
	{
		GLint Type,// 1,0,0,0 for directional 0,1,0,0 for point, 0,0,1,0 for spot light, 0,0,0,1
              position, // where the light is  located if spot or point
			  direction,// direction of the light
			  specularColor,
			  diffuseColor,
			  shiness;
	};
	Light u_lights[10];
	GLint Init(GLchar * fileVertexShader, GLchar * fileFragmentShader);
	~Shaders();
};