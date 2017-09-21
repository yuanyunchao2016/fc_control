/*
 * controllerbase.h
 *
 *  Created on: 2017-9-14
 *      Author: yuanyunchao
 */

#ifndef CONTROLLERBASE_H_
#define CONTROLLERBASE_H_

#include "algobase.h"

class controller_base {
private:
	int time_counter = 0 ;
public:
	controller_base();
	virtual ~controller_base();

	virtual void init_controller();
	virtual void start_controller();
	virtual void stop_controller();

	virtual void calculate_state_process(algo_base * algo);
	virtual void servo_process();
	virtual void config_process();
};

#endif /* CONTROLLERBASE_H_ */
