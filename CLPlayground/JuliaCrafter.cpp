/*#include <iostream>
#include <Tocs/Parallel/Device.h>
#include <Tocs/Parallel/CommandQueue.h>
#include <Tocs/Parallel/Buffer.h>
#include <Tocs/Parallel/Program.h>
#include <Tocs/Parallel/Kernel.h>
#include <Tocs/Parallel/Image2D.h>
#include <Tocs/Drawing/Image.h>
#include <Tocs/Drawing/AnimatedGif.h>

using namespace Tocs::Parallel;
using namespace Tocs::Drawing;
using namespace std;

class Double2
{
public:
	double x;
	double y;

	Double2(double x, double y)
		: x(x), y(y) {}
};

int main ()
{
	int pause;
	cout << "OpenCL Playground." << endl;
	Device device;
	device.PrintInfo ();
	Context context (device);
	CommandQueue queue(context);

	int size = 750;

	Image hostgradient = Image::FromFile ("Gradient.png");

	Image2D gradient (hostgradient.GetWidth (), hostgradient.GetHeight (),ImageFormat::RGBA8,BufferAccess::Read,context);

	Image2D image (size,size,ImageFormat::RGBA8,BufferAccess::Write,context);
	Image hostimage (size,size);

	AnimatedGif gif (size,size);


	Buffer<double> ResultBuffer (context,1);
	vector<double> HostResults;
	
	Program program (context);

	CompileResults results = program.CompileFromFile ("Julia.cl");
	
	if (!results.WasSuccessful ())
	{
		cout << "Compile Failed" << endl;
		cout << "Errors: " << endl << results.GetErrors () << endl;
		cin >> pause;
		return 0;
	}

	Kernel<void(Image2D,Double2,Image2D)> Julia(program.GetKernel("Julia"));

	Double2 d (-1.235,-0.2321);

	int framecount = 200;
	Double2 framestep (0.01/3,0);


	queue(gradient.Write (hostgradient)).Finish ();
	

	for (int i = 0; i < framecount; ++i)
	{
		queue(
		Julia(image,d,gradient).Dim2(image.Width(),image.Height()).
		Then (image.Read(hostimage))
		 ).Finish ();

		d.x += framestep.x;
		d.y += framestep.y;

		gif.AddFrame(hostimage,0.1);
	}

	gif.WriteToFile("julia_anim.gif");

	cout << endl;
	
	cin >> pause;
	return 0;
}
*/