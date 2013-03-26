#pragma once
#include "ScreenQuad.h"
#include <Tocs/Graphics/Texture.h>
#include <Tocs/Parallel/Image2D.h>
#include <Tocs/Graphics/Color.h>
#include <Tocs/Parallel/Program.h>
#include <Tocs/Parallel/Context.h>
#include <Tocs/Parallel/Kernel.h>

class FluidField2D
{
	float DiffusionRate;
	float dx;
	float InkDissipationRate;
	unsigned int Width, Height;
	unsigned int JacobiIterations;
	Tocs::Graphics::Texture2D GDensity, GDensityPrev;
	Tocs::Parallel::Image2D   Density,  DensityPrev;
	Tocs::Parallel::Image2D	  Velocity, VelocityPrev;
	Tocs::Parallel::Image2D   Pressure, PressurePrev, DivergenceField;
	ScreenQuad Quad;

	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D,Tocs::Parallel::Image2D,float,Tocs::Parallel::Image2D,float)> Advect;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D,Tocs::Parallel::Image2D,float)> Divergence;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D,Tocs::Parallel::Image2D,Tocs::Parallel::Image2D,float)> Gradient;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D)> Reset;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D, unsigned int, unsigned int, float, float)> VelSet;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D, Tocs::Parallel::Image2D, Tocs::Parallel::Image2D, float, float)> Jacobi;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image2D, unsigned int, unsigned int, Tocs::Graphics::Color)> InkSet;

	void SwapVelocity ();
	void SwapDensity ();
	void SwapPressure ();

	void VelStep (float dt, Tocs::Parallel::CommandQueue &queue);
	void DensStep (float dt, Tocs::Parallel::CommandQueue &queue);
	void Project (Tocs::Parallel::CommandQueue &queue);
public:
	FluidField2D(unsigned int w, unsigned int h, Tocs::Parallel::Context &context, Tocs::Parallel::CommandQueue &queue);

	void Update (float dt, Tocs::Parallel::CommandQueue &queue);

	void Draw ();

	void SetVel (unsigned int x, unsigned int y, float vx, float vy, float rad, Tocs::Parallel::CommandQueue &queue);
	void SetInk (unsigned int x, unsigned int y, Tocs::Graphics::Color color, float rad, Tocs::Parallel::CommandQueue &queue);

	static void LoadStatics (Tocs::Parallel::Context &context);
};

