#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sched.h>
#include <pthread.h>
#include "ecrt.h"
#include "Rockchip_MADHT1505BA1.h"

bool app_run = true;
int  velocity = 1124000;

void sigint_handler(int sig){
    if(sig == SIGINT){
        // ctrl+c退出时执行的代码
        printf("ctrl+c pressed!\n");
        app_run = false;
    	printf("rk_test end\n");
    	MADHT1505BA1_master_deinit(); 
    }
}

static int thread_bind_cpu(int target_cpu)
{
    cpu_set_t mask;
    int cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    int i;

    if (target_cpu >= cpu_num)
        return -1;

    CPU_ZERO(&mask);
    CPU_SET(target_cpu, &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        perror("pthread_setaffinity_np");

    if (pthread_getaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        perror("pthread_getaffinity_np");

    printf("Thread(%ld) bound to cpu:", gettid());
    for (i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &mask)) {
            printf(" %d", i);
            break;    
        }
    }
    printf("\n");

    return i >= cpu_num ? -1 : i;
}

int main(int argc, char **argv) {
	printf("rk_test start\n");
	int ret = 0;
	int choice = 0;
	int maxpri;
	struct sched_param param;

	MADHT1505BA1_object slave0;

	signal(SIGINT, sigint_handler);

    if(thread_bind_cpu(1) == -1) {
        printf("bind cpu core fail\n");
    }

    // The scheduling priority is the highest
    maxpri = sched_get_priority_max(SCHED_FIFO);
    if(maxpri == -1) { 
        printf("sched_get_priority_max() failed");
    }

    param.sched_priority = maxpri;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &param) == -1) { 
        perror("sched_setscheduler() failed");
    }

	ret = MADHT1505BA1_master_init(3); //bind cpu core 3
	if(ret == -1) {
		printf("MADHT1505BA1_master_init is err\n");
	}
	slave0.alias = 0;
	slave0.position = 0;

	ret = MADHT1505BA1_slaves_init(&slave0);
	if(ret == -1) {
		printf("MADHT1505BA1_slaves_init0 is err\n");
		return -1;
	}
	ret = MADHT1505BA1_master_activate();
	if(ret == -1) {
		printf("MADHT1505BA1_master_activate is err\n");
		return -1;
	}
	ret = MADHT1505BA1_slaves_activate(&slave0);
	if(ret == -1) {
		printf("MADHT1505BA1_slaves_activate0 is err\n");
		return -1;
	}
	ret = MADHT1505BA1_slave_start(1, &slave0);
	if(ret == -1) {
		printf("MADHT1505BA1_slave_start0 is err\n");
		return -1;
	}

	printf("Please wait while checking whether the motor is operational...\n");
	while((MADHT1505BA1_check_motor(&slave0) == -1)) {
		sleep(1);
	}
	printf("motor is ok\n");

	while(app_run) {
    	printf("1. Motor operation\n");
    	printf("2. Motor stop\n");
    	printf("3. exit\n");
    	printf("\nEnter your choice (1-3): ");
    	scanf("%d", &choice);

    	switch(choice) {
    	    case 1:
				MADHT1505BA1_motor_set_position_run(100000, &slave0);
    	        break;
    	        
    	    case 2:
    	        MADHT1505BA1_position_reset(&slave0);
    	        break;
    	    
    	    case 3:
    	    	printf("rk_test end\n");
    	    	MADHT1505BA1_master_deinit();   	        
    	    	return 0;

    	    default:
    	        printf("Invalid choice!\n");
    	        break;
    	}
    	usleep(100);
	}
	
	return 0;
}