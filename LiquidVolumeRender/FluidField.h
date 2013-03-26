#pragma once

#include "ScreenQuad.h"
#include <Tocs/Graphics/Texture3D.h>
#include <Tocs/Parallel/Image3D.h>
#include <Tocs/Graphics/Color.h>
#include <Tocs/Parallel/Program.h>
#include <Tocs/Parallel/Context.h>
#include <Tocs/Parallel/Kernel.h>
#include <Tocs/Math/Vector3.h>
#include "Int3.h"
#include "VolumeCube.h"

class FluidVolume
{
	float DiffusionRate;
	float dx;
	float InkDissipationRate;
	unsigned int Width, Height, Depth;
	unsigned int JacobiIterations;
	Tocs::Graphics::Texture3D GDensity, GDensityPrev;
	Tocs::Parallel::Image3D   Density,  DensityPrev;
	Tocs::Parallel::Image3D	  Velocity, VelocityPrev;
	Tocs::Parallel::Image3D   Pressure, PressurePrev, DivergenceField;
	VolumeCube cube;


	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D,Tocs::Parallel::Image3D,float,Tocs::Parallel::Image3D,float)> Advect;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D,Tocs::Parallel::Image3D,float)> Divergence;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D,Tocs::Parallel::Image3D,Tocs::Parallel::Image3D,float)> Gradient;

	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D, Tocs::Parallel::Image3D, Tocs::Parallel::Image3D, float, float)> Jacobi;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D, Int3, Tocs::Graphics::Color)> InkSet;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D, Int3, Tocs::Math::Vector3)> VelSet;
	static Tocs::Parallel::Kernel<void(Tocs::Parallel::Image3D)> Reset;

	void SwapVelocity ();
	void SwapDensity ();
	void SwapPressure ();

	void VelStep (float dt, Tocs::Parallel::CommandQueue &queue);
	void DensStep (float dt, Tocs::Parallel::CommandQueue &queue);
	void Project (Tocs::Parallel::CommandQueue &queue);
public:
	FluidVolume(unsigned int w, unsigned int h, unsigned int d, Tocs::Parallel::Context &context, Tocs::Parallel::CommandQueue &queue);

	void Update (float dt, Tocs::Parallel::CommandQueue &queue);

	void Draw (Camera &cam);

	void SetVel (unsigned int x, unsigned int y, unsigned int z, Tocs::Math::Vector3 vel, float rad, Tocs::Parallel::CommandQueue &queue);
	void SetInk (unsigned int x, unsigned int y, unsigned int z, Tocs::Graphics::Color color, float rad, Tocs::Parallel::CommandQueue &queue);

	static void LoadStatics (Tocs::Parallel::Context &context);
};