/*
 * controller_data_type.h
 *
 *  Created on: 2017-9-11
 *      Author: yuanyunchao
 */

#ifndef CONTROLLER_DATA_TYPE_H_
#define CONTROLLER_DATA_TYPE_H_

#include <stddef.h>

#define DEBUG

enum dc_type{
	fenergy,
	vapel,
	raycc
};

struct dcdc_status{
	bool dc_on_off ;
	bool set_on = 0 ;
	double dc_input_voltage;
	double dc_input_current;
	double dc_output_voltage;
	double dc_output_current;
	int dc_temperature ;
	double v_set = 500 ;
	double max_current = 30 ;
	int max_power = 15000 ;
	int power_inc_rate = 60 ;
	int power_dec_rate = 30 ;
	bool over_temperature;
	bool output_voltage_over_flow ;
	bool output_voltage_under_flow ;
	bool input_voltage_over_flow ;
	bool input_voltage_under_flow ;
	bool output_current_over_flow ;
	bool communication_error ;
	bool output_short ;
	bool auto_shutdown ;
};

struct hydrogen_sys_status{
	double tank_pressure = 30000 ;
	int tank_volumn = 140 ;
	double hy_pressure ;
	double hy_flow_rate ;
	double hy_temperature ;
	int purge_interval = 20 ;
};

struct air_sys_status{
	double air_pressure ;
	double air_flow_rate ;
	double air_temperature ;
	double air_humidity;
	double valve_rate = 100 ;
};

struct cooler_sys_status{
	double inlet_temperature ;
	double outlet_temperature ;
	double water_flow_rate ;
	double fan_rate = 50 ;
};

struct stack_sys_status{
	double stack_temperature ;
	double output_voltage ;
	double output_current ;
};

struct control_target{
	double stack_power = 30000 ;
	int outlet_temperature = 80 ;
};

enum running_stage{
	IDLE,
	STARTUP,
	STABLE,
	ADJUST,
	SHUTDOWN
};

struct system_status{
	dcdc_status dc_status ;
	hydrogen_sys_status hy_status ;
	air_sys_status air_status ;
	cooler_sys_status cooler_status ;
	stack_sys_status stack_status ;
	control_target c_target ;
	running_stage stage;
} ;



#endif /* CONTROLLER_DATA_TYPE_H_ */
