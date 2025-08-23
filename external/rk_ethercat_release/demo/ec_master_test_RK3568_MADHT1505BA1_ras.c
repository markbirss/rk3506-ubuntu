#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <sched.h>
#include <pthread.h>
#include <stdarg.h>
/****************************************************************************/
 
#include "ecrt.h"
 
/****************************************************************************/
 
// Application parameters
// #define FREQUENCY 250
#define FREQUENCY 1000
#define CLOCK_TO_USE CLOCK_MONOTONIC
#define MEASURE_TIMING  1
#define TARGET_VELOCITY        1124000 /*target velocity*/

#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)                                                   
#define SHIFT_NS  (NSEC_PER_SEC / FREQUENCY /4)

#define DIFF_NS(A, B) (((B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
        (B).tv_nsec - (A).tv_nsec)

#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)

#define     STATUS_SERVO_ENABLE_BIT           (0x04)
/****************************************************************************/

// EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};
static ec_domain_t *domain = NULL;
static ec_domain_state_t domain_state = {};
static ec_slave_config_t *sc = NULL;
static ec_slave_config_state_t sc_state = {};
static uint8_t *domain_pd = NULL;

bool app_run = true;
/****************************************************************************/
 
// process data
 
const uint16_t MADHT1505BA1_alias           = 0;
const uint16_t MADHT1505BA1_position        = 0;
const int MADHT1505BA1_vendor          = 0x0000066f;
const int MADHT1505BA1_product_code    = 0x515050a1;
 
// offsets for PDO entries
unsigned int control_word;
unsigned int modes_of_operation;
unsigned int target_position;
unsigned int touch_probe_function;
unsigned int error_code;
unsigned int status_word;
unsigned int modes_of_operation_display;
unsigned int position_actual_value;
unsigned int touch_probe_status;
unsigned int touch_probe_pos1_pos_value;
unsigned int following_error_actual_value;
unsigned int digital_inputs;
unsigned int current_velocity;
unsigned int target_velocity;

static unsigned int cur_status;
static unsigned int cur_mode;

static unsigned int counter = 0;
static unsigned int sync_ref_counter = 0;
const struct timespec cycletime = {0, PERIOD_NS};

/*****************************************************************************/
 
/* Master 0, Slave 0, "MADHT1505BA1"
 * Vendor ID:       0x0000066f
 * Product code:    0x515050a1
 * Revision number: 0x00010000
 */

ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x6040, 0x00, 16}, /* Controlword */
    {0x6060, 0x00, 8}, /* Modes of operation */
    {0x607a, 0x00, 32}, /* Target position */
    {0x60b8, 0x00, 16}, /* Touch probe function */
    {0x60ff, 0x00, 32}, /* target_velocity */

    {0x603f, 0x00, 16}, /* Error code */
    {0x6041, 0x00, 16}, /* Statusword */
    {0x6061, 0x00, 8}, /* Modes of operation display */
    {0x6064, 0x00, 32}, /* Position actual value */
    {0x60b9, 0x00, 16}, /* Touch probe status */
    {0x60ba, 0x00, 32}, /* Touch probe pos1 pos value */
    {0x60f4, 0x00, 32}, /* Following error actual value */
    {0x60fd, 0x00, 32}, /* Digital inputs */
    {0x606c, 0x00, 32}, /* current_velocity */
};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 5, slave_0_pdo_entries + 0}, /* Receive PDO mapping 1 */
    {0x1a00, 9, slave_0_pdo_entries + 5}, /* Transmit PDO mapping 1 */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_0_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 1, slave_0_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

ec_pdo_entry_reg_t domain_regs[] = {
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x6040, 0, &control_word},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x6060, 0, &modes_of_operation},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x607a, 0, &target_position},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x60b8, 0, &touch_probe_function},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x603f, 0, &error_code},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x6041, 0, &status_word},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x6061, 0, &modes_of_operation_display},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x6064, 0, &position_actual_value},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x60b9, 0, &touch_probe_status},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x60ba, 0, &touch_probe_pos1_pos_value},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x60f4, 0, &following_error_actual_value},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x60fd, 0, &digital_inputs},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x606c, 0, &current_velocity},
{MADHT1505BA1_alias, MADHT1505BA1_position, MADHT1505BA1_vendor, MADHT1505BA1_product_code, 0x60ff, 0, &target_velocity},
{}};

