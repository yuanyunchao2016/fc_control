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
#include "serialservo.h"

int main(int argc,char *argv[]) {
	printf("Start.\n");
#ifdef DEBUG
	for(int i = 0 ; i < argc ; i ++){
		printf("para:%s\n",argv[i]);
	}
#endif


	serial_servo * c_servo = new serial_servo();
	if(argc > 1){
		c_servo->set_path(argv[1]);
	}
	else{
		c_servo->set_path((char*)"/dev/ttyUSB0");
	}

	fc_dcdc_servo *dc_servo = new fc_dcdc_servo(VCI_USBCAN2,0,0);
	dc_servo->set_type(raycc);
	fc_algo_simple *algo = new fc_algo_simple();
	fc_controller_base *control = new fc_controller_base();

	fc_linux_runner *runner = new fc_linux_runner();
	runner->c_servo = c_servo ;
	runner->dc_servo = dc_servo ;

	runner->algo = algo ;
	runner->controller = control ;
	runner->init();
	runner->start();
	//std::this_thread::sleep_for(std::chrono::seconds(60));
	getchar();
	c_servo->stop_servo();
	return EXIT_SUCCESS;
}
