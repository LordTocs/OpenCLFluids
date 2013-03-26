#include "Texture.h"
#include "GLHeader.h"
#include <iostream>

using namespace std;

namespace Tocs {
namespace Graphics {

Texture2D::Texture2D(int width, int height, const TextureFiltering &filtering, const TextureFormat &format)
	: ID (0),
	  _Format (TextureFormat::RGB8),
	  _Filtering (TextureFiltering::MipMapping),
	  _Width(width),
	  _Height(height)
{
	glGenTextures (1, &ID);
	BuildTexture (width,height,filtering,format);
}

Texture2D::Texture2D(Texture2D &&moveme)
	: ID(moveme.ID),
	  _Format (moveme._Format),
	  _Filtering (moveme._Filtering),
	  _Width(moveme._Width),
	  _Height(moveme._Height)
{
	moveme.ID = 0;
	moveme._Width = 0;
	moveme._Height = 0;
}

Texture2D::~Texture2D(void)
{
	glDeleteTextures (1,&ID);
}

Texture2D &Texture2D::operator= (Texture2D &&moveme)
{
	ID = moveme.ID;
	moveme.ID = 0;
	_Width = moveme._Width;
	moveme._Width = 0;
	_Height = moveme._Height;
	moveme._Height = 0;
	return *this;
}

void Texture2D::BuildTexture (int width, int height, const TextureFiltering &filtering, const TextureFormat &format)
{
	Bind ();

	_Filtering = filtering;
	_Format = format;

	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLErrorCheck ();
	
	if (filtering != TextureFiltering::None)
	{
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		GLErrorCheck ();
	}
	else
	{
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GLErrorCheck ();
	}

	if (filtering == TextureFiltering::Anisotropic)
	{
		glTexParameterf (GL_TEXTURE_2D, GL_EXT_texture_filter_anisotropic,2.0f); //Get from context?
		GLErrorCheck ();
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GLErrorCheck ();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLErrorCheck ();

	glTexImage2D (GL_TEXTURE_2D,0,format.InternalGLFormat (),width,height,0,format.Format (),format.Type (),nullptr);
	GLErrorCheck ();

	UnBind ();
}

void Texture2D::SetData (TextureDataFormat format, void *data)
{
	Bind ();
	glTexSubImage2D (GL_TEXTURE_2D,0,0,0,Width (), Height (),format.Format (),format.DataType (),data);
	GLErrorCheck ();

	cout << "SetData: " << format.Format () << ", " << format.DataType () << endl;

	if (Filtering () != TextureFiltering::None)
	{
		glGenerateMipmap (GL_TEXTURE_2D);
		GLErrorCheck ();
	}
	UnBind ();
}

void Texture2D::Bind () const
{
	glBindTexture (GL_TEXTURE_2D,ID);
	GLErrorCheck ();
}

void Texture2D::Bind (int reg) const
{
	glActiveTexture (GL_TEXTURE0 + reg);
	GLErrorCheck ();
	glBindTexture (GL_TEXTURE_2D,ID);
	GLErrorCheck ();
}

void Texture2D::UnBind () const
{
	glBindTexture (GL_TEXTURE_2D,0);
}
}}