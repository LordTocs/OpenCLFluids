#include <iostream>
#include <memory>
#include <vector>
#include <math.h>
#include <Tocs/Parallel/Device.h>
#include <Tocs/Parallel/CommandQueue.h>
#include <Tocs/Parallel/Buffer.h>
#include <Tocs/Parallel/Program.h>
#include <Tocs/Parallel/Kernel.h>
#include <Tocs/Parallel/Image2D.h>
#include "Timer.h"
#include <fstream>

using namespace Tocs::Parallel;
using namespace Tocs::Drawing;
using namespace std;


Device device;
Context context (device);
CommandQueue queue(context);

void PrintField (const vector<int> &bits, int max)
{
	for (unsigned long long int i = 0; i < max; ++i)
	{
		unsigned int maj = i / (sizeof(int)*8);
		unsigned int min = i % (sizeof(int)*8);
		if ((bits[maj] & (1 << min)) != 0)
		{
			cout << i << " ";
		}
	}
}

void SeiveOne (unsigned int max)
{
	int pause;

	//const unsigned int max = 1<<25;
	const unsigned int sqrtmax = sqrt(double(max));
	const unsigned int majorsize = sizeof(int)*8;
	double bitsize = ceil (double(max)/sizeof(int));

	Program p (context);
	auto errors = p.CompileFromFile ("Sieve.cl");
	if (!errors.WasSuccessful ())
	{
		cout << errors.GetErrors () << endl;
		cin >> pause;
		exit(0);
	}

	Kernel<void(int *, unsigned int, unsigned int, unsigned int)> Sieve (p.GetKernel ("Sieve"));

	std::vector <int> HostBitField;
	HostBitField.resize (bitsize,~0);
	HostBitField[0] = 0xFFFFFFFC; //set 0 and 1 to false.
	Buffer<int> DeviceBitField (context,bitsize);

	queue (DeviceBitField.Write (HostBitField)).Finish ();


	int iterations = 0;

	Timer t;
	
	float avtime = 0;

	for (int i = 2; i < sqrtmax; ++i)
	{
		double dimsize = ceil (double(max)/i) - 2;
		Timer t2;
		queue(Sieve(DeviceBitField,i,1,max).Dim1 (dimsize)).Finish ();
		avtime += t2.GetTime ();
	}


	float time = t.GetTime ();
	//PrintField (HostBitField,max);

	cout << "Sieve Results: " << endl
		 << "Max: " << max << endl
		 << "Time: " << time << "s" << endl
		 << "Iterations: " << sqrtmax << endl
		 << "KernelTime: " << avtime << endl
		 << "Not KernelTime: " << time - avtime << endl
		 << "AvgKernelTime: " << avtime/sqrtmax << endl;

	queue (DeviceBitField.Read (HostBitField)).Finish ();

	unsigned int count = 0;
	ofstream outfile ("List.txt");
	for (int i = 0; i < max; ++i)
	{
		unsigned int maj = i / majorsize;
		unsigned int min = i % majorsize;
		if ((HostBitField[maj] & (1 << min)) != 0)
		{
			++count;
			//outfile << i << endl;
		}
	}
	outfile.close ();

	cout << "Prime Count: " << count << endl;
}


void SeiveTwo (unsigned int max)
{
	int pause;

	//const unsigned int max = 1<<25;
	const unsigned int sqrtmax = sqrt(double(max));

	vector<char> HostBoolField;
	HostBoolField.resize (max,1);
	Buffer<char> DeviceBoolField (context,max);

	queue (DeviceBoolField.Write (HostBoolField)).Finish ();

	Program p (context);
	auto errors = p.CompileFromFile ("Sieve2.cl");
	if (!errors.WasSuccessful ())
	{
		cout << errors.GetErrors () << endl;
		cin >> pause;
		exit(0);
	}

	Kernel<void(char *, unsigned int, unsigned int)> Sieve (p.GetKernel ("Sieve"));

	Timer t;
	
	float avtime = 0;

	for (int i = 2; i < sqrtmax; ++i)
	{
		double dimsize = ceil (double(max)/i) - 2;
		Timer t2;
		queue(Sieve(DeviceBoolField,i,max).Dim1 (dimsize)).Finish ();
		avtime += t2.GetTime ();
	}


	float time = t.GetTime ();
	//PrintField (HostBitField,max);

	cout << "Sieve Results: " << endl
		 << "Max: " << max << endl
		 << "Time: " << time << "s" << endl
		 << "Iterations: " << sqrtmax << endl
		 << "KernelTime: " << avtime << endl
		 << "Not KernelTime: " << time - avtime << endl
		 << "AvgKernelTime: " << avtime/sqrtmax << endl;

	queue (DeviceBoolField.Read (HostBoolField)).Finish ();

	unsigned int count = 0;
	ofstream outfile ("List.txt");
	for (int i = 0; i < max; ++i)
	{
		if (HostBoolField[i] != 0)
		{
			++count;
			//outfile << i << endl;
		}
	}
	outfile.close ();

	cout << "Prime Count: " << count << endl;

}

int main ()
{
	int pause;
	device.PrintInfo ();
	cout << endl;
	unsigned int max = 1<<25;

	cin >> pause;

	cout << "One: " << endl;
	SeiveOne (max);
	cout << "Two: " << endl;
	SeiveTwo (max);


	//int pause;
	cin >> pause;
	return 0;

}