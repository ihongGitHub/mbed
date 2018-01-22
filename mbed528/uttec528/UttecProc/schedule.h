#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"

/* Schedule Scenario 2018.01.21
*	1. Tx��  Database ����
*	2. Server���� �� Tx�� ���� Data ����
*	3. Gw������ ���۸� ��� 
* 4. Tx�� �Ϸ� Cycle�� Data����
*	5. Max Event�� 12���� ����(2ȸ ����)
* 6. Event�� 10�� ������ ����(24*6 = 144)
*/
#define DeScheduleEventMax 12
#define DeScheduleInterval 6 //10 minutes

typedef struct{
	uint8_t id;
	uint8_t time;
	uint8_t level;
} sEvent_t;

typedef struct{
	bool flag;
	uint8_t total;
	uint8_t now;
	sEvent_t event[12];
} schedule_t;

class schedule
{
private:
//	static schedule_t m_schedule;

public:	
	schedule();
	void initSchedule();
	void setSchedule(uint8_t, uint8_t);
	void procSchedule();
};	

#endif
