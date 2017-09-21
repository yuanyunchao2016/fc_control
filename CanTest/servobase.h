/*
 * servobase.h
 *
 *  Created on: 2017-9-14
 *      Author: yuanyunchao
 */

#ifndef SERVOBASE_H_
#define SERVOBASE_H_
#include "controller_data_type.h"

class servo_base {
public:
	bool running = false ;
	servo_base();
	virtual ~servo_base();

	virtual void rece_func(void *rece , int len);
	virtual void init_servo();
	virtual void start_servo();
	virtual void stop_servo();
	virtual void send_frame(void * send ,int len);
	virtual void keeplive();
};

#endif /* SERVOBASE_H_ */
