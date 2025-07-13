#ifndef _VIDEO_DISPLAY_H_
#define _VIDEO_DISPLAY_H_

#include <libavutil/imgutils.h>
int video_display_image(uint8_t **data, int *linesize, uint32_t format ,void *user_data);	//(uint8_t *data[AV_NUM_DATA_POINTERS], int linesize[AV_NUM_DATA_POINTERS], uint32_t format ,void *user_data);







#endif