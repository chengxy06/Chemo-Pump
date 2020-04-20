/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-28 dczhang
* Initial revision.
*
************************************************/
#include "alarm.h"
#include "timer.h"
#include "common.h"



/************************************************
* Variable 
************************************************/


//remind alarm list
static SszListNode g_remind_alarm_list_nodes[REMIND_ALARM_MAX_SIZE];
static SszList g_remind_alarm_list_inter;
static SszList* g_remind_alarm_list=&g_remind_alarm_list_inter;

//lowest alarm list
static SszListNode g_lowest_alarm_list_nodes[LOWEST_ALARM_MAX_SIZE];
static SszList g_lowest_alarm_list_inter;
static SszList* g_lowest_alarm_list=&g_lowest_alarm_list_inter;

//low alarm list
static SszListNode g_low_alarm_list_nodes[LOW_ALARM_MAX_SIZE];
static SszList g_low_alarm_list_inter;
static SszList* g_low_alarm_list=&g_low_alarm_list_inter;

//high alarm list
static SszListNode g_high_alarm_list_nodes[HIGH_ALARM_MAX_SIZE];
static SszList g_high_alarm_list_inter;
static SszList* g_high_alarm_list=&g_high_alarm_list_inter;

//highest alarm list
static SszListNode g_highest_alarm_list_nodes[HIGHEST_ALARM_MAX_SIZE];
static SszList g_highest_alarm_list_inter;
static SszList* g_highest_alarm_list=&g_highest_alarm_list_inter;


/************************************************
* Function 
************************************************/
void alarm_init()
{
    ssz_mem_zero(g_remind_alarm_list_nodes,ssz_array_size(g_remind_alarm_list_nodes));
    ssz_mem_zero(g_lowest_alarm_list_nodes,ssz_array_size(g_lowest_alarm_list_nodes));
    ssz_mem_zero(g_low_alarm_list_nodes,ssz_array_size(g_low_alarm_list_nodes));
    ssz_mem_zero(g_high_alarm_list_nodes,ssz_array_size(g_high_alarm_list_nodes));
    ssz_mem_zero(g_highest_alarm_list_nodes,ssz_array_size(g_highest_alarm_list_nodes));

    ssz_list_init(g_remind_alarm_list,g_remind_alarm_list_nodes,REMIND_ALARM_MAX_SIZE,sizeof(SszListNode));
    ssz_list_init(g_lowest_alarm_list,g_lowest_alarm_list_nodes,LOWEST_ALARM_MAX_SIZE,sizeof(SszListNode));
    ssz_list_init(g_low_alarm_list,g_low_alarm_list_nodes,LOW_ALARM_MAX_SIZE,sizeof(SszListNode));
    ssz_list_init(g_high_alarm_list,g_high_alarm_list_nodes,HIGH_ALARM_MAX_SIZE,sizeof(SszListNode));
    ssz_list_init(g_highest_alarm_list,g_highest_alarm_list_nodes,HIGHEST_ALARM_MAX_SIZE,sizeof(SszListNode));
}


AlarmLevel alarm_id_to_level(AlarmID e_alarm_id)
{
    if((e_alarm_id>kRemindAlarmStart) && (e_alarm_id < kRemindAlarmEnd)){
        return kAlarmLevelRemind;
    }
    else if(e_alarm_id>kLowestAlarmStart && e_alarm_id < kLowestAlarmEnd){
        return kAlarmLevelLowest;
    }
    else if(e_alarm_id>kLowAlarmStart && e_alarm_id < kLowAlarmEnd){
        return kAlarmLevelLow;
    }
    else if(e_alarm_id>kHighAlarmstart && e_alarm_id < kHighAlarmEnd){
        return kAlarmLevelHigh;
    }
    else if(e_alarm_id>kHighestAlarmStart && e_alarm_id < kHighestAlarmEnd){
        return kAlarmLevelHighest;
    }

    return kAlarmLevelNone;
}

#if 0
//小于返回-1，等于返回0，大于返回1
//参数为数据的地址
static int list_data_compare(const void* data1, const void* data2, size_t data_size){
	(void)data_size;
    //将无类型指针转换成有类型指针
    int* p1 = (int*) data1;
    int* p2 = (int*) data2;
	if ((*p1) < (*p2)) {
		return -1;
	}else if (*p1 == *p2) {
		return 0;
	}else{
		return 1;
	}
}
#endif

// 报警ID存在，则返回true
bool alarm_list_node_is_exist(AlarmID e_alarm_id, SszList* container)
{
	int pos;
	const void* compare_data = NULL;
    
	for (pos=ssz_list_begin(container); pos!=-1; ) {
		compare_data = ssz_list_at_pos(container, pos);
		if ((int)e_alarm_id == (*(int*)compare_data)) {
			return true;
		}
		pos = ssz_list_next(container, pos);
	}
    return false;
}

//返回节点地址
int alarm_node_pos(SszList* container, AlarmID e_alarm_id)
{
    int pos;
	const void* compare_data = NULL;
    
	for (pos=ssz_list_begin(container); pos!=-1; ) {
		compare_data = ssz_list_at_pos(container, pos);
		if ((int)e_alarm_id == (*(int*)compare_data)) {
			return pos;
		}
		pos = ssz_list_next(container, pos);
	}
    return -1;
}

