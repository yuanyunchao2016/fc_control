/*
 * fcdcdcservo.h
 *
 *  Created on: 2017-9-11
 *      Author: yuanyunchao
 */

#ifndef FCDCDCSERVO_H_
#define FCDCDCSERVO_H_

using namespace std;

#include "canservo.h"
#include "controller_data_type.h"

#ifdef DEBUG
#include "stdio.h"
#endif

extern system_status global_status ;

class fc_dcdc_servo: public can_servo {
private :
	dc_type type = vapel;
	void build_vapel_control_frame(VCI_CAN_OBJ * frame);
	void build_vapel_set_frame(VCI_CAN_OBJ * frame);
	void build_raycc_control_frame(VCI_CAN_OBJ * frame);
public:
	fc_dcdc_servo(DWORD DeviceType,DWORD DeviceInd,int can_ind);
	virtual ~fc_dcdc_servo();
	virtual void rece_func(VCI_CAN_OBJ *rece , int len);
	virtual void keeplive();
	void set_type(dc_type type);
	virtual void init_servo();
};

#endif /* FCDCDCSERVO_H_ */
