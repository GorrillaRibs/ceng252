#include <MQTTClient.h>

#ifndef LOGGERMQTT_H
#define LOGGERMQTT_H

#define ADDRESS "tcp://localhost:1883"
#define CLIENTID "VehicleDataLogger"
#define TOPIC "Logger Data"
#define QOS 1
#define TIMEOUT 10000L

int DlPublishLoggerData(const char * mqttdata);

#endif
