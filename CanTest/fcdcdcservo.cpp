/*
 * fcdcdcservo.cpp
 *
 *  Created on: 2017-9-11
 *      Author: yuanyunchao
 */

#include "fcdcdcservo.h"

fc_dcdc_servo::fc_dcdc_servo(DWORD DeviceType,DWORD DeviceInd,int can_ind):can_servo(DeviceType,DeviceInd,can_ind) {
	// TODO Auto-generated constructor stub

}

fc_dcdc_servo::~fc_dcdc_servo() {
	// TODO Auto-generated destructor stub
}

void fc_dcdc_servo::rece_func(VCI_CAN_OBJ*rece,int len){
	//printf("dcdc\n");
	if(len>0 && len < 2500){
		for(int i = 0 ; i < len ; i ++){
			switch(rece[i].ID){
			case 0x1c0080a7:{
				//vapel
				global_status.dc_status.dc_output_voltage = (double)(((int)rece[i].Data[1]<<8) + rece[i].Data[0])/10.0;
				global_status.dc_status.dc_output_current = (double)(((int)rece[i].Data[3]<<8) + rece[i].Data[2])/100.0;
				global_status.dc_status.dc_input_voltage  = (double)(((int)rece[i].Data[5]<<8) + rece[i].Data[4])/100.0;
				global_status.dc_status.dc_temperature    = rece[i].Data[6];
				global_status.dc_status.output_voltage_over_flow 	=  rece[i].Data[7] & 0x1 ;
				global_status.dc_status.over_temperature 			= (rece[i].Data[7] & 0x2 )>>1 ;
				global_status.dc_status.input_voltage_under_flow 	= (rece[i].Data[7] & 0x4 )>>2 ;
				global_status.dc_status.input_voltage_over_flow 	= (rece[i].Data[7] & 0x8 )>>3 ;
				global_status.dc_status.output_voltage_under_flow 	= (rece[i].Data[7] & 0x10)>>4 ;
				global_status.dc_status.output_short 				= (rece[i].Data[7] & 0x20)>>5 ;
				global_status.dc_status.auto_shutdown 				= (rece[i].Data[7] & 0x40)>>6 ;
				global_status.dc_status.dc_on_off 					= (rece[i].Data[7] & 0x80)>>7 ;
#ifdef DEBUG
				printf("Rece vapel status frame: dc_output_voltage: %.2f ; dc_output_current : %.2f .\n",
						global_status.dc_status.dc_output_voltage, global_status.dc_status.dc_output_current);
				printf("\tdc_input_voltage: %.2f , dc_temperature: %d .\n",
						global_status.dc_status.dc_input_voltage,global_status.dc_status.dc_temperature);
#endif
				break;
			}
			case 0x18006496:{
				//raycc
				global_status.dc_status.dc_output_voltage = (double)(((int)rece[i].Data[0]<<8) + rece[i].Data[1])/10.0;
				global_status.dc_status.dc_output_current = (double)(((int)rece[i].Data[2]<<8) + rece[i].Data[3])/100.0;
				global_status.dc_status.dc_input_voltage = (double)(((int)rece[i].Data[4]<<8) + rece[i].Data[5])/10.0;
				global_status.dc_status.dc_input_current = (double)(((int)rece[i].Data[6]<<8) + rece[i].Data[7])/100.0;
#ifdef DEBUG
				printf("Rece raycc status frame: dc_output_voltage: %.2f ; dc_output_current : %.2f .\n",
						global_status.dc_status.dc_output_voltage, global_status.dc_status.dc_output_current);
				printf("\tdc_input_voltage: %.2f , dc_input_current: %.2f .\n",
						global_status.dc_status.dc_input_voltage,global_status.dc_status.dc_input_current);
#endif
				break;
			}
			case 0x18016496:{
				//raycc
				global_status.dc_status.dc_temperature 	= rece[i].Data[0];
				global_status.dc_status.dc_on_off 		= rece[i].Data[1];
				global_status.dc_status.over_temperature 			=  rece[i].Data[2] & 0x1 ;
				global_status.dc_status.output_voltage_over_flow 	= (rece[i].Data[2] & 0x2 )>>1 ;
				global_status.dc_status.output_voltage_under_flow 	= (rece[i].Data[2] & 0x4 )>>2 ;
				global_status.dc_status.input_voltage_over_flow 	= (rece[i].Data[2] & 0x8 )>>3 ;
				global_status.dc_status.input_voltage_under_flow 	= (rece[i].Data[2] & 0x10)>>4 ;
				global_status.dc_status.output_current_over_flow 	= (rece[i].Data[2] & 0x20)>>5 ;
				global_status.dc_status.communication_error 		= (rece[i].Data[2] & 0x40)>>6 ;
				break;
			}
			default:{
#ifdef DEBUG
				printf("Unresolved CAN frame data.\n");
				printf("ID=%x,SendType=%x,DataLen=%d.\n",rece[i].ID,rece[i].SendType,rece[i].DataLen);
				printf("    Data=");
				for(int j=0 ; j < rece[i].DataLen ; j ++){
					printf("%x",rece[i].Data[j]);
				}
				printf("\n");
#endif
			}
			}
		}
	}
}

