#include <iostream>
#include <memory>
#include <vector>
#include <Tocs/Parallel/Device.h>
#include <Tocs/Parallel/CommandQueue.h>
#include <Tocs/Parallel/Buffer.h>
#include <Tocs/Parallel/Program.h>
#include <Tocs/Parallel/Kernel.h>
#include <Tocs/Parallel/Image2D.h>
#include <Tocs/Drawing/Image.h>
#include <Tocs/Drawing/AnimatedGif.h>
#include "Timer.h"

using namespace Tocs::Parallel;
using namespace Tocs::Drawing;
using namespace std;

/*
class Matrix
{
public:
	unique_ptr<float[]> HostData;
	Buffer<float> DeviceData;
	unsigned int Size;

	
	

	Matrix (const Matrix &);

	static Kernel<void(float*,float*,float*)> MultKernel;
	static KernelBase LoadKernel (Context &context)
	{
		Program program (context);
		auto results = program.CompileFromFile ("MatrixMult.cl");
		if (!results.WasSuccessful ())
		{
			cout << results.GetErrors () << endl;
			int pause;
			cin >> pause;
			exit (0);
		}
		return program.GetKernel("MatrixMult");
	}

	Matrix(Context &context, unsigned int size)
		: Size(size),
		  DeviceData(context,size*size),
		  HostData(new float [size * size] ())
	{
	}

	Matrix(Matrix &&moveme)
		: HostData(std::move(moveme.HostData)),
		  DeviceData(std::move(moveme.DeviceData)),
		  Size(moveme.Size)
	{
	}

	Matrix Mult (Matrix &op2, CommandQueue &queue)
	{
		if (Size != op2.Size)
			throw "WTF ARE YOU DOING";

		Matrix result (queue.GetContext (), Size);
		
		queue(DeviceData.Write(0,Size*Size,HostData.get()).Then
			 (op2.DeviceData.Write(0,Size*Size,op2.HostData.get())).Then
			 (MultKernel(DeviceData,op2.DeviceData,result.DeviceData).Dim2(Size,Size)).Then
		     (result.DeviceData.Read(0,Size*Size,result.HostData.get()))).Finish();
				
		return std::move(result);
	}

	void Mult (Matrix &op2, CommandQueue &queue, Matrix &result)
	{
		if (Size != op2.Size || Size != result.Size)
			throw "WTF ARE YOU DOING";

		queue(DeviceData.Write(0,Size*Size,HostData.get()).Then
			 (op2.DeviceData.Write(0,Size*Size,op2.HostData.get())).Then
			 (MultKernel(DeviceData,op2.DeviceData,result.DeviceData).Dim2(Size,Size)).Then
		     (result.DeviceData.Read(0,Size*Size,result.HostData.get()))).Finish();
	}

	void Write (CommandQueue &queue)
	{
		queue(DeviceData.Write(0,Size*Size,HostData.get ())).Finish ();
	}

	void Read (CommandQueue &queue)
	{
		queue(DeviceData.Read(0,Size*Size,HostData.get ())).Finish();
	}

	void Print () const
	{
		for (int r = 0; r < Size; ++r)
		{
			for (int c = 0; c < Size; ++c)
			{
				cout << HostData[c + r * Size] << " ";
			}
			cout << endl;
		}
	}

	float &operator () (unsigned int r, unsigned int c)
	{
		return HostData[c + r * Size];
	}

	static Matrix Random (Context &context, unsigned int size)
	{
		Matrix result (context, size);

		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				result(i,j) = float(rand ()) / RAND_MAX;
			}
		}

		return std::move(result);
	}

	static Matrix Identity (Context &context, unsigned int size)
	{
		Matrix result (context, size);

		for (int i = 0; i < size; ++i)
		{
			result(i,i) = 1;
		}

		return std::move(result);
	}
};

Device device;
Context context (device);
CommandQueue queue(context);

Kernel<void(float*,float*,float*)> Matrix::MultKernel (Matrix::LoadKernel (context));


int main ()
{
	int pause;
	cout << "OpenCL Playground." << endl;
	
	device.PrintInfo ();
	
	int size = 2000;
	int count = 1;
	Matrix a = Matrix::Random (context,size);
	Matrix b = Matrix::Identity (context,size);
	a.Write (queue);
	b.Write (queue);
	
	Matrix res (context,size);

	Timer t;

	t.Reset ();
	for (int i = 0; i < count; ++i)
	{
		//a.Mult(b,queue,res);
		queue(Matrix::MultKernel(a.DeviceData,b.DeviceData,res.DeviceData).Dim2(a.Size,a.Size));
	}
	queue.Finish ();
	float time = t.GetTime ();
	unsigned int flops =  count * (2 * size * size * size);

	cout << "Time: " << time << endl;
	cout << "Flop count: " << flops << endl;
	cout << "Gigaflops: " << flops / 1000000000.0f / time << endl;

	//res.Print ();

	cin >> pause;
	return 0;
}
*/