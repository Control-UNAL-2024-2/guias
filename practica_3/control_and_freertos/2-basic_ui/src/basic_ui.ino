#include <definitions.h>




void setup() {

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

       // el led pone color o apagado con el cambio de flag
       setLedColor(red * flag, green * flag, blue * flag);
       
       flag = !flag;  // flag pasa de 0 a 1, o de 1 a 0 y apaga o prende el led
       
       // la tarea se repetirá despues de taskInterval = 500 ms
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}


// Esta es la segunda tarea de baja prioridad que atiende los botones del usuario

static void buttonTask(void *pvParameters) {

    // prototipo de una tarea repetitiva   
    for (;;) {

       // Si el boton con logo UN supera el umbral pongo un color naranja 
       if (touchRead(BUTTON_UN) > threshold){
           red = 200;
           green = 64;
           blue = 0;
        } 
        
        // Si el boton con el dibujo de flor de la vida supera el umbral pongo un color azul claro 
       if (touchRead(BUTTON_LIFEFLOWER) > threshold){  
           red = 0;    
           green = 127;
           blue = 127;
            }
       //   
       vTaskDelay(100); // espero 100 ms antes de repetir la tarea
    }

}

// la tarea loop se desactiva
void loop(){     
     vTaskDelete(NULL);
}
