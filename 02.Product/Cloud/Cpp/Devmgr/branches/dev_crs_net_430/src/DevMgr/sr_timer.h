
#ifndef SR_TIMER_H
#define SR_TIMER_H

#include "NetworkInterface.h"

class SR_Timer : public ITimerEvent
{
public:
	SR_Timer(){};
	~SR_Timer(){};

	void OnTimerEvent() {}
private:

};

#endif //SR_TIMER_H