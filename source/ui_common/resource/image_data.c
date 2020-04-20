/************************************************
*
* NAME: image_data.c
*
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-3-2 ryan
* Initial revision.
*
************************************************/
#include "image_data.h"
#include "string.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/
#include "generated_image_data.h"

/************************************************
* Function 
************************************************/

//get image from the image id
const Image* get_image(ImgID img_id)
{
    if (img_id >= kImgIDMax)
    {
        return NULL;
    }
    return &g_image_data[img_id];
}


