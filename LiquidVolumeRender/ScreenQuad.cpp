#include "ScreenQuad.h"
#include <iostream>
#include <Tocs/Graphics/VertexFormat.h>
#include <Tocs/Graphics/GraphicsContext.h>

using namespace std;
using namespace Tocs;
using namespace Tocs::Graphics;
struct CrappyVertex
{
	float x,y,z;
	float u,v;
};

ScreenQuad::ScreenQuad(void)
	: vbo (4 * sizeof(CrappyVertex)),
	  ibo (6)
{
	CrappyVertex vertices [4] = {{1,1,0.5,1,0},{-1,1,0.5,0,0},{1,-1,0.5,1,1},{-1,-1,0.5,0,1}};
	vbo.Write (vertices,4,0);

	unsigned short indices[6] = {2,1,0,  2,3,1};
	ibo.Write (indices,6,0);


	VertexFormat format;
	format.AddMember ("Position",VertexType::Vec3,false);
	format.AddMember ("TexCoord",VertexType::Vec2,false);

	vao.Bind ();
	vao.AddVBO (vbo,format);
	vao.UnBind ();

	ShaderCode vertcode (ShaderType::Vertex);
	vertcode.CompileFromFile ("Quad.vert");
	if (!vertcode.Compiled ())
	{
		cout << vertcode.GetCompileErrors () << endl;
		return;
	}

	ShaderCode fragcode (ShaderType::Pixel);
	fragcode.CompileFromFile ("Quad.frag");
	if (!fragcode.Compiled ())
	{
		cout << fragcode.GetCompileErrors () << endl;
		return;
	}

	
	QuadShader.SetOutput    ("Color",0);

	QuadShader.AddCode (vertcode);
	QuadShader.AddCode (fragcode);

	
	QuadShader.SetAttribute ("InPosition",0);
	QuadShader.SetAttribute ("InTextureCoordinate",1);
	

	QuadShader.Link ();

	if (!QuadShader.Linked ())
	{
		cout << QuadShader.GetLinkErrors () << endl;
	}
	cout << "InPosition: " << QuadShader.GetAttribute ("InPosition") << endl
		 << "InTextureCoordinate: " << QuadShader.GetAttribute ("InTextureCoordinate") << endl
		 << "Color: " << QuadShader.GetOutput ("Color") << endl;
}


void ScreenQuad::Draw (Tocs::Graphics::Texture2D &texture)
{
	
	QuadShader.Bind ();
	QuadShader["Texture"] = texture; //Segfaults without "Texture" uniform being used.
	vao.Bind ();
	ibo.Bind ();
	GraphicsContext::DrawTriangles (2);
	ibo.UnBind ();
	vao.UnBind ();
	QuadShader.UnBind ();
}