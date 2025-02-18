#include <definitions.h>
#include <controller.h>

// Parametros del lazo de  control
const float h = 0.01;  // tiempo de muestreo en segundos
const float umin = -5; // valor mínimo de la señal de control
const float umax = 5; // valor máximo de la señal de control
const float deadZone = 0.3; // valor estimado de la zona muerta 

// variables del lazo de control
float reference; 
float y; // angulo a controlar
float u; //accion de control
float e; // error
float usat; // señal de control saturada




void setup() {
    // iniciamos los perifericos (encoder, led, pwm)
    setupPeripherals();
    
    // Asi definimos la tarea de control, de la máxima prioridad en el nucleo 1
    xTaskCreatePinnedToCore(
            controlTask, // nombre de la rutina
            "general controller task",
            8192,
            NULL,
            24, // prioridad de la tarea (0-24) , siendo 24 la prioridad más critica      
            NULL,
            CORE_1
    );  
    

    // ahora creamos una tarea para el menu de usuario
    xTaskCreatePinnedToCore(
        buttonTask, // nombre de la rutina
        "change reference with the buttons",
        8192,
        NULL,
        10,  // definimos una prioridad baja para esta tarea
        NULL,
        CORE_0 // la vamos a ejecutar en el CORE_0 que es comparte tareas con el procesador, baja prioridad
    );
    

    // ponemos el led en verde para indicar que el control está activo
    setLedColor(0, 255, 0);
}


/* *************************************************************************
*                     FUNCION CRITICA DE CONTROL
***************************************************************************/ 


static void controlTask(void *pvParameters) {

    // Aqui configuro cada cuanto se repite la tarea

    const TickType_t taskInterval = 1000*h;  // repetimos la tarea cada tiempo de muestreo en milisegundos = 1000*0.01= 100ms
    


    // prototipo de una tarea repetitiva   
    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount(); 
       
       // leemos el cambio en el encoder y lo sumamos a la referencia
       reference +=  encoderKnob.getCount() * pulsesTodegreesKnob;  
       encoderKnob.clearCount(); //reseteamos el encoder
   
       // leemos el valor actual del ángulo
       y = encoderMotor.getCount() * pulsesTodegreesMotor;
       
       //calculamos el error       
       e = reference - y; 

       // calculamos la accion de control 
       u = computeController(e, umin, umax);
   
       // realizamos la compensación de zona muerta
       u = compDeadZone(u, deadZone);

       // saturamos la señal de control para los limites energéticos disponibles entre umin=-5V y umax=5V
       usat = constrain(u, umin, umax);

       //enviamos la señal de control saturada al motor
       voltsToMotor(usat);       

       // ahora imprimimos para plotear al funcionamiento del controlador 
       printf(">Angulo:%.2f, Referencia:%0.2f, min:%d, max:%d \r\n", y, reference, -180, 180);  
      
       // la tarea es crítica entonces esperamos exactamente taskInterval ms antes de activarla nuevamente
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}



/*******************************************************************************/
// A continuacion creamos una tarea de menu de usuario


static void buttonTask(void *pvParameters) {
    
    for (;;) {
   
       // Si el boton con logo UN supera el umbral pongo un color naranja 
       if (touchRead(BUTTON_UN) > threshold){           
            
            // incrementamos  la referencia en 180°
            reference += 180;
            
            // cambiamos el color del led a azul por 1 segundo para indicar el cambio en la referencia
            setLedColor(0, 127, 255);
            vTaskDelay(1000);
            setLedColor(0, 255, 0);
            }

        // Si el boton con el dibujo de flor de la vida supera el umbral pongo un color azul claro 
       if (touchRead(BUTTON_LIFEFLOWER) > threshold){  
   
            // decrementamos  la referencia en 180°           
            reference -= 180;

            // cambiamos el color del led a rojo por 1 segundo para indicar el cambio en la referencia
            setLedColor(255, 0, 0);
            vTaskDelay(1000);
            setLedColor(0, 255, 0);
            }
       //   
       vTaskDelay(100); // espero 100 ms antes de leer nuevamente los botones
    }

}



void loop() {
    vTaskDelete(NULL);
}