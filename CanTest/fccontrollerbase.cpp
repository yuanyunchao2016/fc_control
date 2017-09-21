/*
 * fccontrollerbase.cpp
 *
 *  Created on: 2017-9-11
 *      Author: yuanyunchao
 */

#include "fccontrollerbase.h"

fc_controller_base::fc_controller_base() {
	// TODO Auto-generated constructor stub
	c_stage = IDLE ;
	time_counter = 0 ;
}

fc_controller_base::~fc_controller_base() {
	// TODO Auto-generated destructor stub

}

void fc_controller_base::set_dcdc_servo(servo_base *servo){
	this->dcdc_servo = servo ;
}

void fc_controller_base::set_c_servo(servo_base *servo){
	this->c_servo = servo ;
}

//void fc_controller_base::set_target(control_target *c_target){
//	this->c_target = c_target ;
//}

void fc_controller_base::init_controller(){
	c_stage = IDLE ;
}

void fc_controller_base::start_controller(){
	c_stage = STARTUP ;
}

void fc_controller_base::stop_controller(){

}

//should be called every 1s
void fc_controller_base::calculate_state_process(algo_base * algo,double time_cnt){
	//time_counter = time_counter + 1 ;//inc time
	if(algo != NULL)
		algo->calculate(time_cnt);
}

void fc_controller_base::servo_process(){
	//servo should start with single thread outside
}

//should be called every 500ms/300ms
void fc_controller_base::config_process(){
	if(this->dcdc_servo != NULL)
		this->dcdc_servo->keeplive();
	if(this->c_servo != NULL)
		this->c_servo->keeplive();
}
