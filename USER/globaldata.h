#ifndef __GLOBALDATA_H__
#define __GLOBALDATA_H__
#include "stdint.h"

typedef struct
{
	unsigned Do00	:1;
	unsigned Do01	:1;
	unsigned Do02	:1;
	unsigned Do03	:1;
	unsigned Do04	:1;
	unsigned Do05	:1;
	unsigned Do06	:1;
	unsigned Do07	:1;
	unsigned			:1;					//核心板
	unsigned			:1;					//核心板
	unsigned Do10 :1;
	unsigned Do11 :1;
	unsigned Do12 :1;
	unsigned Do13 :1;
	unsigned Do14 :1;
	unsigned Do15 :1;
}DO_StrutctTypedef;

typedef union
{
	DO_StrutctTypedef DO_DataBit;
	uint32_t DO_Data;
}DO_DataTypedef;

typedef struct
{
	DO_DataTypedef DoData;
	DO_DataTypedef is_protected;
}DO_Typedef;

extern DO_Typedef DoStatus;

#endif

