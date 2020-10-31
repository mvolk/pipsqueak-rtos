#ifndef PSQ4_AWS_IOT_H
#define PSQ4_AWS_IOT_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <aws_iot_mqtt_client_interface.h>

#ifdef __cplusplus
extern "C" {
#endif


void psq4_aws_iot_init();


// Note that this blocks until the subscription is created
void psq4_mqtt_subscribe(
    const char *topic,
    enum QoS qos,
    pApplicationHandler_t handler,
    EventBits_t initializingEventBit,
    EventBits_t connectedEventBit
);


// Note that this blocks until the message is published
void psq4_mqtt_publish(
    const char *topic,
    enum QoS qos,
    const char *payload
);


#ifdef __cplusplus
}
#endif

#endif // PSQ4_AWS_IOT_H
