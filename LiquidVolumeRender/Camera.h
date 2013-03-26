#pragma once
#include <Tocs/Math/Matrix4.h>
#include <Tocs/Math/Vector3.h>
#include <Tocs/Graphics/Shader.h>

class Camera
{
	Tocs::Math::Matrix4 ViewMatrix;
	Tocs::Math::Matrix4 ProjectionMatrix;

	float FoV;
	float AspectRatio;
public:
	Tocs::Math::Vector3 Position;
	Tocs::Math::Vector3 LookAt;
	Tocs::Math::Vector3 Up;
	
	Camera(float aspectratio);

	const Tocs::Math::Matrix4 &GetView () const { return ViewMatrix; }
	const Tocs::Math::Matrix4 &GetProjection () const { return ProjectionMatrix; }

	void Compute ();
	void PassToShader (Tocs::Graphics::Shader &shader);
};

