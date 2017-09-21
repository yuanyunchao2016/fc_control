/*
 * controlrunner.h
 *
 *  Created on: 2017-9-19
 *      Author: yuanyunchao
 */

#ifndef CONTROLRUNNER_H_
#define CONTROLRUNNER_H_

class control_runner {
public:
	control_runner();
	virtual ~control_runner();

	virtual void start();
	virtual void stop();
};

#endif /* CONTROLRUNNER_H_ */
