#include "Timer.h"
#include <Windows.h>

Timer::Timer(void)
	: StartTime (GetTickCount ())
{
}


Timer::~Timer(void)
{
}


float Timer::GetTime ()
{
	unsigned int time = GetTickCount ();
	return (time - StartTime)/1000.0f;
}

void Timer::Reset ()
{
	StartTime = GetTickCount ();
}