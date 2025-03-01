// libreria para el control de los leds
#include <Adafruit_NeoPixel.h>

// libreria dle encoder
#include <ESP32Encoder.h>


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


// Aqui definimos el encoder del motor

#define CH_ENC_A  18  // CH_A del encoder del motor
#define CH_ENC_B  17  // CH_B del encoder del motor

// Encoder de la perilla

#define CH_ENC_A_POT  11  // CH_A del encoder de la perilla
#define CH_ENC_B_POT  10  // CH_B del encoder de la perilla



// factores de conversion de pulsos a grados

// Variables del encoder
ESP32Encoder encoderMotor;  // variable encoder del motor
ESP32Encoder encoderKnob;  // variable encoder de la perilla
float pulsesTodegreesMotor = (float) (360.0/2800.0);  // el encoder del motor genera 2800 pulsos / vuelta
float pulsesTodegreesKnob = (float) (360.0/96.0); // el encoder de la perilla genera 96 pulsos / vuelta



// Esta funcion es para enviar un valor en voltios al motor por medio de un driver de puente h

void voltsToMotor( float volts){
    // This function convert a voltage value given in the variable volts
    // to a bipolar pwm signal for controlling the DC motor
    
    uint16_t pwm = abs(volts) * voltsToPwm;

    if (volts < 0){
        // if var volts is negative use CH_PWM_AIN2 to output a pwm signal
        // proportional to the input voltage
        ledcWrite(CH_PWM_AIN1, 0);
        ledcWrite(CH_PWM_AIN2, pwm);
    }
    else{
        // if var volts is negative use CH_PWM_AIN1 to output a pwm signal
        // proportional to the input voltage
        ledcWrite(CH_PWM_AIN1, pwm);
        ledcWrite(CH_PWM_AIN2, 0);
    }
}


void setupPeripherals(void){
    // iniciamos los modulos de pwm   
    ledcSetup(CH_PWM_AIN1  ,  FREQUENCY_PWM, RESOLUTION_PWM);
    ledcSetup(CH_PWM_AIN2  ,  FREQUENCY_PWM, RESOLUTION_PWM);
    ledcAttachPin(PIN_AIN1 , CH_PWM_AIN1);
    ledcAttachPin(PIN_AIN2 , CH_PWM_AIN2);
    
   
    // iniciamos los encoder
    encoderMotor.attachFullQuad (CH_ENC_A, CH_ENC_B);    // encoder del motot
    encoderMotor.setCount(0);

    encoderKnob.attachFullQuad (CH_ENC_A_POT, CH_ENC_B_POT);   // encoder de la perilla
    encoderKnob.setCount(0);   

    // para prender el led
    ledMotor.setPixelColor(0, ledMotor.Color(0, 255, 0));
    ledMotor.setBrightness(20);
    ledMotor.show();
}



float compDeadZone(float var, float dz){
    // This function compensates the dead zone of DC motor
    if (var == 0){
        return 0;
    }
    else {
        float sgnvar = abs(var)/var;
        return var + sgnvar * dz ;
    }
}


// cambian de color el led de rojo a verde por con el cambio del flag
void setLedColor(uint8_t r, uint8_t g, uint8_t b){
    ledMotor.setPixelColor(0, ledMotor.Color(r, g, b));  
    ledMotor.setBrightness(20);
    ledMotor.show();
}
