#pragma once
#include "GUI.h"
class Time {
public:
	Time() {
		QueryPerformanceCounter(&start_time);
		QueryPerformanceCounter(&last_time);
		QueryPerformanceCounter(&current_time);
		QueryPerformanceFrequency(&freq);
	}
	~Time() {}
	// Get timer life time
	double GetTime() {
		QueryPerformanceCounter(&current_time);
		return ((double)current_time.QuadPart / (double)freq.QuadPart) - ((double)start_time.QuadPart / (double)freq.QuadPart);
	}
	// Get time since last reset
	double GetTimeElapsed() {
		QueryPerformanceCounter(&current_time);
		return ((double)current_time.QuadPart / (double)freq.QuadPart) - ((double)last_time.QuadPart / (double)freq.QuadPart);
	}
	void ResetTimer() {
		QueryPerformanceCounter(&last_time);
		//timer_expired_count += GetTimeElapsedCount();
	}
	// Set time to timer
	//void SetTimer(double count_time) {
	//	this->count_time = ((count_time != 0) ? count_time : 1.0);
	//}
	// How many times timer expired all time
	//int GetTimeCount() {
	//	return timer_expired_count + GetTimeElapsedCount();
	//}
	// How many times timer expired after reset
	//int GetTimeElapsedCount() {
	//	return timer_expired_count_last = (int)(GetTimeElapsed() / count_time);
	//}
private:
	LARGE_INTEGER start_time, last_time, current_time, freq;
	//double count_time = 1.0;
	//int timer_expired_count = 0, timer_expired_count_last = 0;
};