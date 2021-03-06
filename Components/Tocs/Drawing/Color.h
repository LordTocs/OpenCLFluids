#pragma once

namespace Tocs {
namespace Drawing {

class Color
{
public:
	unsigned char R,G,B,A;

	Color()
		: R(0), G(0), B(0), A(0) {}
	Color(unsigned char r, unsigned char g, unsigned char b)
		: R(r), G(g), B(b), A(255) {}
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		: R(r), G(g), B(b), A(a) {}
};

}}