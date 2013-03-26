#include "Camera.h"

using namespace Tocs::Math;
using namespace Tocs::Graphics;

Camera::Camera (float ar)
	: FoV (3.14159265f/2),
	  Up (0,1,0),
	  AspectRatio (ar)
{
	ProjectionMatrix = Matrix4::CreateProjection (FoV,AspectRatio,0.001f,1000.0f);
}

void Camera::Compute ()
{
	ViewMatrix = Matrix4::CreateLookAt (Position,LookAt,Up);
}

void Camera::PassToShader (Shader &shader)
{
	shader["View"] = ViewMatrix;
	shader["Projection"] = ProjectionMatrix;
}


