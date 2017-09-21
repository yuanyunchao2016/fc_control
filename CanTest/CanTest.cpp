//============================================================================
// Name        : CanTest.cpp
// Author      : yuanyunchao
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "fclinuxrunner.h"

int main(void) {
	printf("Start.\n");
	fc_dcdc_servo *dc_servo = new fc_dcdc_servo(VCI_USBCAN2,0,0);
	//dc_servo->start_servo();
	//std::thread task_dc_servo(&fc_dcdc_servo::start_servo,dc_servo);
	fc_linux_runner *runner = new fc_linux_runner();
	runner->dc_servo = dc_servo ;
	runner->init();
	runner->start();
	std::this_thread::sleep_for(std::chrono::seconds(60));
	return EXIT_SUCCESS;
}
