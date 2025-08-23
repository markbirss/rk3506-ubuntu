#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct cpu_occupy_
{
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
} cpu_occupy_t;

static double cal_cpuoccupy(cpu_occupy_t *o, cpu_occupy_t *n)
{
    double od, nd;
    double id, sd;
    double cpu_use;

    od = (double)(o->user + o->nice + o->system + o->idle + o->softirq + o->iowait +
                  o->irq);
    nd = (double)(n->user + n->nice + n->system + n->idle + n->softirq + n->iowait +
                  n->irq);

    id = (double)(n->idle);
    sd = (double)(o->idle);
    if ((nd - od) != 0)
        cpu_use = 100.0 - ((id - sd)) / (nd - od) * 100.00;
    else
        cpu_use = 0;
    return cpu_use;
}

static void get_cpuoccupy(cpu_occupy_t *cpust)
{
    FILE *fd;
    int n;
    char buff[256];
    cpu_occupy_t *cpu_occupy;
    cpu_occupy = cpust;

    fd = fopen("/proc/stat", "r");
    if (fd == NULL)
    {
        printf("open /proc/stat failed\n");
        return;
    }
    if (fgets(buff, sizeof(buff), fd) == NULL)
        printf("fgets failed\n");

    sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name,
           &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system,
           &cpu_occupy->idle, &cpu_occupy->iowait, &cpu_occupy->irq,
           &cpu_occupy->softirq);

    fclose(fd);
}

double get_cpu_usage(void)
{
    static cpu_occupy_t cpu_stat1;
    cpu_occupy_t cpu_stat2;
    double cpu;

    get_cpuoccupy(&cpu_stat2);
    cpu = cal_cpuoccupy(&cpu_stat1, &cpu_stat2);
    memcpy(&cpu_stat1, &cpu_stat2, sizeof(cpu_occupy_t));

    return cpu;
}

