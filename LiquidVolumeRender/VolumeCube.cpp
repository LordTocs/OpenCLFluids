#include "VolumeCube.h"
#include <iostream>
#include <Tocs/Graphics/GraphicsContext.h>

using namespace std;

using namespace Tocs;
using namespace Tocs::Graphics;
using namespace Tocs::Math;

struct Crappy3DVertex
{
	float x,y,z;
	float tx,ty,tz;
};


VolumeCube::VolumeCube(float size)
	: Size(size),
		vbo(8 * sizeof(Crappy3DVertex)),
		ibo(6 * 2 * 3)
{
	Crappy3DVertex vertices [8] = {{-1, 1, 1, 0, 1, 1},{-1, 1,-1, 0, 1, 0},{ 1, 1,-1, 1, 1, 0},{ 1, 1, 1, 1, 1, 1},
								   {-1,-1, 1, 0, 0, 1},{-1,-1,-1, 0, 0, 0},{ 1,-1,-1, 1, 0, 0},{ 1,-1, 1, 1, 0, 1}};
	vbo.Write (vertices,8,0);

	unsigned short indices[6*2*3] = {0,1,2, 1,2,3,  0,3,7, 0,7,4,  0,4,5, 0,5,1,  2,1,5, 2,5,6,  3,2,6, 3,6,7,  4,5,6, 6,7,4};
	ibo.Write (indices,6*2*3,0);


	VertexFormat format;
	format.AddMember ("Position",VertexType::Vec3,false);
	format.AddMember ("VolumePos",VertexType::Vec3,false);

	vao.Bind ();
	vao.AddVBO (vbo,format);
	vao.UnBind ();

	ShaderCode vertcode (ShaderType::Vertex);
	vertcode.CompileFromFile ("Volume.vert");
	if (!vertcode.Compiled ())
	{
		cout << vertcode.GetCompileErrors () << endl;
		return;
	}

	ShaderCode fragcode (ShaderType::Pixel);
	fragcode.CompileFromFile ("Volume.frag");
	if (!fragcode.Compiled ())
	{
		cout << fragcode.GetCompileErrors () << endl;
		return;
	}

	
	VolumeShader.SetOutput    ("Color",0);

	VolumeShader.AddCode (vertcode);
	VolumeShader.AddCode (fragcode);

	
	VolumeShader.SetAttribute ("InPosition",0);
	VolumeShader.SetAttribute ("InSurfacePos",1);
	

	VolumeShader.Link ();

	if (!VolumeShader.Linked ())
	{
		cout << VolumeShader.GetLinkErrors () << endl;
	}
}
		  
void VolumeCube::Draw (Camera &cam, Texture3D &volume)
{
	VolumeShader.Bind ();
	cam.PassToShader (VolumeShader);
	VolumeShader["World"] = Matrix4::Identity;

	VolumeShader["CameraPos"] = cam.Position;
	VolumeShader["invScale"] = (1/Size);
	VolumeShader["SampleDist"] = 2.0f;
	VolumeShader["VolumeTexture"] = volume;

	vao.Bind ();
	ibo.Bind ();
	GraphicsContext::DrawTriangles(12);
	ibo.UnBind ();
	vao.UnBind ();
	
	VolumeShader.UnBind ();

}