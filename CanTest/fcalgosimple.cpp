/*
 * fcalgosimple.cpp
 *
 *  Created on: 2017-9-14
 *      Author: yuanyunchao
 */

#include "fcalgosimple.h"

fc_algo_simple::fc_algo_simple() {
	// TODO Auto-generated constructor stub
	this->his_data = new fc_ref_data[HIS_BUFFER_LEN];
	//	for(int i = 0 ;i < HIS_BUFFER_LEN ; i ++){
	//		this->his_data[i] = new fc_ref_data ;
	//	}
	this->cur_ptr = 0 ;
	this->loop_cnt = 0 ;
}

fc_algo_simple::~fc_algo_simple() {
	// TODO Auto-generated destructor stub
	delete this->his_data ;
}

void fc_algo_simple::calculate(double time_cnt){
	fc_ref_data *last ;
	if(cur_ptr == 0){
		last = &his_data[HIS_BUFFER_LEN-1];
	}
	else {
		last = &his_data[cur_ptr - 1];
	}
	//filter the data
	if(time_cnt - last->time_counter < 1) {
		return ;
	}
	this->loop_cnt ++ ;
	fc_ref_data *cur = &his_data[cur_ptr] ;
	his_data[cur_ptr].stack_voltage = global_status.dc_status.dc_input_voltage ;
	his_data[cur_ptr].stack_current = global_status.dc_status.dc_input_current ;
	his_data[cur_ptr].output_voltage = global_status.dc_status.dc_output_voltage ;
	his_data[cur_ptr].output_current = global_status.dc_status.dc_output_current ;
	his_data[cur_ptr].dc_temperature = global_status.dc_status.dc_temperature ;
	his_data[cur_ptr].time_counter = time_cnt ;

#ifdef DEBUG
	printf("FC_ALGO_SIMPLE calculate receives global status,cur_ptr=%d, time_cnt:%.2f.\n",cur_ptr,cur->time_counter);
	printf("\tStack voltage:%.2f.Stack current:%.2f.\n",cur->stack_voltage,cur->stack_current);
	printf("\tDCDC output voltage:%.2f. DCDC output current:%.2f.\n",cur->output_voltage,cur->output_current);
	printf("\tDCDC temperature:%.1f.\n\n",cur->dc_temperature);
#endif

	defection_diagnose();
	output_implement();

	global_status.dc_status.set_on = cur->dc_on_set ;
	global_status.dc_status.v_set = cur->output_voltage_set ;
	global_status.dc_status.max_current = cur->output_max_current_set;
	global_status.air_status.air_flow_rate = cur->air_flow_rate_set ;
	global_status.hy_status.purge_interval = cur->hy_purge_set ;
	global_status.cooler_status.water_flow_rate = cur->water_flow_rate_set ;

#ifdef DEBUG
	char * s ;
	switch(cur->defect){
	case NORMAL: s = (char*)"NORMAL" ; break;
	case NEW_START : s = (char*)"NEW_START"; break;
	case RE_CONN: s = (char*)"RE_CONN" ; break;
	case WARMUP: s = (char*)"WARMUP" ; break;
	case LOW_POWER: s = (char*)"LOW_POWER" ; break;
	default: s = (char*)"DEFAULT";
	}
	printf("FC_ALGO_SIMPLE calculate result. State is %s.\n",s);
	printf("\tDCDC set is %d. V_set = %.2f. Max_current = %.2f.\n",
			cur->dc_on_set,cur->output_voltage_set,cur->output_max_current_set);
	printf("\tAir flow rate is %d%%. Water flow rate is %d%%. Hy purge interval is %d.\n\n",
			cur->air_flow_rate_set,cur->water_flow_rate_set,cur->hy_purge_set);
#endif
	refresh_history();
}

void fc_algo_simple::refresh_history(){
	cur_ptr ++ ;
	if(cur_ptr >= HIS_BUFFER_LEN){
		cur_ptr = 0 ;
	}
}

