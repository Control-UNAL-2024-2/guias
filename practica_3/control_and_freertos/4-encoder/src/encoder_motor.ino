
#include <definitions.h>


void setup() {
    // incluir perifericos
    setupPeripherals();

    // Asi definimos una tarea para el sistema operativo
    xTaskCreatePinnedToCore(
            readEncoderTask, // nombre de la rutina
            "this task reads encoders",
            8192,
            NULL,
            20, // prioridad de la tarea (0-24) , siendo 24 la prioridad más critica      
            NULL,
            CORE_1
    );  
}


// Esta es la tarea critica que definimos, la cual cambia solamente de color el led


static void readEncoderTask(void *pvParameters) {

    // Aqui configuro cada cuanto se repite la tarea   
    const TickType_t taskInterval = 200;    

    // variables locales de la tarea
    float y;
    float reference;
    float flag;


    // prototipo de una tarea repetitiva   
    for (;;) {
        TickType_t xLastWakeTime = xTaskGetTickCount();  
        
        // leemos el encoder del ángulo del motor, lo convertimos a grados  para tener el angulo de giro
        y = encoderMotor.getCount() * pulsesTodegreesMotor;

        // leemos el encoder de la perilla lo convertimos a grados  
        reference = encoderKnob.getCount() * pulsesTodegreesKnob;

        // imprimimos en el serial para ploteR
        printf(">Perilla:%0.2f, Angulo:%.2f\r\n", reference, y);

        
        // la tarea es crítica entonces esperamos exactamente taskInterval ms antes de activarla nuevamente
        vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}


void loop() {
    vTaskDelete(NULL);
}
