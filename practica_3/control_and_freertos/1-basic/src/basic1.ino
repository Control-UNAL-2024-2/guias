
#include <definitions.h>




void setup() {

    // Asi definimos una tarea para el sistema operativo
    xTaskCreatePinnedToCore(
            blinkledTask, // nombre de la rutina
            "blinks the rgb led", // descripcion
            8192,
            NULL,
            20, // prioridad
            NULL,
            CORE_1
    );
  
}


// Esta es la tarea que definimos, la cual solo cambia de color el led


static void blinkledTask(void *pvParameters) {

    // Aqui configuro la duracion de la tarea
    const TickType_t taskInterval = 500;
    
    // bandera para cambiar de color el led    
    bool flag = 1;

    // prototipo de una tarea repetitiva   
    for (;;) {
       TickType_t xLastWakeTime = xTaskGetTickCount();  

       // cambian de color el led de rojo a verde por con el cambio del flag
       setLedColor(red * flag, green * (1-flag), blue);
       
       // invierto la bandera para cambiar el led de color
       flag = !flag;  // flag pasa de 0 a 1 o de 1 a 0    para cambiar el color
       
       // la tarea se repetirá despues de taskInterval = 500ms
       vTaskDelayUntil(&xLastWakeTime, taskInterval);
    }

}

// la tarea loop se desactiva
void loop(){     
     vTaskDelete(NULL);
}
