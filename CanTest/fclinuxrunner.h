/*
 * fclinuxrunner.h
 *
 *  Created on: 2017-9-19
 *      Author: yuanyunchao
 */

#ifndef FCLINUXRUNNER_H_
#define FCLINUXRUNNER_H_

#include "controlrunner.h"
#include "fcdcdcservo.h"
#include "fccontrollerbase.h"
#include "fcalgosimple.h"

#include <iostream>
#include <utility>
#include <functional>
#include <thread>
#include <chrono>
#include <utility>
#include <atomic>
#include <time.h>

class fc_linux_runner: public control_runner {
private :
	bool running = false ;
public:
	fc_algo_simple *algo = NULL;
	fc_dcdc_servo *dc_servo = NULL ;
	fc_controller_base *controller = NULL ;
	fc_linux_runner();
	virtual ~fc_linux_runner();
	virtual void start();
	virtual void stop();

	void init();
	void thread_task_500ms();
	void thread_task_300ms();

};

#endif /* FCLINUXRUNNER_H_ */