static int64_t  system_time_base = 0LL;
int cpu_core = 3;
int debug_mode = 0;
int mode_option = 0;
/*****************************************************************************/

static const char short_options[] = "d:c:m:";
static const struct option long_options[] = {{"debug", required_argument, NULL, 'd'},
                                             {"cpu_core", no_argument, NULL, 'c'},
                                             {"mode", no_argument, NULL, 'm'},
                                             {"help", no_argument, NULL, 'h'},
                                             {0, 0, 0, 0}};

static void usage_tip(FILE *fp, int argc, char **argv) {
    fprintf(fp,
            "Usage: %s [options]\n"
            "Version %s\n"
            "Options:\n"
            "-d | --debug      Enabling debug mode\n"
            "-c | --cpu_core   bind cpu_core\n"
            "-m | --mode       mode options  0 is velocity mode, 1 is target mode\n"
            "-h | --help        for help \n\n"
            "\n",
            argv[0], "V1.0");
}

void get_opt(int argc, char *argv[]) {
    for (;;) {
        int idx;
        int c;
        c = getopt_long(argc, argv, short_options, long_options, &idx);
        if (-1 == c)
            break;
        switch (c) {
        case 0: /* getopt_long() flag */
            break;
        case 'd':
            debug_mode = atoi(optarg);
            break;
        case 'c':
            cpu_core = atoi(optarg);
            break;
        case 'm':
            mode_option = atoi(optarg);
            break;
        case 'h':
            usage_tip(stdout, argc, argv);
            exit(EXIT_SUCCESS);
        default:
            usage_tip(stderr, argc, argv);
            exit(EXIT_FAILURE);
        }
    }
}


void printf_debug(const char* fmt, ...){

  if (debug_mode) {
    va_list args;
    va_start(args, fmt); 
    vprintf(fmt, args); 
    va_end(args);
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

struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
    struct timespec result;

    if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC) {
        result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
    } else {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }

    return result;
}

void check_master_state(void)
{

    ec_master_state_t ms;
 
    ecrt_master_state(master, &ms);
 
    if (ms.slaves_responding != master_state.slaves_responding) {
        printf_debug("found %u slave(s).\n", ms.slaves_responding);
    }
    if (ms.al_states != master_state.al_states) {
        printf_debug("AL states: 0x%02X.\n", ms.al_states);
    }
    if (ms.link_up != master_state.link_up) {
        printf_debug("Link is %s.\n", ms.link_up ? "up" : "down");
    }
 
    master_state = ms;
}
 
/*****************************************************************************/
 
void check_slave_config_states(void)
{

    ec_slave_config_state_t s;
 
    ecrt_slave_config_state(sc, &s);
 
    //printf("sc->watchdog_divider = %d sc->watchdog_intervals = %d\n", sc->watchdog_divider, sc->watchdog_intervals);
    if (s.al_state != sc_state.al_state) {
        printf_debug("slaveDrive: State 0x%02X.\n", s.al_state);
    }
    if (s.online != sc_state.online) {
        printf_debug("slaveDrive: %s.\n", s.online ? "online" : "offline");
    }
    printf_debug("slaveDrive: %s  operational.\n", s.operational ? "yes" : "Not ");
 
    sc_state = s;
}
 
/*****************************************************************************/
 
void check_domain_state(void)
{

    ec_domain_state_t ds;
 
    ecrt_domain_state(domain, &ds);
 
    if (ds.working_counter != domain_state.working_counter) {
        printf_debug("Domain: WC %u.\n", ds.working_counter);
    }
    if (ds.wc_state != domain_state.wc_state) {
        printf_debug("Domain: State %u.\n", ds.wc_state);
    }
 
    domain_state = ds;
}
 