//should be called every 500ms
void fc_dcdc_servo::keeplive(){
	VCI_CAN_OBJ frame ;
	switch(this->type){
	case fenergy: break;
	case vapel :{
		this->build_vapel_control_frame(&frame);
		this->send_frame(&frame,1);
		this->build_vapel_set_frame(&frame);
		this->send_frame(&frame,1);
		break;
	}
	case raycc:{
		this->build_raycc_control_frame(&frame);
		this->send_frame(&frame,1);
		break;
	}
	default:{
#ifdef DEBUG
		printf("No define dcdc type.");
#endif
	}
	}
}

void fc_dcdc_servo::set_type(dc_type type){
	this->type = type;
}

void fc_dcdc_servo::build_vapel_control_frame(VCI_CAN_OBJ * frame){
	frame->ID = 0x1cf1a780;
	frame->DataLen = 8 ;
	frame->Data[0] = (BYTE)(global_status.dc_status.max_power & 0xff) ;
	frame->Data[1] = (BYTE)((global_status.dc_status.max_power & 0xff00)>>8) ;
	frame->Data[2] = (BYTE)(global_status.dc_status.power_inc_rate & 0xff) ;
	frame->Data[3] = (BYTE)((global_status.dc_status.power_inc_rate & 0xff00)>>8) ;
	frame->Data[4] = (BYTE)(global_status.dc_status.power_dec_rate & 0xff) ;
	frame->Data[5] = (BYTE)((global_status.dc_status.power_dec_rate & 0xff00)>>8) ;
	BYTE control_byte0, control_byte1 ;
	if(global_status.dc_status.set_on){
		control_byte0 = 0x32;
		control_byte1 = 0x00;
	}
	else{
		control_byte0 = 0xc0;
		control_byte1 = 0x00;
	}
	frame->Data[6] = control_byte0 ;
	frame->Data[7] = control_byte1 ;
}

void fc_dcdc_servo::build_vapel_set_frame(VCI_CAN_OBJ * frame){
	frame->ID = 0x1cf2a780 ;
	frame->DataLen = 8 ;
	frame->Data[0] = (BYTE)((int)global_status.dc_status.v_set & 0xff) ;
	frame->Data[1] = (BYTE)(((int)global_status.dc_status.v_set & 0xff00)>>8) ;
	frame->Data[2] = (BYTE)((int)global_status.dc_status.max_current & 0xff) ;
	frame->Data[3] = (BYTE)(((int)global_status.dc_status.max_current & 0xff00)>>8) ;
}

void fc_dcdc_servo::build_raycc_control_frame(VCI_CAN_OBJ * frame){
	frame->ID = 0x1c009664;
	frame->DataLen = 8 ;
	frame->Data[0] = (BYTE)global_status.dc_status.set_on ;
	frame->Data[2] = (BYTE)((int)global_status.dc_status.v_set & 0xff) ;
	frame->Data[1] = (BYTE)(((int)global_status.dc_status.v_set & 0xff00)>>8) ;
	frame->Data[4] = (BYTE)((int)global_status.dc_status.max_current & 0xff) ;
	frame->Data[3] = (BYTE)(((int)global_status.dc_status.max_current & 0xff00)>>8) ;
}

