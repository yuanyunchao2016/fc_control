/*
 * cancard.h
 *
 *  Created on: 2017-9-6
 *      Author: yuanyunchao
 */

#ifndef CANCARD_H_
#define CANCARD_H_

#include "controlcan.h"
#include <pthread.h>

struct can_status{
	bool IF_OPENED =FALSE ;
	bool IF_INIT = FALSE ;
	bool IF_START = FALSE ;
	bool INFO_ERR = FALSE ;
	bool SEND_ERR = FALSE ;
	bool RECE_ERR = FALSE ;
};

class can_card {
private:
	DWORD DeviceType;
	DWORD DeviceInd;
	//VCI_CAN_OBJ * rece_buffer ;

	can_status status ;
	PVCI_BOARD_INFO  pinfo ;
protected:
	can_status init();
public:
	//can_card();
	can_card(DWORD DeviceType,DWORD DeviceInd);
	virtual ~can_card();

	PVCI_BOARD_INFO get_board_info();
	can_status get_status();
	can_status init_can(int can_ind ,VCI_INIT_CONFIG config);
	can_status send(DWORD can_ind,VCI_CAN_OBJ * send ,int len);
	int rece(DWORD can_ind,VCI_CAN_OBJ * rece,int max_len);
};

#endif /* CANCARD_H_ */
