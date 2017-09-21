/*
 * canservo.cpp
 *
 *  Created on: 2017-9-6
 *      Author: yuanyunchao
 */

#include "canservo.h"
#include "stdio.h"

//can_servo::can_servo() {
//	// TODO Auto-generated constructor stub
//	this->can_servo(VCI_USBCAN2,0,0);
//}

can_servo::can_servo(DWORD DeviceType = VCI_USBCAN2,DWORD DeviceInd = 0 ,int can_ind = 0):can_card(DeviceType,DeviceInd){
	this->MAX_BUFFER_LEN = 10 ;
	this->can_ind = can_ind ;
}

can_servo::~can_servo() {
	// TODO Auto-generated destructor stub
}

void can_servo::rece_func(VCI_CAN_OBJ *rece ,int len){
	if(len>0 && len < 2500){
		for(int i = 0 ; i < len ; i ++){
			printf("ID=%x,SendType=%x,DataLen=%d.\n",rece[i].ID,rece[i].SendType,rece[i].DataLen);
			printf("    Data=");
			for(int j=0 ; j < rece[i].DataLen ; j ++){
				printf("%x",rece[i].Data[j]);
			}
			printf("\n");
		}
	}
}

void can_servo::init_servo(){
	can_status status = this->init();
	int reinit_cnt = 0 ;
	while(status.IF_OPENED == FALSE && reinit_cnt++ < 3){
		status = this->init();
	}
	VCI_INIT_CONFIG config;
	config.AccCode=0;
	config.AccMask=0xffffffff;
	config.Filter=1;
	config.Mode=0;
	config.Timing0=0x01;
	config.Timing1=0x1C;
	this->init_can(this->can_ind,config);
}

//TODO: start with new thread
void can_servo::start_servo(){
	this->running = true ;
	VCI_CAN_OBJ rece[2500];
	int rece_len = -1 ;

	this->init_servo();

	while(this->running && this->get_status().RECE_ERR == false
			&& this->get_status().IF_START == true){
		rece_len = this->rece(this->can_ind,rece,2500);
		//if(rece_len>0){
		this->rece_func(rece,rece_len);
		//}
	}
}

void can_servo::stop_servo(){
	this->running = FALSE ;
}

void can_servo::send_frame(VCI_CAN_OBJ * send ,int len){
	this->send(this->can_ind,send,len);
}

void can_servo::keeplive(){
}
