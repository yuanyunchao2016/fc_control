/*
 * serialservo.cpp
 *
 *  Created on: 2017-10-9
 *      Author: yuanyunchao
 */

#include "serialservo.h"

serial_servo::serial_servo() {
	// TODO Auto-generated constructor stub
	fd = -1 ;
	ret = -1 ;

}

serial_servo::serial_servo(char * path) {
	// TODO Auto-generated constructor stub
	fd = -1 ;
	ret = -1 ;
	this->path = path ;
}

serial_servo::~serial_servo() {
	// TODO Auto-generated destructor stub
}


void serial_servo::init_servo(){
	fd = uart_open(fd,this->path);/*串口号/dev/ttySn,USB口号/dev/ttyUSBn*/
	if(fd == -1)
	{
		printf("uart_open error\n");
		exit(EXIT_FAILURE);
	}

	if(uart_set(fd,115200,0,8,'n',1) == -1)
	{
		printf("uart set failed!\n");
		exit(EXIT_FAILURE);
	}
}

void serial_servo::start_servo(){
	int ret1 = -1 ;
	this->init_servo();
	//char r_buf[1024];
	bzero(r_buf,1024);
	this->running = true ;
	while(this->running){
		ret1 = uart_read(fd,r_buf,1024);
		if(ret1 == -1){
			printf("uart read failed!\n");
			exit(EXIT_FAILURE);
		}
		else{
			this->rece_func(r_buf,ret1);
		}
	}
}


void serial_servo::stop_servo(){
	this->running = false ;
}

void serial_servo::rece_func(void* rece,int len){
#ifdef DEBUG
	printf("RECE:%s,len:%d.\n",(char*)rece,len);
#endif
	if(len>0){
		Document d;
		d.Parse((char*)rece);
		Value& s = d["HyTemp"];
		int t = s.GetInt();
		printf("HyTemp:%d\n",t);
	}
}

void serial_servo::send_frame(void* send,int len){
	ret = uart_write(fd,(char*)send,len);
	if(ret == -1){
		fprintf(stderr,"uart write failed!\n");
		exit(EXIT_FAILURE);
	}
}

void serial_servo::keeplive(){
	int w_len = this->build_serial_frame() ;
	this->send_frame(this->w_buf,w_len);
#ifdef DEBUG
	printf("Serial send:%s.\n",w_buf);
#endif
}

int serial_servo::build_serial_frame(){
	int air_rate,water_rate,fan_rate,w_len ;
	bzero(w_buf,1024);
	air_rate = global_status.air_status.air_flow_rate ;
	water_rate = global_status.cooler_status.water_flow_rate ;
	fan_rate = global_status.cooler_status.fan_rate ;
	w_len = sprintf(w_buf,"$%d|%d|%d#",air_rate,water_rate,fan_rate);
	return w_len ;
}
