/*
 * controllerbase.cpp
 *
 *  Created on: 2017-9-14
 *      Author: yuanyunchao
 */

#include "controllerbase.h"

controller_base::controller_base() {
	// TODO Auto-generated constructor stub

}

controller_base::~controller_base() {
	// TODO Auto-generated destructor stub
}

void controller_base::init_controller(){
	//global_status.stage = idle ;
}

void controller_base::start_controller(){

}

void controller_base::stop_controller(){

}

void controller_base::calculate_state_process(algo_base * algo){
	this->time_counter ++ ;
	algo->calculate(this->time_counter);
}

void controller_base::servo_process(){

}

void controller_base::config_process(){

}
