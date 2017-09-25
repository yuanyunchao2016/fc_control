/*
 * canservo.h
 *
 *  Created on: 2017-9-6
 *      Author: yuanyunchao
 */

#ifndef CANSERVO_H_
#define CANSERVO_H_

#include "cancard.h"
#include "servobase.h"

class can_servo: public can_card,public servo_base {
private:
	//bool running = FALSE ;
public:
	int can_ind ;
	int MAX_BUFFER_LEN = 10  ;
	//can_servo();
	can_servo(DWORD DeviceType,DWORD DeviceInd,int can_ind);
	virtual ~can_servo();
	virtual void rece_func(VCI_CAN_OBJ *rece , int len);
	virtual void init_servo();
	virtual void start_servo();
	virtual void stop_servo();
	virtual void send_frame(VCI_CAN_OBJ * send ,int len);
	virtual void keeplive();
};

#endif /* CANSERVO_H_ */
