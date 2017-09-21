/*
 * cancard.cpp
 *
 *  Created on: 2017-9-6
 *      Author: yuanyunchao
 */

#include "cancard.h"
#include "stdio.h"
//can_card::can_card() {
//	// TODO Auto-generated constructor stub
//	this->can_card(VCI_USBCAN2,0);
//}

can_card::can_card(DWORD DeviceType = VCI_USBCAN2,DWORD DeviceInd = 0){
	this->DeviceType = DeviceType ;
	this->DeviceInd = DeviceInd ;
	this->pinfo = new VCI_BOARD_INFO;
	//this->rece_buffer = new VCI_CAN_OBJ [100] ;
	//this->init();
}

can_card::~can_card() {
	// TODO Auto-generated destructor stub
	VCI_CloseDevice(this->DeviceType,this->DeviceInd);
}

can_status can_card:: get_status(){
	return this->status ;
}

PVCI_BOARD_INFO can_card::get_board_info() {
	this->pinfo = new VCI_BOARD_INFO;
	if(this->status.IF_OPENED == FALSE){
		this->init();
	}
	if(VCI_ReadBoardInfo(this->DeviceType,this->DeviceInd,pinfo)==1){
		this->status.INFO_ERR = FALSE ;
		return pinfo ;
	}
	else{
		this->status.INFO_ERR = TRUE ;
		return NULL;
	}
}

can_status can_card::init(){
	//reopen can port
	if(this->status.IF_OPENED == TRUE) {
		VCI_CloseDevice(this->DeviceType,this->DeviceInd);
		this->status.IF_OPENED = FALSE ;
	}
	if(VCI_OpenDevice(this->DeviceType,this->DeviceInd, 0)==1){
		this->status.IF_OPENED = TRUE ;
	}
	return this->status ;
}

can_status can_card::init_can(int can_ind ,VCI_INIT_CONFIG config){
	if(this->status.IF_OPENED == FALSE){
		this->init();
	}
	if(this->get_board_info() != NULL){
		if(VCI_InitCAN(this->DeviceType,this->DeviceInd,can_ind,&config)!=1){
			this->status.IF_INIT = FALSE ;
			return this->status ;
		}
		else {
			this->status.IF_INIT = TRUE ;
		}
		if(VCI_StartCAN(this->DeviceType,this->DeviceInd,can_ind)!=1) {
			this->status.IF_START = FALSE ;
			return this->status;
		}
		else{
			this->status.IF_START = TRUE ;
		}
	}
	return this->status ;
}

can_status can_card::send(DWORD can_ind,VCI_CAN_OBJ * send ,int len){
	if(VCI_Transmit(this->DeviceType, this->DeviceInd, can_ind, send, len) > 0){
		this->status.SEND_ERR = FALSE ;
	}
	return this->status;
}

int can_card::rece(DWORD can_ind,VCI_CAN_OBJ * rece,int max_len){
	int len = -1 ;
	len=VCI_Receive(this->DeviceType,this->DeviceInd,can_ind,rece,max_len,100000);
	if(len<0){
		this->status.RECE_ERR = TRUE;
	}
	else {
		this->status.RECE_ERR = FALSE ;
	}
	return len ;
}

