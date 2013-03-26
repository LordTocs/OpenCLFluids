#include "FluidField2D.h"
#include <iostream>
#include <Tocs/Graphics/Color.h>
using namespace std;
using namespace Tocs;
using namespace Tocs::Parallel;
using namespace Tocs::Graphics;

Kernel<void(Image2D,Image2D,float,Image2D,float)> FluidField2D::Advect ((KernelBase ()));
Kernel<void(Image2D,Image2D,float)>         FluidField2D::Divergence ((KernelBase ()));
Kernel<void(Image2D,Image2D,Image2D,float)> FluidField2D::Gradient ((KernelBase ()));
Kernel<void(Image2D)>                       FluidField2D::Reset ((KernelBase ()));
Kernel<void(Image2D, unsigned int, unsigned int, float, float)> FluidField2D::VelSet ((KernelBase ()));
Kernel<void(Image2D, unsigned int, unsigned int, Color)> FluidField2D::InkSet ((KernelBase ()));
Kernel<void(Image2D, Image2D, Image2D, float, float)> FluidField2D::Jacobi ((KernelBase ()));


void FluidField2D::LoadStatics (Context &context)
{
	Program fluid2d (context);
	auto result = fluid2d.CompileFromFile ("Liquids2D.cl");
	if (!result.WasSuccessful ())
	{
		cout << result.GetErrors () << endl;
		return;
	}

	Advect.SetBase (fluid2d.GetKernel ("Advect2D"));
	Divergence.SetBase (fluid2d.GetKernel ("Divergence2D"));
	Gradient.SetBase (fluid2d.GetKernel ("Gradient2D"));
	Reset.SetBase(fluid2d.GetKernel("Reset"));
	VelSet.SetBase(fluid2d.GetKernel("VelSet"));
	Jacobi.SetBase(fluid2d.GetKernel("Jacobi2D"));
	InkSet.SetBase(fluid2d.GetKernel("InkSet"));
}

FluidField2D::FluidField2D(unsigned int w, unsigned int h, Tocs::Parallel::Context &context, Tocs::Parallel::CommandQueue &queue)
	: Width(w),
	  Height(h),
	  GDensity (w,h,TextureFiltering::None,TextureFormat::RGBA8),
	  GDensityPrev (w,h,TextureFiltering::None, TextureFormat::RGBA8),
	  Density(GDensity,BufferAccess::ReadWrite,context),
	  DensityPrev(GDensityPrev,BufferAccess::ReadWrite,context),
	  Velocity(w,h,ImageFormat::RGFLOAT, BufferAccess::ReadWrite, context),
	  VelocityPrev(w,h,ImageFormat::RGFLOAT, BufferAccess::ReadWrite, context),
	  Pressure(w,h,ImageFormat::AFLOAT, BufferAccess::ReadWrite, context),
	  PressurePrev(w,h,ImageFormat::AFLOAT, BufferAccess::ReadWrite, context),
	  DivergenceField (w,h,ImageFormat::AFLOAT, BufferAccess::ReadWrite, context),
	  DiffusionRate(2),
	  JacobiIterations(30),
	  dx(1),
	  InkDissipationRate(1)
{
	std::unique_ptr<Color[]> colordata (new Color [w*h] ());

	GDensity.SetData (TextureDataFormat::RGBA8,colordata.get ());
	GDensityPrev.SetData (TextureDataFormat::RGBA8,colordata.get());

	std::vector<float> vels;
	vels.resize (w*h*2,0);

	queue(Velocity.Write (vels).Then (
		  VelocityPrev.Write (vels)).Then (
		  Reset(Pressure).Dim2(w,h)).Then (
		  Reset(PressurePrev).Dim2(w,h)).Then (
		  Reset(DivergenceField).Dim2(w,h))).Finish ();
}


