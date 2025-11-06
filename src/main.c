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

enum state { WAITING, DATA_READY,};
enum state programState = WAITING;

// Imu-anturin globaalit muuttujat ja niiden esittely
float ax, ay, az;
char symboli;
int counter = 0;

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
                counter = 0;
                programState = DATA_READY;
            } 
            else if (ax > 0.8) {
                symboli = '-';
                counter = 0;
                programState = DATA_READY;
            }
            else if (ax < -0.8) {
                symboli = (char)' ';
                counter++;
                programState = DATA_READY;
            }

        }

        // Do not remove this
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void print_task(void *arg){
    (void)arg;
    
    while(1){
        if (programState == DATA_READY) {
            printf("%c", symboli);
            if (counter == 2){
                printf("\n");
                counter=0;
            }
            programState = WAITING;
        }
        
        
        // Do not remove this
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


int main() {
    stdio_init_all();
    init_hat_sdk();
    sleep_ms(300); 

    TaskHandle_t hSensorTask, hPrintTask, hUSB = NULL;

    // 
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
    
    return 0;
}