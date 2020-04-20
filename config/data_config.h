#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


#define SERIAL_NUMBER_LEN 12


//used to read this to check if not init or version upgrade
//if not init or version upgrade, write default data to nvram
typedef struct {
	int8_t init_flag;//the flag indicate if the data is init
	int8_t version;//if the version is not same, it will reinit the data
}DataInitInfo;

//used to clear the block data which is the type, 
// e.g. clear user data will erase the block data which's type is User
typedef enum {
	kDataTypeFCT,
	kDataTypeFactory,
	kDataTypeUser,
	kDataTypeOther,
	kDataTypeMax,
}DataType;


//Notice: first define block, then define the block's childes
//Cause it use this rule to find which id is at which block 
// you can read or write by block or child id
//e.g.  
// struct XXX{    <- XXX is a block
//	int id;			<- id the child of the block
//	char name[20];   <-	name is the child of the block
// };
// XXX a;
// data_read(XXX, &a, sizeof(a));    <- can read block
// data_read(id, &a.id, sizeof(a.id));  <- can read block's child(must use the block cache first)
// data_read(id, &a.name, sizeof(a.name)); <- can read block's child(must use the block cache first)
typedef enum {
	//FCT, Factory, User data init info, used to init default data if not init
	kDataFCTInitBlock, //DataInitInfo
	kDataFactoryInitBlock, //DataInitInfo
	kDataUserInitBlock, //DataInitInfo

	//FCT data block
	kDataFCTBlock, 
	kDataBatteryRedress,  //unit: ad
	kDataPressureRedress, //unit: ad

	//Factory data block
	kDataFactoryBlock,
	kDataSerialNumber,
	kDataHardwareVersion,
	kDataBatteryTwoThreshold, //unit: mV
	kDataBatteryOneThreshold, //unit: mV
	kDataBatteryLowThreshold, //unit: mV
	kDataBatteryExhaustThreshold, //unit: mV
	kDataOcclusionThreshold, //unit:ad
	kDataEncoderCountEachML, //encoder count of one ml
	kDataPassword,           //password to unlock parameter setting
	kDataBubbleAlarmSwitch,
	kDataOcclusionSlope, //zoom 1000, this is a, pressure=a*(current_adc-start_adc)
	kDataOcclusionIncreaseOfOneDrop, //一滴增加的adc值
    kDataPressureThreshold,
    kDataPrecisionFactor,

    //User data block
	kDataUserBlock,
	kDataLastShutdownIsAbnormal,
 	KDataVoiceVolume,
 	kDataBrightness,
	kDataPeekModeSwitch,
	kDataInfusionModeSelection,
	//kDataOcclusionLevel,
	kDataRunLifetimeCount,
	kDataMaxCleanTubeDose,

	//infusion block
	kDataInfusionInfoBlock,

	//set infusion para block
	kDataSetInfusionParaInfoBlock,


	kDataIDMax,
}DataID;


#ifdef __cplusplus
}
#endif