//如果节点被设置则返回true,否则返回false
bool alarm_is_set(AlarmID e_alarm_id)
{
    AlarmLevel e_alarm_level;

    e_alarm_level = alarm_id_to_level(e_alarm_id);

    if(kAlarmLevelRemind == e_alarm_level){
        if(true == alarm_list_node_is_exist(e_alarm_id, g_remind_alarm_list))
            return true;
    }
    else if(kAlarmLevelLowest == e_alarm_level){
        if(true == alarm_list_node_is_exist(e_alarm_id, g_lowest_alarm_list))
            return true;
    }
    else if(kAlarmLevelLow == e_alarm_level){
        if(true == alarm_list_node_is_exist(e_alarm_id, g_low_alarm_list))
            return true;
    }
    else if(kAlarmLevelHigh == e_alarm_level){
        if(true == alarm_list_node_is_exist(e_alarm_id, g_high_alarm_list))
            return true;
    }
    else if(kAlarmLevelHighest == e_alarm_level){
        if(true == alarm_list_node_is_exist(e_alarm_id, g_highest_alarm_list))
            return true;
    }

    return false;
}

//保存报警ID并产生消息
void alarm_set(AlarmID e_alarm_id)
{
    AlarmLevel e_alarm_level;

    //return if alarm is set
    if(true == alarm_is_set(e_alarm_id))
        return;

    e_alarm_level = alarm_id_to_level(e_alarm_id);

    //将新节点插在链表的后面
    if(kAlarmLevelRemind == e_alarm_level){
        ssz_list_push_back(g_remind_alarm_list,&e_alarm_id);
    }
    else if(kAlarmLevelLowest == e_alarm_level){
        ssz_list_push_back(g_lowest_alarm_list,&e_alarm_id);
    }
    else if(kAlarmLevelLow == e_alarm_level){
        ssz_list_push_back(g_low_alarm_list,&e_alarm_id);
    }
    else if(kAlarmLevelHigh == e_alarm_level){
        ssz_list_push_back(g_high_alarm_list,&e_alarm_id);
    }
    else if(kAlarmLevelHighest == e_alarm_level){
        ssz_list_push_back(g_highest_alarm_list,&e_alarm_id);
    }
	common_printfln("set alarm %d",e_alarm_id);
    msg_post_int(kMsgAlarmSet,(int)e_alarm_id);
}

//清除报警信息并产生消息
void alarm_clear(AlarmID e_alarm_id)
{
    AlarmLevel e_alarm_level;
    int pos;
    
    //return if alarm not set
    if(false == alarm_is_set(e_alarm_id))
        return;

    e_alarm_level = alarm_id_to_level(e_alarm_id);

    if(kAlarmLevelRemind == e_alarm_level){
        pos = alarm_node_pos(g_remind_alarm_list, e_alarm_id);
        ssz_list_erase(g_remind_alarm_list, pos);        
    }
    else if(kAlarmLevelLowest == e_alarm_level){
        pos = alarm_node_pos(g_lowest_alarm_list, e_alarm_id);
        ssz_list_erase(g_lowest_alarm_list, pos); 
    }
    else if(kAlarmLevelLow == e_alarm_level){
        pos = alarm_node_pos(g_low_alarm_list, e_alarm_id);
        ssz_list_erase(g_low_alarm_list, pos); 
    }
    else if(kAlarmLevelHigh == e_alarm_level){
        pos = alarm_node_pos(g_high_alarm_list, e_alarm_id);
        ssz_list_erase(g_high_alarm_list, pos); 
    }
    else if(kAlarmLevelHighest == e_alarm_level){
        pos = alarm_node_pos(g_highest_alarm_list, e_alarm_id);
        ssz_list_erase(g_highest_alarm_list, pos); 
    }
	common_printfln("clear alarm %d", e_alarm_id);
    msg_post_int(kMsgAlarmClear,(int)e_alarm_id);
}

AlarmLevel alarm_highest_priority_level()
{

	if (false == ssz_list_is_empty(g_highest_alarm_list))
		return kAlarmLevelHighest;
	else if (false == ssz_list_is_empty(g_high_alarm_list))
		return kAlarmLevelHigh;
	else if (false == ssz_list_is_empty(g_low_alarm_list))
		return kAlarmLevelLow;
	else if (false == ssz_list_is_empty(g_lowest_alarm_list))
		return kAlarmLevelLowest;
	else if (false == ssz_list_is_empty(g_remind_alarm_list))
		return kAlarmLevelRemind;

    return kAlarmLevelNone;
}

//获取最高等级的最新报警ID
//lint -e{613}
AlarmID alarm_highest_priority_id()
{
    AlarmLevel e_alarm_level;
    AlarmID e_alarm_id;
    
    e_alarm_level = alarm_highest_priority_level();

    if(kAlarmLevelRemind == e_alarm_level){
        e_alarm_id = *(AlarmID*)ssz_list_back(g_remind_alarm_list);      
    }
    else if(kAlarmLevelLowest == e_alarm_level){
        e_alarm_id = *(AlarmID*)ssz_list_back(g_lowest_alarm_list);   
    }
    else if(kAlarmLevelLow == e_alarm_level){
        e_alarm_id = *(AlarmID*)ssz_list_back(g_low_alarm_list);  
    }
    else if(kAlarmLevelHigh == e_alarm_level){
        e_alarm_id = *(AlarmID*)ssz_list_back(g_high_alarm_list);  
    }
    else if(kAlarmLevelHighest == e_alarm_level){
        e_alarm_id = *(AlarmID*)ssz_list_back(g_highest_alarm_list);  
    }
    else{
        e_alarm_id = kAlarmNull;
    }

    return e_alarm_id;
}



