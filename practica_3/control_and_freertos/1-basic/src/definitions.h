
// libreria para el control de los leds
#include <Adafruit_NeoPixel.h>

// definición de los nucleos en el esp32



#define CORE_0 0   // nucleo base usado por algunas tareas procesador
#define CORE_1 1   // nucleo para el usuario, poner aqui las tareas críticas



// Estas lineas configuran los leds
#define  LIGHT_PIN         19  
#define  NUMPIXELS          1

// cargamos el objeto para manejar el led
Adafruit_NeoPixel ledMotor(1, LIGHT_PIN, NEO_GRB + NEO_KHZ800); 

// color base del led

uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;






// cambian de color el led de rojo a verde por con el cambio del flag
void setLedColor(uint8_t r, uint8_t g, uint8_t b){
    ledMotor.setPixelColor(0, ledMotor.Color(r, g, b));  
    ledMotor.setBrightness(20);
    ledMotor.show();
}