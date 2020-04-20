/************************************************
* DESCRIPTION:
*   used to handle communication
*
************************************************/
#include "com.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static ComReceiveHandler g_com_receive_handlers[kComIDMax][kComHandleMax] = {NULL};
static SszFile* g_com_files[kComIDMax];

/************************************************
* Function 
************************************************/


/***********************************************
* Description:
*   handle new received data
* Argument:
*
* Return:
*
************************************************/
void com_handle_new_received_data(ComID com_id)
{
    int buff_size = ssz_fsize(g_com_files[com_id]);
    
    if (buff_size>0)
    {
        int i;
        bool ret;
        uint8_t a;

        for(i=0; i<buff_size; i++)
        {
            //get the receive byte
            a = (uint8_t)ssz_fgetc(g_com_files[com_id]);
            ret = com_handle_new_byte(com_id, a);
			if(ssz_fsize(g_com_files[com_id])==0){
				//the receive data is clear by a com handle, so stop handle
				break;
			}
			
            if (ret == false)
            {
                ssz_traceln("com %d receive unknow byte[%x]\n", com_id, a);
            }
        }

    }
}

//handle one new received byte, return true if someone handled it
bool com_handle_new_byte( ComID com_id, uint8_t ch)
{
    int j;
    bool ret = false;

    for(j=0; j<kComHandleMax; j++)
    {
        //let each handler to handle it    
        if (g_com_receive_handlers[com_id][j])
        {
            if(ret == false)
            {
                //previous handler not know it and not handle it
                ret = g_com_receive_handlers[com_id][j](ch, false);
            }
            else
            {
                //previous handler already know it and handle it
                g_com_receive_handlers[com_id][j](ch, true);
            }
        }    
	}

    return ret;
}

/***********************************************
* Description:
*   bind receive handler
* Argument:
*   order: 
*   handler:
*
* Return:
*
************************************************/
void com_set_receive_handler(ComID com_id, ComHandleOrder order, ComReceiveHandler handler)
{
    ssz_assert(order<kComHandleMax);
    g_com_receive_handlers[com_id][order] = handler;
}

SszFile* com_to_file( ComID com_id){
    return g_com_files[com_id];
}
void com_init_by_file(ComID com_id, SszFile* file)
{
    g_com_files[com_id] = file;
}