/**
******************************************************************************
* @file    CSysTick.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-1-27
* @brief   This file provides all the SysTick functions.
******************************************************************************/
#include "CSysTick.h"
#include "stddef.h"

//const uint16_t CSysTick::taskPeriodOverflowNbr[CSysTick::NUM_OF_TASK_PERIOD] = 
//{1, 2, 5, 10, 20, 50, 100, 200, 5000, 10000};
const uint16_t CSysTick::taskPeriodOverflowNbr[CSysTick::NUM_OF_TASK_PERIOD] = 
{0, 0, 0, 1, 2, 5, 10, 20, 500, 1000};
uint16_t CSysTick::taskPeriodCounter[CSysTick::NUM_OF_TASK_PERIOD];
uint16_t CSysTick::missUpCounter_ = 0;
uint32_t CSysTick::absoluteTime_ = 0;
//
//constructor
//
CSysTick::CSysTick()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //9MHz
	stop();
	reset();
	SetFreq(10);	//default frequence 1kHz
}

//
//void CSysTick::SetFreq(uint32_t freq)
//
void CSysTick::SetFreq(uint32_t freq)
{
	if( 9000000/freq > 0xFFFFFFul)
	{
		while(1);//error
	}	
	else
	{
		SysTick->LOAD = 9000000/freq;
	}
}

//
//uint32_t CSysTick::get_Freq()
//
uint32_t CSysTick::get_Freq()
{
	uint32_t freq = 9000000/SysTick->LOAD;
	return freq;
}

/**
  * @brief  get the system absolute run time
  * @param  None
  * @retval absoluteTime_
  */
uint32_t CSysTick::getAbsoluteTime()
{
	return absoluteTime_;
}

/**
  * @brief  constructor
  * @param  timerPeriod
  * @retval None
  */
CTickTimer::CTickTimer(uint32_t timerPeriod)
	:timerPeriod_(timerPeriod)	
{}
	
/**
  * @brief  reset and start the timer
  * @param  None
  * @retval None
  */
void CTickTimer::restart()
{
	absoluteStartTime_ = sysTick::Instance()->getAbsoluteTime();
}

/**
  * @brief  reset and start the timer
  * @param  None
  * @retval None
  */
bool CTickTimer::isTimeUp()
{
	uint32_t absoluteTime = sysTick::Instance()->getAbsoluteTime();
	uint32_t targetTime = absoluteStartTime_ + timerPeriod_;
	if(targetTime > absoluteStartTime_)
	{
		if((absoluteTime >= absoluteStartTime_) && (absoluteTime < targetTime)) 
			return false;
		else 
			return true;
	}else //overflow
	{
		if((absoluteTime >= targetTime) && (absoluteTime < absoluteStartTime_))
			return true;
		else
			return false;
	}
}
//end of file
