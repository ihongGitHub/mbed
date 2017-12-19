#ifndef __RS485_H__
#define __RS485_H__

#include <stdint.h>
#include <stdbool.h>

#include "DimmerRf.h"

class rs485
{
private:
	static bool m_doneFlag;
	static rfFrame_t m_485Rx;

public:
	rs485();
	bool is485Done();
	void clear485Done();
	void task485(rfFrame_t*);

	rfFrame_t* return485Buf();
};
#endif
