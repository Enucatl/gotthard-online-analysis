/*
 * configurations.h
 *
 *  Created on: Jan 9, 2013
 *      Author: l_cartier
 */

#ifndef CONFIGURATIONS_H_
#define CONFIGURATIONS_H_


#ifdef TOMCAT
#warning "Threshold and other configuration for tomcat"
#define CONFIG "tomcat"

#define FRAME_NOISE_THR 20000.0
#define OP_AMP_CORR 0.07
#define NOISE_THR 100.0
#define CP_SUM_THR 300.0
#define CP_SUM_THR_UP 500.0

#elif CSAX
#warning "Threshold and other configuration for CSAX"
#define CONFIG "csax"

#define FRAME_NOISE_THR 40000.0
#define OP_AMP_CORR 0.08
#define NOISE_THR 100.0
#define CP_SUM_THR 420.0
#define CP_SUM_THR_UP 560.0

#elif APS
#warning "Threshold and other configuration for APS"
#define CONFIG "aps"

#define FRAME_NOISE_THR 40000.0
#define OP_AMP_CORR 0.07
#define NOISE_THR 60.0
#define CP_SUM_THR 300.0
#define CP_SUM_THR_UP 500.0

#else
#warning "No specific thresold and configuration"
#define CONFIG "no specific"

#define FRAME_NOISE_THR 40000.0
#define OP_AMP_CORR 0.08
#define NOISE_THR 500.0
#define CP_SUM_THR 500.0
#define CP_SUM_THR_UP 600.0


#endif



#endif /* CONFIGURATIONS_H_ */