/*****************************************************************************/
void cyclic_task_position_mode()
{
    int tmp = false;
    struct timespec wakeupTime, time;
    static int curpos = 0;
    // get current time
    clock_gettime(CLOCK_TO_USE, &wakeupTime);

    while(app_run) {
        wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);

        // Write application time to master
        //
        // It is a good idea to use the target time (not the measured time) as
        // application time, because it is more stable.
        //
        ecrt_master_application_time(master, TIMESPEC2NS(wakeupTime));

        // receive process data
        ecrt_master_receive(master);
        ecrt_domain_process(domain);

        // check process data state (optional)
        check_domain_state();

        if (counter) {
            counter--;
        } else { // do this at 1 Hz
            counter = FREQUENCY;
            // check for master state (optional)
            check_master_state();
            check_slave_config_states();

            EC_WRITE_U16(domain_pd + control_word, 0x80); //复位错误码
            EC_WRITE_U8(domain_pd + modes_of_operation, 8); //设置当前控制器模式为位置模式
            cur_mode = EC_READ_U8(domain_pd + modes_of_operation_display);
            printf_debug("curMode: %d\t", cur_mode); //当前操作模式
            cur_status = EC_READ_U16(domain_pd + status_word);
            printf_debug("curStatus: %x\n", cur_status);
            if((cur_status & 0x004f) == 0x0040 && tmp == false) {
                EC_WRITE_U16(domain_pd + control_word, 0x06); 
                printf_debug("0x06\n");
            }
            else if((cur_status & 0x006f) == 0x0021 && tmp == false) {
                EC_WRITE_U16(domain_pd + control_word, 0x07);
                printf_debug("0x07\n");
            }
            else if((cur_status & 0x006f) == 0x0023 && tmp == false) {
                EC_WRITE_U16(domain_pd + control_word, 0x0F);
                printf_debug("0x0F\n");
            }
            else if((cur_status & 0x006f) == 0x0027 && tmp == false)
            {
                EC_WRITE_U16(domain_pd + control_word, 0x001f);
                printf_debug("0x1f\n");
    
                curpos = EC_READ_S32(domain_pd + position_actual_value);     
                printf_debug("madht >>> Axis  current position = %d\n", curpos);

                if((EC_READ_U16(domain_pd + status_word) & (STATUS_SERVO_ENABLE_BIT)) == 0) {
                    tmp = false;
                    printf_debug("EC_READ_U16(domain_pd + status_word) & (STATUS_SERVO_ENABLE_BIT)) == 0\n");
                    continue;
                }else {
                    tmp = true;
                }
            }

            if(tmp == true) {
                cur_status = EC_READ_U16(domain_pd + status_word);
                printf_debug("curpos = %d\t",curpos);
                printf_debug("actpos... %d\n",EC_READ_S32(domain_pd + position_actual_value));
                curpos += 10000;
                EC_WRITE_S32(domain_pd + target_position, curpos);
                tmp = false;
            }
        }

        if (sync_ref_counter) {
            sync_ref_counter--;
        } else {
            sync_ref_counter = 1; // sync every cycle

            clock_gettime(CLOCK_TO_USE, &time);
            ecrt_master_sync_reference_clock_to(master, TIMESPEC2NS(time));
        }
        ecrt_master_sync_slave_clocks(master);

        // send process data
        ecrt_domain_queue(domain);
        ecrt_master_send(master);
    }
}

