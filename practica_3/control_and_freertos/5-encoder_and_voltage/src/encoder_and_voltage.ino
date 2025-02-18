
#include <definitions.h>


void setup() {
    // iniciamos los perifericos
    setupPeripherals();

    // Asi definimos una tarea para el sistema operativo
    xTaskCreatePinnedToCore(
            sendVoltsFromKnob, // nombre de la rutina
            "reads the encoder",
            8192,
            NULL,
            20, // prioridad de la tarea (0-24) , siendo 24 la prioridad más critica      
            NULL,
            CORE_1
    );      
}


// Esta es la tarea critica que definimos que aplica un voltaje determinado al motor

static void sendVoltsFromKnob(void *pvParameters) {

    // Aqui configuro cada cuanto se repite la tarea
   
    const TickType_t taskInterval = 100;    
    float userVolts;
    const float pulsesVoltsKnob = 5.0/48.0;
    float flag;

    // prototipo de una tarea repetitiva   
    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount();  
        
       // leemos el encoder, lo convertimos a grados  y lo imprimimos para plotear 
       userVolts =  constrain(encoderKnob.getCount() * pulsesVoltsKnob, -5, 5);
       

       // enviamos leido en la perilla a voltios en el motor
       voltsToMotor(userVolts);

       // imprimimos la slaida en voltios seleccionada por el usuario
       printf(">volts:%.2f \r\n", userVolts);
      
       // cambiamos el color del led dependiendo del signo de la tensión aplicada
       flag = (userVolts >= 0) ? 1 : 0;
       setLedColor(flag * red, 0 ,green * (1-flag));

       // la tarea es crítica entonces esperamos exactamente taskInterval ms antes de activarla nuevamente
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}



// desactivamos la funcion loop
void loop() {
    vTaskDelete(NULL);
}