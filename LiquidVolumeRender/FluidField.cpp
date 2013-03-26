#include "FluidField.h"

#include <iostream>
#include <Tocs/Graphics/Color.h>
using namespace std;
using namespace Tocs;
using namespace Tocs::Parallel;
using namespace Tocs::Math;
using namespace Tocs::Graphics;

Kernel<void(Image3D,Image3D,float,Image3D,float)>     FluidVolume::Advect ((KernelBase ()));
Kernel<void(Image3D,Image3D,float)>                   FluidVolume::Divergence ((KernelBase ()));
Kernel<void(Image3D,Image3D,Image3D,float)>           FluidVolume::Gradient ((KernelBase ()));
Kernel<void(Image3D)>                                 FluidVolume::Reset ((KernelBase ()));
Kernel<void(Image3D, Int3, Vector3)>                  FluidVolume::VelSet ((KernelBase ()));
Kernel<void(Image3D, Int3, Color)>                    FluidVolume::InkSet ((KernelBase ()));
Kernel<void(Image3D, Image3D, Image3D, float, float)> FluidVolume::Jacobi ((KernelBase ()));


void FluidVolume::LoadStatics (Context &context)
{
	Program fluid2d (context);
	auto result = fluid2d.CompileFromFile ("Liquids.cl");
	if (!result.WasSuccessful ())
	{
		cout << result.GetErrors () << endl;
		return;
	}

	Advect.SetBase (fluid2d.GetKernel ("Advect3D"));
	Divergence.SetBase (fluid2d.GetKernel ("Divergence3D"));
	Gradient.SetBase (fluid2d.GetKernel ("Gradient3D"));
	Reset.SetBase(fluid2d.GetKernel("Reset"));
	VelSet.SetBase(fluid2d.GetKernel("VelSet"));
	Jacobi.SetBase(fluid2d.GetKernel("Jacobi3D"));
	InkSet.SetBase(fluid2d.GetKernel("InkSet"));
}

FluidVolume::FluidVolume(unsigned int w, unsigned int h, unsigned int d, Tocs::Parallel::Context &context, Tocs::Parallel::CommandQueue &queue)
	: Width(w),
	  Height(h),
	  Depth(d),
	  GDensity (w,h,d,TextureFiltering::None,TextureFormat::RGBA8),
	  GDensityPrev (w,h,d,TextureFiltering::None, TextureFormat::RGBA8),
	  Density(GDensity,BufferAccess::ReadWrite,context),
	  DensityPrev(GDensityPrev,BufferAccess::ReadWrite,context),
	  Velocity(w,h,d,ImageFormat::RGFLOAT, BufferAccess::ReadWrite, context),
	  VelocityPrev(w,h,d,ImageFormat::RGFLOAT, BufferAccess::ReadWrite, context),
	  Pressure(w,h,d,ImageFormat::AFLOAT, BufferAccess::ReadWrite, context),
	  PressurePrev(w,h,d,ImageFormat::AFLOAT, BufferAccess::ReadWrite, context),
	  DivergenceField (w,h,d,ImageFormat::AFLOAT, BufferAccess::ReadWrite, context),
	  DiffusionRate(1),
	  JacobiIterations(20),
	  dx(1),
	  InkDissipationRate(0.99),
	  cube(w)
{

	queue(Reset(Velocity).Dim3(w,h,d)).Finish ();
	queue(Reset(VelocityPrev).Dim3(w,h,d)).Finish ();
	queue(Density.AcquireGL ()).Finish ();
	queue(DensityPrev.AcquireGL ()).Finish ();
	queue(Reset(Density).Dim3(w,h,d)).Finish ();
	queue(Reset(DensityPrev).Dim3(w,h,d)).Finish ();
	queue(Reset(Pressure).Dim3(w,h,d)).Finish ();
	queue(Reset(PressurePrev).Dim3(w,h,d)).Finish ();
	queue(Reset(DivergenceField).Dim3(w,h,d)).Finish ();
	queue(Density.ReleaseGL ()).Finish ();
	queue(DensityPrev.AcquireGL()).Finish ();
}


