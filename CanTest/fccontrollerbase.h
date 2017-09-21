/*
 * fccontrollerbase.h
 *
 *  Created on: 2017-9-11
 *      Author: yuanyunchao
 */

#ifndef FCCONTROLLERBASE_H_
#define FCCONTROLLERBASE_H_

#include "controller_data_type.h"
#include "servobase.h"
#include "controllerbase.h"

#include "algobase.h"

class fc_controller_base {
private:
	servo_base *dcdc_servo = NULL ;
	servo_base *c_servo = NULL ;
	int servo_num = 0 ;
	running_stage c_stage ;

	int time_counter ;
public:
	fc_controller_base();
	virtual ~fc_controller_base();

	void set_dcdc_servo(servo_base *servo);
	void set_c_servo(servo_base *servo);
	//void set_target(control_target *c_target);

	virtual void init_controller();
	virtual void start_controller();
	virtual void stop_controller();

	virtual void calculate_state_process(algo_base * algo,double time_cnt);
	virtual void servo_process();
	virtual void config_process();

};

#endif /* FCCONTROLLERBASE_H_ */
