#ifndef __COLOR_ARRAY_H__
#define __COLOR_ARRAY_H__

#include <string.h>
#include "stm32f10x.h"

#define COLOR_BASE	3	
#define PWM_HIGH		60
#define PWM_LOW			30
//light num
#define NUM_LED		((uint16_t)14)
#define DMA_SIZE (u16)(24*NUM_LED+2)
#define STEP_NUM 255
extern unsigned char SinTab[STEP_NUM + 1];
extern unsigned char DMA_color_array[NUM_LED*COLOR_BASE];
extern unsigned char DMA_bin_array[NUM_LED*COLOR_BASE * 8+2];
enum emode
{
	powerOn = 0,
	battery = 1,
	alert = 2,
	specified = 3
};

class ColorArray
{
public:
	static ColorArray* Instance()  //singleton
	{
		if (m_pInstance == NULL)
			m_pInstance = new ColorArray();
		return m_pInstance;
	}
//	void set_array(unsigned char t[]);	//mostly not used
	void set_normal_command(emode t, int max, int param, int r, int g, int b);

	unsigned char* p_bin_array;
	unsigned char* p_color_array;
	
private:
//	unsigned char m_color_array[NUM_LED*COLOR_BASE];
//	unsigned char m_bin_array[NUM_LED*COLOR_BASE*8];
	int m_len;
		
	void update();
	void cal_power_on(int maxIntensity);
	void cal_alert(int maxIntensity);
	void cal_battery_display(int param, int max);	//0~100

	void set_color_array(int num, int r = 0, int g = 0, int b = 0); //single direction to all LED
	void judge(int& counter, int& direction, int maxIntensity);

	ColorArray()
	{
		m_len = NUM_LED*COLOR_BASE;
		p_color_array = DMA_color_array;
		p_bin_array = DMA_bin_array+1;
		memset(DMA_color_array, 0, m_len);
		memset(DMA_bin_array, PWM_LOW, m_len * 8);
		*(p_bin_array - 1) = 30;
		*(p_bin_array + m_len * 8) = 0;
	}

	~ColorArray(){};
		
	//singleton
	static ColorArray *m_pInstance;
};

class CSingleton  
{  
private:  
    CSingleton()   //构造函数是私有的  
    {  
    }
		//void Test();
    static CSingleton *m_pInstance;  
public:  
    static CSingleton * GetInstance()  
    {  
        if(m_pInstance == NULL)  //判断是否第一次调用  
            m_pInstance = new CSingleton();  
        return m_pInstance;  
    }  
		void Test();
};

#endif 
