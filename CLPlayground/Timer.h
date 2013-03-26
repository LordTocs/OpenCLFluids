#pragma once


class Timer
{
	unsigned int StartTime;
public:
	Timer();
	~Timer();

	float GetTime ();

	void Reset ();
};

