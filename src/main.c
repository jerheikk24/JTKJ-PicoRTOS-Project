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

enum state { WAITING=1, DATA_READY};
enum state programState = WAITING;

// Imu-anturin globaalit muuttuja
float ax, ay, az;

static void btn_fxn(uint gpio, uint32_t eventMask) {
     uint8_t pinValue = gpio_get(LED1);
    pinValue = !pinValue;
    gpio_put(LED1, pinValue);
}


// Ensimmainen taski
//
static void sensor_task(void *arg){
    (void)arg;
    float ax, ay, az;               // Muuttujien esittely
    init_ICM42670();                // IMU-anturin alustus
    ICM42670_start_with_default_values();
    
    for(;;){
        if (programState == WAITING) {
            ICM42670_read_sensor_data(&ax, &ay, &az, NULL, NULL, NULL, NULL);
            if (ay < -0.8) {
                printf(".\n");
            } 
            else if (ax > 0.8) {
                printf("-\n");
            }
            else if (ax < -0.8) {
                printf("valilyonti\n");
            }
            programState = DATA_READY;
        }
    

    
        // tight_loop_contents(); 


   


        // Tehtävä 3:  Muokkaa aiemmin Tehtävässä 2 tehtyä koodia ylempänä.
        //             Jos olet oikeassa tilassa, tallenna anturin arvo tulostamisen sijaan
        //             globaaliin muuttujaan.
        //             Sen jälkeen muuta tilaa.
       
        // Exercise 2. Just for sanity check. Please, comment this out
        // Tehtävä 2: Just for sanity check. Please, comment this out
        // printf("sensorTask\n");

        // Do not remove this
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void print_task(void *arg){
    (void)arg;
    
    while(1){
        if (programState == DATA_READY) {
           
            programState = WAITING;
        }
        
        // Tehtävä 3: Kun tila on oikea, tulosta sensoridata merkkijonossa debug-ikkunaan
        //            Muista tilamuutos
        //            Älä unohda kommentoida seuraavaa koodiriviä.
       
        // tight_loop_contents();
        


        
        //printf("printTask\n");
        
        // Do not remove this
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    init_red_led ();
    init_button1 ();
    gpio_set_irq_enabled_with_callback(BUTTON1, GPIO_IRQ_EDGE_FALL, true, btn_fxn);



    
    
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

