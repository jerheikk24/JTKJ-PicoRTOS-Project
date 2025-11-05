// Kirjastojen esittelyt

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include "tkjhat/sdk.h"


#define DEFAULT_STACK_SIZE 2048
#define CDC_ITF_TX      1


// Tilakoneen esittely 

enum state { WAITING, DATA_READY, VALMIS};
enum state programState = WAITING;

// Imu-anturin globaalit muuttujat ja niiden esittely
float ax, ay, az;
char symboli;

// Ensimmainen taski

static void sensor_task(void *arg){
    (void)arg;
    

    init_ICM42670();                // IMU-anturin alustus
    ICM42670_start_with_default_values();
    
    for(;;){
        if (programState == WAITING) {
            ICM42670_read_sensor_data(&ax, &ay, &az, NULL, NULL, NULL, NULL);
            if (ay < -0.8) {
                symboli = '.';
                programState = DATA_READY;
            } 
            else if (ax > 0.8) {
                symboli = '-';
                programState = DATA_READY;
            }
            else if (ax < -0.8) {
                symboli = (char)' ';
                programState = DATA_READY;
            }
        }
    

        // Tehtävä 3:  Muokkaa aiemmin Tehtävässä 2 tehtyä koodia ylempänä.
        //             Jos olet oikeassa tilassa, tallenna anturin arvo tulostamisen sijaan
        //             globaaliin muuttujaan.
        //             Sen jälkeen muuta tilaa.


        // Do not remove this
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void print_task(void *arg){
    (void)arg;
    
    while(1){
        if (programState == DATA_READY) {
            programState = VALMIS;
            printf("%c", symboli);        // HUOM tarkista tarviiko VALMIS tilaa
            programState = WAITING;
        }
        
        
        // Do not remove this
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


// Exercise 4: Uncomment the following line to activate the TinyUSB library.  
// Tehtävä 4:  Poista seuraavan rivin kommentointi aktivoidaksesi TinyUSB-kirjaston. 

/*
static void usbTask(void *arg) {
    (void)arg;
    while (1) {
        tud_task();              // With FreeRTOS wait for events
                                 // Do not add vTaskDelay. 
    }
}*/

int main() {

   

    stdio_init_all();

    // Uncomment this lines if you want to wait till the serial monitor is connected
    /*while (!stdio_usb_connected()){
        sleep_ms(10);
    }*/ 
    
    init_hat_sdk();
    sleep_ms(300); //Wait some time so initialization of USB and hat is done.

    
    // Tehtävä 1:  Alusta painike ja LEd ja rekisteröi vastaava keskeytys.
    //             Keskeytyskäsittelijä on määritelty yläpuolella nimellä btn_fxn
    



    
    
    TaskHandle_t hSensorTask, hPrintTask, hUSB = NULL;

    // Exercise 4: Uncomment this xTaskCreate to create the task that enables dual USB communication.
    // Tehtävä 4: Poista tämän xTaskCreate-rivin kommentointi luodaksesi tehtävän,
    // joka mahdollistaa kaksikanavaisen USB-viestinnän.

    /*
    xTaskCreate(usbTask, "usb", 2048, NULL, 3, &hUSB);
    #if (configNUMBER_OF_CORES > 1)
        vTaskCoreAffinitySet(hUSB, 1u << 0);
    #endif
    */


    // Create the tasks with xTaskCreate
    BaseType_t result = xTaskCreate(sensor_task, // (en) Task function
                "sensor",                        // (en) Name of the task 
                DEFAULT_STACK_SIZE,              // (en) Size of the stack for this task (in words). Generally 1024 or 2048
                NULL,                            // (en) Arguments of the task 
                2,                               // (en) Priority of this task
                &hSensorTask);                   // (en) A handle to control the execution of this task

    if(result != pdPASS) {
        printf("Sensor task creation failed\n");
        return 0;
    }
    result = xTaskCreate(print_task,  // (en) Task function
                "print",              // (en) Name of the task 
                DEFAULT_STACK_SIZE,   // (en) Size of the stack for this task (in words). Generally 1024 or 2048
                NULL,                 // (en) Arguments of the task 
                2,                    // (en) Priority of this task
                &hPrintTask);         // (en) A handle to control the execution of this task

    if(result != pdPASS) {
        printf("Print Task creation failed\n");
        return 0;
    }

    // Start the scheduler (never returns)
    vTaskStartScheduler();
    
    // Never reach this line.
    return 0;
}

