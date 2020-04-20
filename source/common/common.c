#include "common.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static ModuleInfo g_module_output_info[kModuleIDMax] = {
	{COMMON_MODULE_NAME,false},
	{MOTOR_MODULE_NAME,false},
	{MOTOR_MONITOR_MODULE_NAME,false},
	{INFUSION_MONITOR_MODULE_NAME,false},
	{SCREEN_MODULE_NAME,false},
};

/************************************************
* Function 
************************************************/

//set the module output enable/disable
void set_module_output(ModuleID module, bool is_enable) {
	g_module_output_info[module].is_enable_output = is_enable;
}
//return is the module enable output
bool is_module_enable_output(ModuleID module) {
	return g_module_output_info[module].is_enable_output;
}

//return module name
const char* get_module_name(ModuleID module) {
	return g_module_output_info[module].module_name;
}