void FluidVolume::SwapDensity ()
{
	Texture3D temp = std::move (GDensity);
	GDensity = std::move(GDensityPrev);
	GDensityPrev = std::move(temp);

	Image3D temp2 = std::move (Density);
	Density = std::move(DensityPrev);
	DensityPrev = std::move(temp2);
}

void FluidVolume::SwapVelocity ()
{
	Image3D temp2 = std::move (Velocity);
	Velocity = std::move(VelocityPrev);
	VelocityPrev = std::move(temp2);
}

void FluidVolume::SwapPressure ()
{
	Image3D temp2 = std::move (Pressure);
	Pressure = std::move(PressurePrev);
	PressurePrev = std::move(temp2);
}

void FluidVolume::Update (float dt, CommandQueue &queue)
{
	//VelStep
	//cout << dt << endl;
	VelStep (dt,queue);
	DensStep (dt,queue);

}

void FluidVolume::SetVel (unsigned int x, unsigned int y, unsigned int z, Vector3 vel, float rad, CommandQueue &queue)
{
	queue(VelSet(Velocity,Int3(x,y,z),vel).Dim3(2*rad,2*rad,2*rad)).Finish ();
}


void FluidVolume::SetInk (unsigned int x, unsigned int y, unsigned int z, Color color, float rad, CommandQueue &queue)
{
	queue(Density.AcquireGL ().Then (
		  InkSet(Density,Int3(x,y,z),color).Dim3(2*rad,2*rad,2*rad)).Then (
		  Density.ReleaseGL ())).Finish ();
}
void FluidVolume::Project (Tocs::Parallel::CommandQueue &queue)
{
	queue(Reset(Pressure).Dim3(Width,Height,Depth).Then(Reset(PressurePrev).Dim3(Width,Height,Depth))).Finish ();
	queue(Divergence (Velocity,DivergenceField,0.5f/dx).Dim3(Width,Height,Depth)).Finish ();
	for (int k = 0; k < JacobiIterations; ++k)
	{
		SwapPressure ();
		queue(Jacobi(Pressure,PressurePrev,DivergenceField,-(dx * dx),4).Dim3(Width,Height,Depth)).Finish ();
	}
	SwapVelocity ();
	queue(Gradient(Pressure,VelocityPrev,Velocity,0.5f/dx).Dim3(Width,Height,Depth)).Finish();

}

void FluidVolume::VelStep (float dt, Tocs::Parallel::CommandQueue &queue)
{
	float a = dx * dx / (dt * DiffusionRate);
	for (int i = 0; i < JacobiIterations; ++i)
	{
		SwapVelocity ();
		queue (Jacobi(Velocity,VelocityPrev,VelocityPrev,a,(4 + a)).Dim3(Width,Height,Depth)).Finish ();
	}
	Project(queue);
	SwapVelocity ();
	queue(Advect(Velocity,VelocityPrev,dt,VelocityPrev,1).Dim3(Width,Height,Depth)).Finish ();
	Project (queue);
}

void FluidVolume::DensStep (float dt, Tocs::Parallel::CommandQueue &queue)
{
	queue(Density.AcquireGL ().Then (DensityPrev.AcquireGL())).Finish ();
	float a = dx * dx / (dt * DiffusionRate);
	for (int i = 0; i < JacobiIterations; ++i)
	{
		SwapDensity ();
		queue (Jacobi(Density,DensityPrev,DensityPrev,a,(4 + a)).Dim3(Width,Height,Depth)).Finish ();
	}

	SwapDensity ();
	queue(Advect(Density,DensityPrev,dt,Velocity,InkDissipationRate).Dim3(Width,Height,Depth)).Finish ();
	queue(Density.ReleaseGL ().Then (DensityPrev.ReleaseGL())).Finish ();

}

void FluidVolume::Draw (Camera &cam)
{
	cube.Draw (cam,GDensity);
}


