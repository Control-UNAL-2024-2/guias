// libreria para el control de los leds
#include <definitions.h>






void setup() {
    // iniciar perifericos
    setupPeripherals();

    // Asi definimos una tarea para el sistema operativo
    xTaskCreatePinnedToCore(
            blinkledTask, // nombre de la rutina
            "blinks the rgb led",
            8192,
            NULL,
            23, // prioridad de la tarea (0-24) , siendo 24 la prioridad más critica      
            NULL,
            CORE_1
    );

    

    // Asi definimos una segunda tarea para poner a funcionar botones

    xTaskCreatePinnedToCore(
            buttonTask, // nombre de la rutina
            "activate motor with the buttons",
            8192,
            NULL,
            10,  // definimos una prioridad baja para esta tarea
            NULL,
            CORE_0 // la vamos a ejecutar en el CORE_0 que es comparte tareas con el procesador 
    );
   

  
}


// Esta es la tarea critica que definimos, la cual cambia solamente de color el led


static void blinkledTask(void *pvParameters) {

    // Aqui configuro cada cuanto se repite la tarea
    const TickType_t taskInterval = 500;
    
    // bandera para cambiar de color el led    
    bool flag = 1;

    // prototipo de una tarea repetitiva   
    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount();  

       // ajusta el color del led 
       setLedColor(red, green, blue);
       
       flag = !flag;  // flag pasa de 0 a 1, o de 1 a 0 para cambiar el color
       
       // la tarea es crítica entonces esperamos exactamente 500ms antes de activarla nuevamente
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}




// Esta es la segunda tarea de baja prioridad que atiende los botones

static void buttonTask(void *pvParameters) {


    // prototipo de una tarea repetitiva   
    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount();  

       if (touchRead(BUTTON_UN) > threshold){
           // si presiona el boton  UN activamos el motor en un sentido y ponemos el led en rojo
           voltsToMotor(.8);           
           red = 127;
           green = 20;
           blue = 0;
           voltsToMotor(0);    

            }

       if (touchRead(BUTTON_LIFEFLOWER) > threshold){
        // si presiona el boton flor de la vida activamos el motor en otro sentido y ponemos el led en azul cyan
           voltsToMotor(-0.8);
           red = 0;    
           green = 127;
           blue = 127;
            voltsToMotor(0);    
            }



       // la tarea no es critica entonces podemos pararla con un vTaskdelay
       vTaskDelay(100);
    }

}




// la tarea loop se desactiva
void loop(){     
     vTaskDelete(NULL);
}
