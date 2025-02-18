#include "mqtt_utils.h"

String arrayToString(float * array, uint framesize){
    String arrayHex;
    for (uint i = 0; i < framesize; i++ ){
        arrayHex.concat(float2Hex(array[i]));
        if (i < framesize-1){
            arrayHex.concat(',');
        }
    }
    return arrayHex;
}



char * float2Hex(float number){
    // This function converts a floating-point number to a 4-digit hexadecimal string. Consequently,
    // it enables the transmission of an exact float number with the minimum byte usage through MQTT.
    static char str[9];
    unsigned long ui;
    memcpy(&ui, &number, sizeof (ui));
    sprintf(str, "%08x", ui);
    return str;
}


char * long2Hex(unsigned long number){
    // This function converts a long integer number to a 4-digit hexadecimal string. Consequently,
    // it enables the transmission of a big number with the minimum byte usage through MQTT.
    static char str[9];
    unsigned long ui;
    memcpy(&ui, &number, sizeof (ui));
    sprintf(str, "%08x", ui);
    return str;
}

float hex2Float(const char* string){
    // This function converts a 4-digit hexadecimal string received from MQTT to a floating-point number.
    // It enables the reception of precise float numbers without rounding, minimizing the transmitted byte count.
    unsigned long temp;
    sscanf(string, "%8x", &temp);
    float f = *(float *)&temp;
    return f;
}