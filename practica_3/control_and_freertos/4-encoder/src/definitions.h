#include <Adafruit_NeoPixel.h>
#include <ESP32Encoder.h>




ESP32Encoder encoderMotor;  // variable encoder del motor
ESP32Encoder encoderKnob;  // variable encoder de la perilla


// definición de los nucleos en el esp32

#define CORE_0 0   // nucleo base usado por algunas tareas procesador
#define CORE_1 1   // nucleo para el usuario, poner aqui las tareas críticas



// Estas lineas configuran los leds
#define  LIGHT_PIN         19  
#define  NUMPIXELS          1
Adafruit_NeoPixel ledMotor(1, LIGHT_PIN, NEO_GRB + NEO_KHZ800); 


// Esta es la configuración del PWM
#define  FREQUENCY_PWM     100 // frecuencia del pwa
#define  RESOLUTION_PWM    12  // bits del pwm
#define  PIN_AIN1          2   // pin del pwm 1
#define  PIN_AIN2          1   
#define  CH_PWM_AIN1       0
#define  CH_PWM_AIN2       1

// factor de conversion de bits a voltios:  2^ #bits -1 / voltios
float  voltsToPwm =   (pow(2, RESOLUTION_PWM) - 1) / 5; 





// estas definiciones son para los botones
#define BUTTON_UN   14
#define BUTTON_LIFEFLOWER  7

uint16_t threshold  = 60000; // umbral de deteccion para los botones


// color base del led

uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;


// Aqui definimos los encoder

#define CH_ENC_A  18 // pines del encoder del motor
#define CH_ENC_B  17 // pines del encoder del motor

// Encoder de la perilla

#define CH_ENC_A_POT  11  //WEMOS 5// ESP32S3 11//
#define CH_ENC_B_POT  10 // WEMOS 23 // ESP32S3 10//


float pulsesTodegreesMotor = (float) (360.0/2800.0);  // el encoder del motor genera 2800 pulsos / vuelta
float pulsesTodegreesKnob = (float) (360.0/96.0); // el encoder de la perilla genera 96 pulsos / vuelta


void setupPeripherals(void){
    
    // iniciamos los encoder
    // encoder del motot
    encoderMotor.attachFullQuad (CH_ENC_A, CH_ENC_B);
    encoderMotor.setCount(0);

    // encoder de la perilla
    encoderKnob.attachFullQuad (CH_ENC_A_POT, CH_ENC_B_POT);
    encoderKnob.setCount(0);

    // para prender el led 
    ledMotor.setPixelColor(0, ledMotor.Color(0, 255, 0));
    ledMotor.setBrightness(20);
    ledMotor.show();
}
