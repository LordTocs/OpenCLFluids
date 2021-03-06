#pragma once

#include "Quaternion.h"
namespace Tocs {
namespace Math {

class Vector3;

class Matrix4
{
	float Data [4][4];
	Matrix4(float diagonal);
public:
	static const Matrix4 Identity;

	Matrix4();

	void Invert ();
	void Translate (const Vector3 &vector);

	static Matrix4 CreateTranslation (const Vector3 &vector);
	static Matrix4 CreateRotation (const Quaternion &rotation);

	static Matrix4 CreateLookAt (const Vector3 &Eyepoint, const Vector3 &LookPoint);
	static Matrix4 CreateLookAt (const Vector3 &Eyepoint, const Vector3 &LookPoint, const Vector3 &Up);

	static Matrix4 CreateFrustum (float left, float right,float bottom, float top, float nearval, float farval);
	static Matrix4 CreateProjection (float FoV, float AspectRatio, float Near, float Far);
	
	static Matrix4 CreateRotateX (float xrot);
	static Matrix4 CreateRotateY (float yrot);
	static Matrix4 CreateRotateZ (float zrot);

	static Matrix4 Inversion (const Matrix4 matrix);


	float *GetData () const { return (float *)&(Data[0][0]); }
};

}}