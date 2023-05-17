#include <string.h>
#include <stdbool.h>
#include "timex.h"
#include "xtimer.h"
#include "stdio.h"
#include "periph/adc.h"
#include "analog_util.h"
#include "thread.h"

#define WATER_FLOW ADC_LINE(0)
#define ADC_RES ADC_RES_12BIT

#define SECONDS 1*US_PER_SEC //regulator for water_flow_rate formula
#define SAMPLING 5*US_PER_SEC //duration time of sampling
#define SMALL_SAMPLING 1*US_PER_SEC
#define DAILY_PERIODIC 30*US_PER_SEC //periodic time of sampling
#define SMALL_PERIODIC 15*US_PER_SEC
//char water_sensor_stack[THREAD_STACKSIZE_MAIN];
//kernel_pid_t water_sensor_thread;

xtimer_ticks32_t sample_time_now;
xtimer_ticks32_t sample_time_end;
xtimer_ticks32_t sample_time_diff;

xtimer_ticks32_t periodic_time;

//sudo BOARD=esp32-heltec-lora32-v2 BUILD_IN_DOCKER=1 DOCKER="sudo docker" PORT=/dev/ttyUSB0 make flash

/*
We are in the same situation of YF-S201 water flow sensor - 30L/min

l_hour = (flow_frequency * 60 / 7.5)
every liter of water that flows, the Hall Sensor outputs 450 pulses. 

V_total(L) = N* 1/450(L) 

the total volume of fluid flowing through the water flow sensor is equal to the water flow rate(Q - unit L/s) multiplied by time t(unit s) .
V_total(L) = Q(L/s)*t(s) 

N* 1/450 = Q(L/s)*t(s) 
N/t = 450 * Q(L/s) 

f = 450*Q(L/s); 
Q(L/s) = f/450; 
Q(L/min) = f*60/450 = f/7.5 
Q(L/hour) = f*60*60/450 = f*60 /7.5 

l_hour = (flow_frequency * 60 / 7.5)


LEGGERE: https://wiki.seeedstudio.com/Water-Flow-Sensor/#:~:text=Water%20flow%20sensor%20consists%20of,outputs%20the%20corresponding%20pulse%20signal.


SU AMAZON ->  1L = 596 impulsi ----- https://www.amazon.it/dp/B079QYRQT5?psc=1&ref=ppx_yo2ov_dt_b_product_details
K=541,3307 [Impulsi/Litro] con errore di +-1% ma anche così 
USIAMO QUESTA
*/


int water_sensor(void){
    printf("water sensor thread start\n");
    
    int middle_turn=-1;
    int sample=0;

    float number_turn=0.0;
    float remain=0.0;
    float frequency=0.0;
    float water_flow_rate=0.0;
    
    sample = adc_sample(WATER_FLOW,ADC_RES);

    if (sample==0){
        //middle_turn=0;
        remain=1;
    }
    else{ 
        //middle_turn=0;  
    }
    
    sample_time_now=xtimer_now();

    while(1){
        sample = adc_sample(WATER_FLOW,ADC_RES);
        
        if (middle_turn==0 && sample==4095){
            number_turn++;
        }
        middle_turn=sample;
        
        sample_time_end=xtimer_now();
        sample_time_diff=xtimer_diff(sample_time_end, sample_time_now); //microsecondi
        
        if(sample_time_diff > SAMPLING){
            break;
        }

        if(sample_time_diff > SMALL_SAMPLING && number_turn==0.0){
            printf("No flow detected\n");
            return 0;
        }

    }

    if(remain!=0){
        number_turn=number_turn+remain;
    }
    
    printf("time: %d microseconds\t turns: %f\n",sample_time_diff,number_turn);
           
    //Q(L/s) = f/impulsi; -> Q(L/min) = f*60/impulsi  
    //usiamo per ora 541
    frequency=number_turn/(float)sample_time_diff*SECONDS;
    water_flow_rate=(float)frequency*60/541; 
    printf("frequency: %f Hz\t water_flow_rate: %f L/min\n",frequency,water_flow_rate);
    printf("water sensor thread end\n");

    return 1;
}



int main(void)
{
    
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    // initialize the FLAME_PIN (ADC) line
    if (adc_init(WATER_FLOW) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", WATER_FLOW);
        return 1;
    } else {
        printf("Successfully initialized ADC_LINE(%u)\n", WATER_FLOW);
    }
    
	/* Thread 1 to collect water data from water sensor     
	Thread 2 responsible for interrupt service 
	interrupt_service_thread=thread_create(interrupt_service_stack, sizeof(interrupt_service_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING,
                  interrupt_service, NULL, "interrupt_service");
	water_sensor_thread=thread_create(water_sensor_stack, sizeof(water_sensor_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_SLEEPING,
                  water_sensor, NULL, "water_sensor");
    */
    int val_last_test=1;
    periodic_time = xtimer_now();
    while(1){
        val_last_test=water_sensor();
        if(val_last_test==1){
            printf("done\n");
            xtimer_periodic_wakeup(&periodic_time, DAILY_PERIODIC);
        }    
        else{
            printf("to do again\n");
            xtimer_periodic_wakeup(&periodic_time, SMALL_PERIODIC);
        }
        printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    }

    printf("main thread end\n");

	return 0;
    
}    
