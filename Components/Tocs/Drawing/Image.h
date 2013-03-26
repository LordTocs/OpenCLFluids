#pragma once
#include <memory>
#include "Color.h"
#include <string>

namespace Tocs {
namespace Drawing {

class Image
{
	std::unique_ptr<Color []> ColorArray;
	unsigned int Width;
	unsigned int Height;
public:
	Image(unsigned int width, unsigned int height);
	Image(Image &&moveme);
	explicit Image(const Image &copyme);

	Color &operator () (unsigned int x, unsigned int y) { return ColorArray.get()[x + y * Width]; }
	const Color &operator () (unsigned int x, unsigned int y) const { return ColorArray.get()[x + y * Width]; }

	void WriteToFile (const std::string &filename) const;

	unsigned int GetWidth () const { return Width; }
	unsigned int GetHeight () const { return Height; }

	Color *GetDataPtr () { return ColorArray.get (); }

	static Image FromFile (const std::string &filename);
};

}}