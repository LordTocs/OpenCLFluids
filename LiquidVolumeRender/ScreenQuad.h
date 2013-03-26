#pragma once
#include <Tocs/Graphics/Texture.h>
#include <Tocs/Graphics/VBO.h>
#include <Tocs/Graphics/IBO.h>
#include <Tocs/Graphics/VAO.h>
#include <Tocs/Graphics/Shader.h>
class ScreenQuad
{
	Tocs::Graphics::VBO vbo;
	Tocs::Graphics::IBO ibo;
	Tocs::Graphics::VAO vao;
	Tocs::Graphics::Shader QuadShader;
public:
	ScreenQuad();

	void Draw (Tocs::Graphics::Texture2D &texture);
};