void fc_algo_simple::defection_diagnose(){
	fc_ref_data *cur = &his_data[cur_ptr] ;
	fc_ref_data *last ;
	int ptr,i ;
	if(loop_cnt < HIS_BUFFER_LEN){
		cur->defect = NEW_START ;
		return ;
	}
	if(cur_ptr == 0){
		last = &his_data[HIS_BUFFER_LEN-1];
	}
	else {
		last = &his_data[cur_ptr - 1];
	}
	if(loop_cnt < 60){
		cur->defect = NEW_START ;
	}
	else{
		//state machine
		//printf("\nlast defect:%d\n",last->defect);
		switch(last->defect){
		case NORMAL:{
			if(if_lowpower(cur->stack_voltage,cur->stack_current)){
				cur->defect = LOW_POWER ;
			}
			else if(cur->time_counter - last->time_counter > 5){
				cur->defect = RE_CONN ;
			}
			else if(if_cold_system(cur->dc_temperature)){
				cur->defect = WARMUP ;
			}
			else {
				cur->defect = NORMAL ;
			}
			break ;
		}
		case NEW_START:{
			for(i = 1 ; i < HIS_BUFFER_LEN ; i ++){
				ptr = cur_ptr - i ;
				if(ptr < 0) {
					ptr = ptr + HIS_BUFFER_LEN ;
				}
				if(his_data[ptr].defect == NEW_START && his_data[ptr].stack_voltage >= STACK_OPEN_VOLTAGE){
					//cur.defect = NEW_START ;
					continue;
				}
				else{
					break;
				}
			}
			if(i >= 60) {
				if(if_cold_system(cur->dc_temperature)){
					cur->defect = WARMUP ;
				}
				else{
					cur->defect = NORMAL ;
				}
			}
			else{
				cur->defect = NEW_START ;
			}

			break ;
		}
		case RE_CONN :{
			if(cur->output_current > 5){
				//dcdc is on
				if(if_lowpower(cur->stack_voltage,cur->stack_current)){
					cur->defect = LOW_POWER ;
				}
				else{
					cur->defect = NORMAL ;
				}
			}
			else{
				cur->defect = NEW_START ;
			}
			break ;
		}
		case LOW_POWER :{
			if(!if_lowpower(cur->stack_voltage,cur->stack_current) ){
				for(i = 1 ; i < HIS_BUFFER_LEN ; i ++){
					ptr = cur_ptr - i ;
					if(ptr < 0) {
						ptr = ptr + HIS_BUFFER_LEN ;
					}
					if(his_data[ptr].defect == LOW_POWER
							&&
							!if_lowpower(his_data[ptr].stack_voltage,his_data[ptr].stack_current)){
						//cur.defect = NEW_START ;
						continue ;
					}
					else {
						break ;
					}
				}
				if(i > 60)
					cur->defect = NORMAL ;
				else
					cur->defect = LOW_POWER ;
			}
			else{
				cur->defect = LOW_POWER;
			}
			break ;
		}
		case WARMUP:{
			if(!if_cold_system(cur->dc_temperature)){
				cur->defect = NORMAL ;
			}
			else {
				cur->defect = WARMUP ;
			}
			break ;
		}
		default:{
			cur->defect = NEW_START ;
		}
		}
	}
}

void fc_algo_simple::output_implement(){
	fc_ref_data *cur = &his_data[cur_ptr] ;
//	fc_ref_data *last ;
//	if(cur_ptr == 0){
//		last = &his_data[HIS_BUFFER_LEN-1];
//	}
//	else {
//		last = &his_data[cur_ptr - 1];
//	}
	switch(cur->defect){
	case NORMAL:{
		normal_control(this->max_power);
		break ;
	}
	case NEW_START:{
		cur->dc_on_set = false ;
		cur->output_voltage_set = MIN_OUTPUT_VOLTAGE ;
		cur->output_max_current_set = 10 ;
		cur->hy_purge_set = 10 ;
		cur->air_flow_rate_set = 25 ;
		cur->water_flow_rate_set = 25 ;
		break ;
	}
	case RE_CONN :{
		if(cur->output_current > 5){
			normal_control(max_power/4);
//			cur.output_voltage_set = cur.output_voltage ;
//			cur.output_max_current_set = cur.output_current ;
//			if(cur.output_voltage_set*cur.output_max_current_set < max_power *0.25){
//				cur.hy_purge_set = 20 ;
//				cur.air_flow_rate_set = 50 ;
//				cur.water_flow_rate_set = 50 ;
//			}
//			else if(cur.output_voltage_set*cur.output_max_current_set < max_power *0.5){
//				cur.hy_purge_set = 20 ;
//				cur.air_flow_rate_set = 75 ;
//				cur.water_flow_rate_set = 75 ;
//			}
//			else{
//				cur.hy_purge_set = 20 ;
//				cur.air_flow_rate_set = 100 ;
//				cur.water_flow_rate_set = 100 ;
//			}
		}
		else{
			cur->dc_on_set = false ;
			cur->output_voltage_set = MIN_OUTPUT_VOLTAGE ;
			cur->output_max_current_set = 10 ;
			cur->hy_purge_set = 10 ;
			cur->air_flow_rate_set = 25 ;
			cur->water_flow_rate_set = 25 ;
		}
		break ;
	}
	case LOW_POWER :{
		//half max power usage
		normal_control(max_power/2);
		break ;
	}
	case WARMUP:{
		normal_control(max_power/2);
		break ;
	}
	default:{
		cur->dc_on_set = false ;
		cur->output_voltage_set = MIN_OUTPUT_VOLTAGE ;
		cur->output_max_current_set = 10 ;
		cur->hy_purge_set = 10 ;
		cur->air_flow_rate_set = 25 ;
		cur->water_flow_rate_set = 25 ;
	}
	}
}

