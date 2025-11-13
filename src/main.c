// Jere Heikkila, Aleksi Pietila & Aleksi Vuopala
// Kaikille jaetaan pisteet tasan


// Kirjastojen esittelyt, suoraan JTKJ 2. labrasessiosta, osa otettu pois joita ei tarvita

#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tkjhat/sdk.h"


#define DEFAULT_STACK_SIZE 2048                 // Muistin määrä, 2048 tavua (sanaa)


// Tilakoneen esittely 

enum state { WAITING, DATA_READY,};             // Tilakone:  Kaksi tilaa (WAITING ja DATA_READY)
enum state programState = WAITING;              


// Imu-anturin globaalit muuttujat ja niiden esittely
float ax, ay, az;
char symboli;
int counter = 0;


// Ensimmainen taski anturille, joka mittaa asentoa

static void sensor_task(void *arg){                   //!! tehnyt Jere H !!
    (void)arg;
    
    init_ICM42670();                                 // IMU-anturin alustus
    ICM42670_start_with_default_values();            // Alustaa imu-anturille oletusarvot
    

    for(;;){
        if (programState == WAITING) {                                                  // Jos on odotustilassa (WAITING) -> niin aloittaa mittaamisen
            ICM42670_read_sensor_data(&ax, &ay, &az, NULL, NULL, NULL, NULL);           // Kiihtyvyysantureita ei tarvita -> ne NULL:iksi
            if (ay < -0.8) {
                symboli = '.';                  // Tulee piste
                counter = 0;
                programState = DATA_READY;                                              // Tilanvaihto takaisin DATA_READY (jokaisessa silmukassa)
            } 
            else if (ax > 0.8) {
                symboli = '-';                  // Tulee viiva
                counter = 0;
                programState = DATA_READY;  
            }
            else if (ax < -0.8) {
                symboli = (char)' ';            // Tulee valilyonti
                counter++;                      // Sama kuin counter + 1, tarvitaan rivinvaihdon muodostamiseen
                programState = DATA_READY;
            }

        }

        // "odottaa" 1 sekunnin
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// Toinen taski, joka tulostaa sensortaskin tuloksen

static void print_task(void *arg){              //!! Tehnyt Aleksi P ja Aleksi V !!
    (void)arg;
    
    while(1){
        if (programState == DATA_READY) {       // Jos on tila DATA_READY, niin...
            printf("%c", symboli);              // Tulostaa kyseisen symbolin, missa asennossa pico on 
            if (counter == 2){
                printf("\n");                   // Ehtolause, jos valilyonteja on kaksi, niin tulee samalla rivinvaihto 
                counter=0;
            }
            programState = WAITING;             // Tilavaihto takaisin odotustilaan (WAITING)
        }
        
        
        // "odottaa" 1 sekunnin
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// Tasta alaspain on otettu suoraan JTKJ 2. labrasessiosta, tosin ollaan selitetty mita mikakin tarkoittaa

int main() {
    stdio_init_all();
    init_hat_sdk();                                     // JTK:n alustus
    sleep_ms(300);                                      // Odottaa 0.3s, etta USB ja anturi kerkeaa kaynnistya

    TaskHandle_t hSensorTask, hPrintTask, hUSB = NULL;

    // 
    BaseType_t result = xTaskCreate(sensor_task,        // Taskin funktio
                "sensor",                               // Taskin nimi 
                DEFAULT_STACK_SIZE,                     // Muistimaara taskille, tassa 2048
                NULL,                                   // Parametri tehtavalle
                2,                                      // Taskin prioriteetti
                &hSensorTask);                          // Taskin osoitin

    if(result != pdPASS) {
        printf("Sensor task creation failed\n");
        return 0;
    }
    result = xTaskCreate(print_task,                // Katso ylempaa maarittelyt, taysin samat
                "print",               
                DEFAULT_STACK_SIZE,   
                NULL,                  
                2,                   
                &hPrintTask);         


    if(result != pdPASS) {
        printf("Print Task creation failed\n");
        return 0;
    }

    // Ajastimen käynnistys

    vTaskStartScheduler();
    
    return 0;
}