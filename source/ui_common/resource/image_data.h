#pragma once
#include "ssz_def.h"

/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//string id define
#include "generated_image_id.h"
#include "ui.h"

//get image from the image id
const Image* get_image(ImgID img_id);




#ifdef __cplusplus
}
#endif

