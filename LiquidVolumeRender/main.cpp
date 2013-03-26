#include <iostream>
#include <Tocs/Core/Ticker.h>
#include <Tocs/Graphics/SimpleWindow.h>
#include <Tocs/Graphics/GraphicsContext.h>
#include <Tocs/Parallel/Device.h>
#include <Tocs/Parallel/Context.h>
#include <Tocs/Input/InputManager.h>
#include <Tocs/Core/Numerics.h>
#include "ScreenQuad.h"
#include "FluidField2D.h"
#include "FluidField.h"
#include "Camera.h"
#include <Windows.h>


using namespace Tocs;
using namespace Tocs::Graphics;
using namespace Tocs::Parallel;
using namespace Tocs::Input;
using namespace std;


class Jet
{
	
public:
	unsigned int x, y;
	Color color;
	float fx,fy; 
	float size;

	Jet()
		: color(),x(0),y(0),fx(0),fy(0),size(5)	{}
	
	Jet (Color c, unsigned int x, unsigned int y)
		: color (c), x(x), y(y),fx(100),fy(100),size(5) {}

	void Appy (FluidField2D &field, CommandQueue &queue)
	{
		field.SetInk (x,y,color,size,queue);
		field.SetVel (x,y,fx,fy,size,queue);
	}

	void SetDir (float angle, float len)
	{
		fx = cos(angle) * len;
		fy = sin(angle) * len;
	}
};

int main ()
{
	int pause;
	cout << "Rendering..." << endl;
	int width = 400;
	int height = 400;
	SimpleWindow window ("Fluid",800,800,false,false);
	GraphicsContext gcontext (window);
	
	Device pdevice;
	Context pcontext (pdevice,gcontext);
	pdevice.PrintInfo ();

	CommandQueue queue (pcontext);
	FluidField2D::LoadStatics (pcontext);
	FluidField2D fluid (width,height,pcontext,queue);

	//Seed the random with the time, sorry not platform independant here.
	srand(GetTickCount ());


	gcontext.SetClearColor (Color(100,149,237));
	window.Show ();
	Ticker ticker;
	

	Jet jets [15];

	Jet mousejet (Color(255,0,0),0,0);

	unsigned int border = 20;

	for (int i = 0; i < 15; ++i)
	{
		jets[i].x = (float(rand()) / ((float)RAND_MAX))*(width - 2*border) + border;
		jets[i].y = (float(rand()) / ((float)RAND_MAX))*(height - 2*border) + border;
		jets[i].color = Color(50 + rand()%200, 50 + rand()%200, 50 + rand()%200);
		jets[i].size = 10 + (rand() / ((float)RAND_MAX))*3;
	}


	float totalt = 0;

	while (!window.IsExiting ())
	{
		float dt = ticker.GetTickTime (); //Gets time between frames.
		totalt += dt; //Keeps track of the total time.

		window.PumpMessages (); //Handles OS interation with the window
		gcontext.ClearActiveBuffer ();

		//Mouse Input and Random Jets of Ink affect the liquid here.

		for (int i = 0; i < 15; ++i)
		{
			jets[i].SetDir (sin(totalt*4) +  + jets[i].x,100 + sin(totalt + jets[i].y)*100);
			jets[i].Appy(fluid,queue);
		}
		window.Input.Update(dt);
		if (window.Input.Mouse.IsLeftButtonDown ())
		{
			unsigned int x = window.Input.Mouse.X;
			unsigned int y = window.Input.Mouse.Y;

			unsigned int px = window.Input.Mouse.PrevX;
			unsigned int py = window.Input.Mouse.PrevY;

			mousejet.x = Clamp<unsigned int>(x,10,width-border);
			mousejet.y = Clamp<unsigned int>(y,10,height-border);

			mousejet.fx = (x - px) * 1000;
			mousejet.fy = (y - py) * 1000;

			mousejet.Appy(fluid,queue);
		}


		fluid.Update (dt + 0.001,queue); //Update (0.001 prevents a division by 0)
		fluid.Draw (); //Render the liquid to OpenGL's backbuffer
		//quad.Draw(tex);
		gcontext.FlipToScreen (); //Present the backbuffer to the window.
	}
	window.Close ();


	cin >> pause;
	return 0;
}

/*
int main ()
{
	int pause;
	cout << "Rendering..." << endl;
	int width = 400;
	int height = 400;
	SimpleWindow wind ("Liquid",800,800,false,false);
	GraphicsContext gcontext (wind);
	srand(GetTickCount ());
	
	Device pdevice;
	Context pcontext (pdevice,gcontext);
	pdevice.PrintInfo ();

	CommandQueue queue (pcontext);

	FluidVolume::LoadStatics (pcontext);
	FluidVolume fluid (50,50,50,pcontext,queue);

	Camera cam(1);

	cam.Position (4,4,4);
	cam.LookAt(0,0,0);
	cam.Compute ();

	gcontext.SetClearColor (Color(100,149,237));
	wind.Show ();
	Ticker ticker;
	
	float totalt = 0;


	while (!wind.IsExiting ())
	{
		float dt = ticker.GetTickTime ();
		totalt += dt;

		wind.PumpMessages ();
		gcontext.ClearActiveBuffer ();

		wind.Input.Update(dt);
		
		fluid.Draw (cam);

		//quad.Draw(tex);
		gcontext.FlipToScreen ();
	}
	wind.Close ();


	cin >> pause;
	return 0;
}*/