/*
 * fclinuxrunner.cpp
 *
 *  Created on: 2017-9-19
 *      Author: yuanyunchao
 */

#include "fclinuxrunner.h"

fc_linux_runner::fc_linux_runner() {
	// TODO Auto-generated constructor stub

}

fc_linux_runner::~fc_linux_runner() {
	// TODO Auto-generated destructor stub
}

void fc_linux_runner::start(){
	this->running = true ;
	std::thread task_500(&fc_linux_runner::thread_task_500ms,this);
	task_500.detach();
	std::thread task_300(&fc_linux_runner::thread_task_300ms,this);
	task_300.detach();
}

void fc_linux_runner::stop(){
	this->running = false ;
}


void fc_linux_runner::init(){
	if(this->controller != NULL){
		this->controller->set_dcdc_servo(this->dc_servo);
		this->controller->set_c_servo(NULL);
	}
	//start servo
	if(this->dc_servo!= NULL){
		std::thread task_dc_servo(&fc_dcdc_servo::start_servo,this->dc_servo);
		task_dc_servo.detach();
	}
}

void fc_linux_runner::thread_task_500ms(){
	time_t t = time(0) ;
	while(this->running){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		time(&t);
#ifdef DEBUG
		printf("Thread 500ms is called.Current time is %.2f\n",(double)t);
#endif
		//calculate process
		if(this->controller != NULL) {
			//printf("1\n");
			this->controller->calculate_state_process(this->algo,(double)t);
		}
	}
}

void fc_linux_runner::thread_task_300ms(){
	time_t t = time(0) ;
	while(this->running){
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		time(&t);
#ifdef DEBUG
		printf("Thread 300ms is called.Current time is %.2f\n",(double)t);
#endif
		if(this->controller != NULL) {
			this->controller->config_process();
		}
	}
}