static int clean_cycle = 0;//5 * 60 * FREQUENCY;
void cyclic_task_velocity_mode()
{
    static unsigned int timeout_error = 0;
    struct timespec wakeupTime, time;
    uint16_t    status;
    int8_t      opmode;
    int32_t     cur_velocity;
    bool        print = false;
    uint32_t	loop = 0;

    struct timespec startTime, endTime, lastStartTime, cur = {};
    uint32_t period_ns = 0, exec_ns = 0, latency_ns = 0,
             latency_min_ns = 0, latency_max_ns = 0,
             period_min_ns = 0, period_max_ns = 0,
             exec_min_ns = 0, exec_max_ns = 0;
             
    period_max_ns = 0;
    period_min_ns = 0xffffffff;
    latency_max_ns = 0;
    latency_min_ns = 0xffffffff;
    
    clock_gettime(CLOCK_TO_USE, &lastStartTime);
    clock_gettime(CLOCK_TO_USE, &wakeupTime);
    while(app_run) {
        wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);

        // Write application time to master
        //
        // It is a good idea to use the target time (not the measured time) as
        // application time, because it is more stable.
        //
        ecrt_master_application_time(master, TIMESPEC2NS(wakeupTime));
            
        /*Receive process data*/
        //ecrt_master_receive(master);
        ecrt_domain_process(domain);
        // check process data state (optional)
        check_domain_state();

        if (counter) {
            counter--;
        }else {
            counter = FREQUENCY;
            check_master_state();
            check_slave_config_states();
            /*Check process data state(optional)*/
            
            EC_WRITE_U16(domain_pd + control_word, 0x80);
            // EC_WRITE_U8(domain_pd + modes_of_operation, 9);
            /*Read inputs*/
            status = EC_READ_U16(domain_pd + status_word);
            opmode = EC_READ_U8(domain_pd + modes_of_operation_display);
            cur_velocity = EC_READ_S32(domain_pd + current_velocity);
            printf_debug("madht:  act velocity = %d ,  status = 0x%x, opmode = 0x%x\n", cur_velocity,  status, opmode);

            if( (status & 0x004f) == 0x0040) {
                printf_debug("0x06\n");
                EC_WRITE_U16(domain_pd + control_word, 0x0006);
                EC_WRITE_U8(domain_pd + modes_of_operation, 9);
            }
    
            else if( (status & 0x006f) == 0x0021) {
                printf_debug("0x07\n");
                EC_WRITE_U16(domain_pd + control_word, 0x0007);
            }
    
            else if( (status & 0x006f) == 0x0023) {
                printf_debug("0x0f\n");
                EC_WRITE_U16(domain_pd + control_word, 0x000f);
                EC_WRITE_S32(domain_pd + target_velocity, TARGET_VELOCITY);
            }
            
            //operation enabled
            else if( (status & 0x006f) == 0x0027) {
                printf_debug("0x1f\n");
                EC_WRITE_U16(domain_pd + control_word, 0x001f);
            }
        }

        clock_gettime(CLOCK_TO_USE, &time);
        ecrt_master_sync_reference_clock_to(master, TIMESPEC2NS(time));
        ecrt_master_sync_slave_clocks(master);
        // send process data
        ecrt_domain_queue(domain);
        ecrt_master_send(master);

        clock_gettime(CLOCK_TO_USE, &startTime);

	/* Assuming the packet received could be completed in 1/2 period. */
	for(loop = 0; loop < 100000000; loop++)
	{
	        clock_gettime(CLOCK_TO_USE, &cur);
	        if (DIFF_NS(startTime, cur) >= PERIOD_NS / 2) {
        		/*Receive process data*/
			ecrt_master_receive(master);
	        	break;
	        }
	}

        latency_ns = DIFF_NS(wakeupTime, startTime);
        period_ns = DIFF_NS(lastStartTime, startTime);
        //exec_ns = DIFF_NS(lastStartTime, endTime);
    /* clean */
        if (clean_cycle >= (5 * 60 * 1000)) {
            clean_cycle = 0;
            period_max_ns = 0;
            period_min_ns = 0xffffffff;
            latency_max_ns = 0;
            latency_min_ns = 0xffffffff;
        }

        if (latency_ns > latency_max_ns) {
            latency_max_ns = latency_ns;
        }
        if (latency_ns < latency_min_ns) {
            latency_min_ns = latency_ns;
        }

        if (period_ns > period_max_ns) {
            period_max_ns = period_ns;
            print = true;
        }
        if (period_ns < period_min_ns) {
            period_min_ns = period_ns;
            print = true;
        }

        clean_cycle++;
        lastStartTime = startTime;
        // output timing stats
        if (print) {
            printf("period     %10u ... %10u\n",
                    period_min_ns, period_max_ns);
        //printf("exec       %10u ... %10u\n",
        //        exec_min_ns, exec_max_ns);
        printf("latency    %10u ... %10u\n",
                latency_min_ns, latency_max_ns);
        }
        //period_max_ns = 0;
        //period_min_ns = 0xffffffff;
        //exec_max_ns = 0;
        //exec_min_ns = 0xffffffff;
        //latency_max_ns = 0;
        //latency_min_ns = 0xffffffff;
        print = false;

    }
}
 
