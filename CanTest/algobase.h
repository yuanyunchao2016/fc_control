/*
 * algobase.h
 *
 *  Created on: 2017-9-14
 *      Author: yuanyunchao
 */

#ifndef ALGOBASE_H_
#define ALGOBASE_H_

class algo_base {
public:
	algo_base();
	virtual ~algo_base();

	virtual void calculate(double time_cnt);
};

#endif /* ALGOBASE_H_ */
