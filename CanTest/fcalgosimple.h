/*
 * fcalgosimple.h
 *
 *  Created on: 2017-9-14
 *      Author: yuanyunchao
 */

#ifndef FCALGOSIMPLE_H_
#define FCALGOSIMPLE_H_

#include "algobase.h"
#include "controller_data_type.h"
#ifdef DEBUG
#include <iostream>
#include <stdio.h>
#include <string>
#endif

#define HIS_BUFFER_LEN 100

enum fc_defection{
	NORMAL,
	NEW_START,
	RE_CONN,
	WARMUP,
	LOW_POWER,
	HY_STARVE,
	AIR_STARVE,
	HIGH_DC_TEMP,
	HIGH_STACK_TEMP
};

struct fc_ref_data{
	double time_counter = 0 ;

	//double max_power = 30000 ;

	double stack_voltage ;
	double stack_current ;
	double output_voltage ;
	double output_current ;
	double dc_temperature ;

	bool dc_on_set = false;
	double output_voltage_set = 300 ;
	double output_max_current_set = 10 ;
	int air_flow_rate_set = 25 ;
	int hy_purge_set = 20 ;
	int water_flow_rate_set = 50 ;

	fc_defection defect = NEW_START ;
};

extern system_status global_status ;

class fc_algo_simple: public algo_base {
private:
	fc_ref_data *his_data ;
	int cur_ptr,loop_cnt ;

	double MAX_OUTPUT_VOLTAGE = 500 ;
	double MIN_OUTPUT_VOLTAGE = 460 ;
	double STACK_OPEN_VOLTAGE = 140 ;
	double max_power = 30000 ;

	void refresh_history();
	void defection_diagnose();
	void output_implement();
	bool if_lowpower(double voltage,double current);
	bool if_cold_system(double temperature);
	void normal_control(double max_power);
public:
	fc_algo_simple();
	virtual ~fc_algo_simple();

	virtual void calculate(double time_cnt);
	fc_ref_data get_cur_data();
	void set_para(double max_v, double min_v, double stack_v, double max_p);
};

#endif /* FCALGOSIMPLE_H_ */
