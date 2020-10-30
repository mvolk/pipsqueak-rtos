#ifndef PSQ4_UI_H
#define PSQ4_UI_H

#include <freertos/FreeRTOS.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    size_t max_trans_size;
} psq4_ui_params_t;


void psq4_ui_task(void * pvParameters);


#ifdef __cplusplus
}
#endif

#endif // PSQ4_UI_H