void FluidField2D::SwapDensity ()
{
	Texture2D temp = std::move (GDensity);
	GDensity = std::move(GDensityPrev);
	GDensityPrev = std::move(temp);

	Image2D temp2 = std::move (Density);
	Density = std::move(DensityPrev);
	DensityPrev = std::move(temp2);
}

void FluidField2D::SwapVelocity ()
{
	Image2D temp2 = std::move (Velocity);
	Velocity = std::move(VelocityPrev);
	VelocityPrev = std::move(temp2);
}

void FluidField2D::SwapPressure ()
{
	Image2D temp2 = std::move (Pressure);
	Pressure = std::move(PressurePrev);
	PressurePrev = std::move(temp2);
}

void FluidField2D::Update (float dt, CommandQueue &queue)
{
	VelStep (dt,queue);
	DensStep (dt,queue); //Really should be called InkStep
}

void FluidField2D::SetVel (unsigned int x, unsigned int y, float vx, float vy,float rad, CommandQueue &queue)
{
	queue(VelSet(Velocity,x,y,vx,vy).Dim2(2*rad,2*rad)).Finish ();
}


void FluidField2D::SetInk (unsigned int x, unsigned int y, Color color, float rad, CommandQueue &queue)
{
	queue(Density.AcquireGL ().Then (
		  InkSet(Density,x,y,color).Dim2(2*rad,2*rad)).Then (
		  Density.ReleaseGL ())).Finish ();
}
void FluidField2D::Project (Tocs::Parallel::CommandQueue &queue)
{
	//Zero out pressure texutures
	queue(Reset(Pressure).Dim2(Width,Height).Then(Reset(PressurePrev).Dim2(Width,Height))).Finish ();
	//Calculate the divergence of the velocity
	queue(Divergence (Velocity,DivergenceField,0.5f/dx).Dim2(Width,Height)).Finish ();
	//Use iterations to approximate a solution
	for (int k = 0; k < JacobiIterations; ++k)
	{
		SwapPressure ();
		//Call the jacobi
		queue(Jacobi(Pressure,PressurePrev,DivergenceField,-(dx * dx),4).Dim2(Width,Height)).Finish ();
	}
	SwapVelocity ();
	//Subtract the gradient vector field to correct the velocities.
	queue(Gradient(Pressure,VelocityPrev,Velocity,0.5f/dx).Dim2(Width,Height)).Finish();

}

void FluidField2D::VelStep (float dt, Tocs::Parallel::CommandQueue &queue)
{
	float a = dx * dx / (dt * DiffusionRate);
	for (int i = 0; i < JacobiIterations; ++i) //Iterates to approx. the solution
	{
		SwapVelocity ();//Swaps Velocity and VelocityPrev
		//     Kernel template class being put to use..
		queue (Jacobi(Velocity,VelocityPrev,VelocityPrev,a,(4 + a)).Dim2(Width,Height)).Finish ();
		//Finish () blocks the CPU thread until the queue has executed all commands.
	}
	SwapVelocity ();
	queue(Advect(Velocity,VelocityPrev,dt,VelocityPrev,1).Dim2(Width,Height)).Finish ();
	Project (queue);//Does the mass conserving corrections to the velocity field.
}

void FluidField2D::DensStep (float dt, Tocs::Parallel::CommandQueue &queue)
{
	queue(Density.AcquireGL ().Then (DensityPrev.AcquireGL())).Finish ();
	float a = dx * dx / (dt * DiffusionRate);
	for (int i = 0; i < JacobiIterations; ++i)
	{
		SwapDensity ();
		queue (Jacobi(Density,DensityPrev,DensityPrev,a,(4 + a)).Dim2(Width,Height)).Finish ();
	}

	SwapDensity ();
	queue(Advect(Density,DensityPrev,dt,Velocity,InkDissipationRate).Dim2(Width,Height)).Finish ();
	queue(Density.ReleaseGL ().Then (DensityPrev.ReleaseGL())).Finish ();

}

void FluidField2D::Draw ()
{
	Quad.Draw (GDensity);
}