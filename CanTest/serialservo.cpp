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
	frame_id = 0 ;

}

serial_servo::serial_servo(char * path) {
	// TODO Auto-generated constructor stub
	fd = -1 ;
	ret = -1 ;
	frame_id = 0 ;
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
	double v,c,p1,p2,p3,p4,t1,t2,t3,t4,t5,t6 ;
	Document d;
#ifdef DEBUG
	printf("RECE:%s,len:%d.\n",(char*)rece,len);
//	for(int i =0 ; i < len ; i ++){
//		printf("%x",((char*)rece)[i]);
//	}
//	printf("\n");
#endif
	if(len>0){
		d.Parse((char*)rece);
		if(d.HasParseError()){
#ifdef DEBUG
			printf("\tJson parse error.\n");
#endif
			return ;
		}
		if(!d.HasMember("v")){
#ifdef DEBUG
			printf("\tJson key value error.\n");
#endif
			return;
		}
		v = d["v"].GetDouble();
		c = d["c"].GetDouble();
		p1 = d["p1"].GetDouble();
		p2 = d["p2"].GetDouble();
		p3 = d["p3"].GetDouble();
		p4 = d["p4"].GetDouble();
		t1 = d["t1"].GetDouble();
		t2 = d["t2"].GetDouble();
		t3 = d["t3"].GetDouble();
		t4 = d["t4"].GetDouble();
		t5 = d["t5"].GetDouble();
		t6 = d["t6"].GetDouble();
#ifdef DEBUG
		printf("v:%.2f,c:%.2f,p1:%.2f,p2:%.2f,p3:%.2f,p4:%.2f,t1:%.2f,t2:%.2f,t3:%.2f,t4:%.2f,t5:%.2f,t6:%.2f.\n",
				v,c,p1,p2,p3,p4,t1,t2,t3,t4,t5,t6);
#endif
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
	double v_set,i_set ;
	bzero(w_buf,1024);
	v_set = global_status.dc_status.v_set ;
	i_set = global_status.dc_status.max_current ;
	air_rate = global_status.air_status.air_flow_rate ;
	water_rate = global_status.cooler_status.water_flow_rate ;
	fan_rate = global_status.cooler_status.fan_rate ;
	w_len = sprintf(w_buf,"$%d:%.2f:%.2f:%d:%d#",frame_id,v_set,i_set,water_rate,air_rate);
	frame_id ++ ;
	return w_len ;
}
