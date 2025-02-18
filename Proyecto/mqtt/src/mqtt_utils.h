#ifndef MQTT_UTILS    // To make sure you don't declare the function more than once by including the header multiple times.
#define MQTT_UTILS

#include <Arduino.h>

String arrayToString(float * array, uint framesize);
char * float2Hex(float number);
char * long2Hex(unsigned long number);
float hex2Float(const char* string);

#endif