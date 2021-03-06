#pragma once
#include <string>


namespace Tocs {
namespace Graphics {

class TextureFormat
{
	enum InternalFormat
	{
		rgb4,
		rgb5,
		rgb8,
		rgb10,
		rgb12,
		rgb16,
		
		rgba2,
		rgba8,
		rgba12,
		rgba16,

		bgr8,
		argb8, 
		depth32f,
		depth16f,
	};
	TextureFormat (InternalFormat format);

	InternalFormat Internal;
public:
	const static TextureFormat RGB4;
	const static TextureFormat RGB5;
	const static TextureFormat RGB8;
	const static TextureFormat RGB10;
	const static TextureFormat RGB12;
	const static TextureFormat RGB16;
	const static TextureFormat RGBA2;
	const static TextureFormat RGBA8;
	const static TextureFormat RGBA12;
	const static TextureFormat RGBA16;
	const static TextureFormat BGR8;
	const static TextureFormat ARGB8;
	const static TextureFormat Depth32F;
	const static TextureFormat Depth16F;

	bool IsSupported () const;
	std::string ToString () const;

	unsigned int InternalGLFormat () const;
	unsigned int Format () const;
	unsigned int Type () const;

	bool operator ==  (const TextureFormat &op2) const {return Internal == op2.Internal;}
	bool operator !=  (const TextureFormat &op2) const {return Internal != op2.Internal;}
};

class TextureDataFormat
{
	enum InternalFormat
	{
		rgb8,
		rgba8,
		bgr8,
		bgra8,
		depthf,
	};
	InternalFormat Internal;

	TextureDataFormat (InternalFormat format);

public:
	const static TextureDataFormat RGB8;
	const static TextureDataFormat RGBA8;
	const static TextureDataFormat BGR8;
	const static TextureDataFormat BGRA8;
	const static TextureDataFormat DepthFloat;

	unsigned int DataType () const;
	unsigned int Format () const;

	bool operator ==  (const TextureDataFormat &op2) const {return Internal == op2.Internal;}
	bool operator !=  (const TextureDataFormat &op2) const {return Internal != op2.Internal;}

	std::string ToString () const;
};

class TextureFiltering
{
	enum InternalEnum
	{
		none,
		mipmapping,
		anisotropic
	};
	InternalEnum Internal;

	TextureFiltering (InternalEnum format)
		: Internal(format)
	{}

public:
	const static TextureFiltering None;
	const static TextureFiltering Anisotropic;
	const static TextureFiltering MipMapping;

	bool IsSupported () const;

	bool operator ==  (const TextureFiltering &op2) const {return Internal == op2.Internal;}
	bool operator !=  (const TextureFiltering &op2) const {return Internal != op2.Internal;}

	std::string ToString () const;
};

}}