/****************************************************************************/

void sigint_handler(int sig){
    if(sig == SIGINT){
        // ctrl+c退出时执行的代码
        printf("ctrl+c pressed!\n");
        app_run = false;
    }
}

/****************************************************************************/
 
int main(int argc, char **argv)
{
    struct sigaction sa;
    struct itimerval tv;
    uint32_t abort_code = 0;
    struct timespec wakeupTime;
    signal(SIGINT, sigint_handler);
    printf("##### rockchip ethercat test #####\n");
    printf("start thread set\n");
    struct sched_param param;
    int maxpri, count;

    get_opt(argc, argv);
    printf("cpu_core = %d debug_mode = %d mode_option = %d\n", cpu_core, debug_mode, mode_option);

    // bing cpu core
    if(thread_bind_cpu(cpu_core) == -1) {
        printf("bind cpu core fail\n");
        return -1;
    }

    // The scheduling priority is the highest
    maxpri = sched_get_priority_max(SCHED_FIFO);
    if(maxpri == -1) { 
        printf("sched_get_priority_max() failed"); 
        return -1; 
    }

    printf("max priority of SCHED_FIFO is %d\n", maxpri);
    param.sched_priority = maxpri;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &param) == -1) { 
        perror("sched_setscheduler() failed"); 
        return -1; 
    }
    printf("end thread set\n");

    master = ecrt_request_master(0);
    if (!master) {
        printf("ecrt_request_master is err\n");
        return -1;
    }

    printf("request_master sucess\n");

    check_master_state();
 
    for(int cnt = 0; cnt < master_state.slaves_responding; cnt++) {
        printf("configure slave slaves_responding %d\n", cnt+1);
        sc = ecrt_master_slave_config(
                            master, 
                            MADHT1505BA1_alias, 
                            MADHT1505BA1_position, 
                            MADHT1505BA1_vendor, 
                            MADHT1505BA1_product_code);
        if(!sc) {
            printf("Failed to get slave configuration.\n");
            return -1;
        }
    }
    
    //ecrt_master_reset(master);
    check_slave_config_states();
    
    domain = ecrt_master_create_domain(master);
    if (!domain) {
        printf("ecrt_master_create_domain is fail\n");
        return -1;  
    }

    printf("Configuring PDOs...\n");
    if (ecrt_slave_config_pdos(sc, EC_END, slave_0_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }
    if(ecrt_domain_reg_pdo_entry_list(domain, domain_regs)) {
        fprintf(stderr, "Failed to ecrt_domain_reg_pdo_entry_list.\n");
        return -1;
    };
    // configure SYNC signals for this slave arg0: sc arg1:register  arg2:sync0_cycle_time, arg3:sync0_shift_time, arg4:sync1_cycle_time, arg5:sync1_shift_time
    int frequency = FREQUENCY;
    printf("configure SYNC signals for this slave... FREQUENCY %d\n", frequency);
    ecrt_slave_config_dc(sc, 0x300, PERIOD_NS, 0, 0, 0);

    printf("Activating master...\n");
    if (ecrt_master_activate(master)) {
        printf("ecrt_master_activate is fail\n");
        return -1;
    }

    printf("domain_pd ...\n");
    if (!(domain_pd = ecrt_domain_data(domain))) {
        printf("ecrt_domain_data is fail\n");
        return -1;
    }
 
    printf("Started.\n");
    cyclic_task_velocity_mode();
    
    ecrt_master_deactivate(master);
    ecrt_release_master(master);
    master = NULL;
    printf("rockchip ec_test is end\n");
    return 0;
}
