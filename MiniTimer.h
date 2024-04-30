// Implementation of stripped down version from here https://github.com/sstaub/Timer
#ifndef MINI_TIMER_H
#define MINI_TIMER_H

#include "Arduino.h"

enum status_t {STOPPED, RUNNING};

class MiniTimer {

public:

	MiniTimer() {}
	~MiniTimer() {}
	
	void start() {
		elapsed = 0;
		started = millis();
		status = RUNNING;
	}
	
	void stop() {
		if (status == RUNNING) {
			elapsed = millis() - started;
		}
		status = STOPPED;
	}
	
	uint32_t read() {
		if (status == RUNNING) {
			return millis() - started + elapsed;
		}
		return elapsed;
	}
	
private:

	uint32_t started;
	uint32_t elapsed;
	status_t status;
};

#endif
