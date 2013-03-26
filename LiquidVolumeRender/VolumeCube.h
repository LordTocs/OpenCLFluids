#pragma once
#include <Tocs/Graphics/Texture3D.h>
#include <Tocs/Graphics/VBO.h>
#include <Tocs/Graphics/IBO.h>
#include <Tocs/Graphics/VAO.h>
#include <Tocs/Graphics/Shader.h>
#include "Camera.h"



class VolumeCube
{
	Tocs::Graphics::VBO vbo;
	Tocs::Graphics::IBO ibo;
	Tocs::Graphics::VAO vao;
	Tocs::Graphics::Shader VolumeShader;
	float Size;
public:
	VolumeCube (float size);

	void Draw (Camera &cam, Tocs::Graphics::Texture3D &volume);

};

