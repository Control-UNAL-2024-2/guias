#include <definitions.h>


// Variables del lazo de  control

const float h = 0.01;  // tiempo de muestreo en segundos
const float umin = -5; // valor mínimo de la señal de control
const float umax = 5; // valor máximo de la señal de control
const float deadZone = 0.3; // valor estimado de la zona muerta 


// variables del lazo de control    
float kp = 0.01;  //constante proporcional
float reference; 
float angle; // angulo a controlar
float y; // variable a controlar, en este caso velocidad angular
float u; //accion de control
float e; // error
float usat; // señal de control saturada


// manejadores de las tareas para poder  activarlas o desactivarlas

TaskHandle_t h_angleControlTask;
TaskHandle_t h_speedControlTask;


void setup() {
    // iniciamos los perifericos (encoder, led, pwm)
    setupPeripherals();
    
    // Asi definimos la tarea de control, de la máxima prioridad en el nucleo 1
    xTaskCreatePinnedToCore(
        angleControlTask, // nombre de la rutina
        "simple proportional controller",
        8192,
        NULL,
        23, // prioridad de la tarea (0-24) , siendo 24 la prioridad más critica      
        &h_angleControlTask,
        CORE_1
    );  
     

    // Asi definimos la tarea de control, de la máxima prioridad en el nucleo 1
    xTaskCreatePinnedToCore(
        speedControlTask, // nombre de la rutina
        "simple proportional controller",
        8192,
        NULL,
        23, // prioridad de la tarea (0-24) , siendo 24 la prioridad más critica      
        &h_speedControlTask,
        CORE_1
    );  

    // ponemos el led en naranja para indicar que la tarea de control de angulo está activa
    setLedColor(255, 64, 0); 

    // con esta linea detenemos la tarea de control de velocidad para solo iniciar con la de angulo
    vTaskSuspend(h_speedControlTask);

    // ahora creamos una tarea para el menu de usuario
    xTaskCreatePinnedToCore(
        buttonTask, // nombre de la rutina
        "activate motor with the buttons",
        8192,
        NULL,
        10,  // definimos una prioridad baja para esta tarea
        NULL,
        CORE_0 // la vamos a ejecutar en el CORE_0 que es comparte tareas con el procesador, baja prioridad
    );
  

}


/* *************************************************************************
*                     TAREAS CRÍTICAS DE CONTROL 
*
*                          control de angulo
***************************************************************************/ 

static void angleControlTask(void *pvParameters) {

    // Aqui configuro cada cuanto se repite la tarea

    const TickType_t taskInterval = 1000*h;  // repetimos la tarea cada tiempo de muestreo en milisegundos = 1000*0.01= 100ms
    


    // prototipo de una tarea repetitiva   
    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount(); 
       
       // leemos el valor actual de la referencia
       reference =  encoderKnob.getCount() * pulsesTodegreesKnob;
       
       // leemos el valor actual del ángulo
       y =  encoderMotor.getCount() * pulsesTodegreesMotor;
       
       //calculamos el error       
       e = reference - y; 

       // calculamos la accion de control proporcional
       u = kp * e; 

       // realizamos la compensacion de zona muerta del motor u = u + deadzone * sign(u)  
       u = compDeadZone(u, deadZone);

       // saturamos la señal de control para los limites energéticos disponibles entre -5V y 5V
       usat = constrain(u, -5, 5);

       //enviamos la señal de control saturada al motor
       voltsToMotor(usat);     


       // ahora imprimimos para plotear al funcionamiento del controlador 
       printf(">Referencia:%0.2f, Angulo:%.2f, min:%d, max:%d \r\n", reference, y, -180, 180);  
      
       // la tarea es crítica entonces esperamos exactamente taskInterval ms antes de activarla nuevamente
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}

// tarea de control de velocidad

static void speedControlTask(void *pvParameters) {

    // Aqui configuro cada cuanto se repite la tarea

    const TickType_t taskInterval = 1000*h;  // repetimos la tarea cada tiempo de muestreo en milisegundos = 1000*0.01= 100ms
    



    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount(); 
       
       // leemos el valor actual de la referencia
       reference =  encoderKnob.getCount() * pulsesTodegreesKnob;

       //para encontrar la velocidad  dividimos los pulsos que se hicieron por el tiempo de muestreo        
       y = encoderMotor.getCount() * pulsesTodegreesMotor / h;
       // reiniciamos la cuenta de pulsos para medir cuantos ocurrieron solo en este ciclo de muestreo
       encoderMotor.clearCount(); 
       
       //calculamos el error       
       e = reference - y; 

       // calculamos la accion de control proporcional
       u = kp * e; 

       // saturamos la señal de control para los limites energéticos disponibles entre -5V y 5V
       usat = constrain(u, umin, umax);

       //enviamos la señal de control saturada al motor
       voltsToMotor(usat);     

       // ahora imprimimos para plotear al funcionamiento del controlador 
       printf(">Referencia:%0.2f, Velocidad:%.2f,  min:%d, max:%d \r\n", reference, y,  -700, 700);  
      
       // la tarea es crítica entonces esperamos exactamente taskInterval ms antes de activarla nuevamente
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}


/******************************************************************************
*           TAREA DE BAJA PRIORIDAD - BOTONES
*
/***************************************************************************** */



static void buttonTask(void *pvParameters) {
    
    for (;;) {
   
       // Si el boton con logo UN supera el umbral pongo un color naranja 
       if (touchRead(BUTTON_UN) > threshold){
            // suspendemos la tarea de control de velocidad
            vTaskSuspend(h_angleControlTask);

            // reiniciamos los encoder para que no haya una transición brusca
            encoderMotor.clearCount();   
            encoderKnob.clearCount();                        
                       
            // ponemos el led en naranja para indicar control de velocidad
            setLedColor(0, 255, 0); 
           
            // activamos la tarea de control de velocidad
            vTaskResume(h_speedControlTask);   
           
            }

        // Si el boton con el dibujo de flor de la vida supera el umbral pongo un color azul claro 
       if (touchRead(BUTTON_LIFEFLOWER) > threshold){  
            // suspendemos la tarea de control de angulo
            vTaskSuspend(h_speedControlTask);

            // reiniciamos los encoder para que no haya una transición brusca
            encoderMotor.clearCount();  
            encoderKnob.clearCount();           
            
            // ponemos el led en naranja para indicar control de velocidad
            setLedColor(255, 64, 0); 

            vTaskResume(h_angleControlTask);
            }
       //   
       vTaskDelay(100); // espero 100 ms antes de repetir la tarea
    }

}


void loop() {
    vTaskDelete(NULL);
}