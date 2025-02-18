// libreria para el control de los leds
#include <Adafruit_NeoPixel.h>


// definición de los nucleos en el esp32
#define CORE_0 0   // nucleo base usado por algunas tareas procesador
#define CORE_1 1   // nucleo para el usuario, poner aqui las tareas críticas


// Estas lineas configuran los leds
#define  LIGHT_PIN         19  // pin del led
#define  NUMPIXELS          1  // solo hay un led
Adafruit_NeoPixel ledMotor(NUMPIXELS, LIGHT_PIN, NEO_GRB + NEO_KHZ800); 


// estas lineas son para configurar los botones
#define BUTTON_UN   14         // el boton de la un está en el pin 14
#define BUTTON_LIFEFLOWER  7   // el boton de la flor de la vida está en el pin 14
uint16_t threshold  = 60000;   // el umbral de lectura del sensor de contacto > 60000 es seguro que el usuario tocó



// color base del led

uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;

// cambian de color el led de rojo a verde por con el cambio del flag
void  setLedColor(uint8_t r, uint8_t g, uint8_t b){
    ledMotor.setPixelColor(0, ledMotor.Color(r, g, b));  
    ledMotor.setBrightness(30);
    ledMotor.show();
}