bool fc_algo_simple::if_lowpower(double voltage,double current){
	if(voltage < 123 - 0.175* current ){
		return true ;
	}
	else{
		return false ;
	}
}

bool fc_algo_simple::if_cold_system(double temperature){
	if(temperature < 0){
		return true;
	}
	else {
		return false ;
	}
}

void fc_algo_simple::normal_control(double i_max_power){
	fc_ref_data *cur = &his_data[cur_ptr] ;
	fc_ref_data *last ;
	if(cur_ptr == 0){
		last = &his_data[HIS_BUFFER_LEN-1];
	}
	else {
		last = &his_data[cur_ptr - 1];
	}
	cur->dc_on_set = true ;
	if(last->dc_on_set == false){
		//first set
		if(last->output_voltage > MIN_OUTPUT_VOLTAGE)
			cur->output_voltage_set = last->output_voltage ;
		else
			cur->output_voltage_set = MIN_OUTPUT_VOLTAGE ;
		cur->output_max_current_set = 10 ;
	}
	else if(cur->output_voltage * cur->output_current < i_max_power * 0.95){
		//incr power
		if(last->output_voltage_set >= MAX_OUTPUT_VOLTAGE){
			cur->output_voltage_set = MAX_OUTPUT_VOLTAGE ;
		}
		else if(cur->output_voltage > last->output_voltage_set*0.95){
			cur->output_voltage_set = last->output_voltage_set + 1 ;
		}
		else if(cur->output_voltage < last->output_voltage_set*0.9){
			cur->output_voltage_set = last->output_voltage_set - 1 ;
		}
		else {
			cur->output_voltage_set = last->output_voltage_set ;
		}
		if(cur->output_current < last->output_max_current_set * 0.95){
			cur->output_current = last->output_voltage_set*last->output_max_current_set/cur->output_voltage_set ;
		}
		else if(last->output_voltage_set*last->output_max_current_set*1.01
				< i_max_power){
			cur->output_max_current_set =
					last->output_voltage_set*last->output_max_current_set*1.01/cur->output_voltage_set ;
		}
		else {
			cur->output_max_current_set = i_max_power/cur->output_voltage_set ;
		}
	}
	else if(cur->output_voltage * cur->output_current > i_max_power * 1.05){
		if(last->output_voltage_set > MAX_OUTPUT_VOLTAGE){
			cur->output_voltage_set = MAX_OUTPUT_VOLTAGE ;
		}
		else {
			cur->output_voltage_set = last->output_voltage_set ;
		}
		cur->output_max_current_set = i_max_power / cur->output_voltage_set ;
	}
	else {
		cur->output_voltage_set = last->output_voltage_set ;
		cur->output_max_current_set = last->output_max_current_set ;
	}
	if(cur->output_voltage*cur->output_current < i_max_power *0.25){
		cur->hy_purge_set = 20 ;
		cur->air_flow_rate_set = 50 ;
		cur->water_flow_rate_set = 50 ;
	}
	else if(cur->output_voltage*cur->output_current < i_max_power *0.5){
		cur->hy_purge_set = 20 ;
		cur->air_flow_rate_set = 75 ;
		cur->water_flow_rate_set = 75 ;
	}
	else{
		cur->hy_purge_set = 20 ;
		cur->air_flow_rate_set = 100 ;
		cur->water_flow_rate_set = 100 ;
	}
}

fc_ref_data fc_algo_simple::get_cur_data(){
	return his_data[cur_ptr];
}

void fc_algo_simple::set_para(double max_v, double min_v, double stack_v, double max_p){
	this->MAX_OUTPUT_VOLTAGE = max_v ;
	this->MIN_OUTPUT_VOLTAGE = min_v ;
	this->STACK_OPEN_VOLTAGE = stack_v ;
	this->max_power = max_p ;
}
