#pragma once

#include "TextureFormat.h"

namespace Tocs {
namespace Graphics {



class Texture2D
{
	unsigned int ID;
	Texture2D (const Texture2D &); //No Copying textures.

	TextureFormat _Format;
	TextureFiltering _Filtering;
	int _Width;
	int _Height;

	void BuildTexture (int width, int height, const TextureFiltering &filtering, const TextureFormat &format);

	Texture2D &operator= (const Texture2D &);
public:
	Texture2D(int width, int height, const TextureFiltering &filtering, const TextureFormat &format);
	Texture2D(Texture2D &&moveme);
	~Texture2D();

	unsigned int GetID () const { return ID; }

	const TextureFormat &Format () const {return _Format;}
	const TextureFiltering &Filtering () const {return _Filtering;}

	int Width () const { return _Width; };
	int Height () const { return _Height; };

	bool HasMipMaps () const;

	void Bind () const;
	void Bind (int Register) const;
	void UnBind () const;

	void SetData (TextureDataFormat format, void *data);

	Texture2D &operator= (Texture2D &&moveme);
};

}}