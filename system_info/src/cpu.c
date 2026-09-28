#include <sys/sysinfo.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>

#include "internal.h"
Error_Code get_cpu_usage(double *usage)
{
    unsigned long long idle1;
    unsigned long long total1;

    unsigned long long idle2;
    unsigned long long total2;

    if (read_cpu_times(&idle1, &total1) != 0)
        return CPU_READ_ERROR;

    usleep(200000);

    if (read_cpu_times(&idle2, &total2) != 0)
        return CPU_READ_ERROR;

    unsigned long long idle_delta = idle2 - idle1;

    unsigned long long total_delta = total2 - total1;

    if (total_delta == 0)
        return CPU_INVALID_ERROR;

    *usage =100.0 * (1.0 - ((double)idle_delta / (double)total_delta));

    return OK;
}

Error_Code read_cpu_times(unsigned long long *idle, unsigned long long *total)
{
    FILE *fp;
    char line[256];

    unsigned long long user = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idle_time = 0;
    unsigned long long iowait = 0;
    unsigned long long irq = 0;
    unsigned long long softirq = 0;
    unsigned long long steal = 0;

    int scanned;

    if (idle == NULL || total == NULL)
        return CPU_INVALID_ERROR;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
        return CPU_READ_ERROR;

    if (fgets(line, sizeof(line), fp) == NULL){
        fclose(fp);
        return CPU_READ_ERROR;
    }

    fclose(fp);

    scanned = sscanf(line,"%*s %llu %llu %llu %llu %llu %llu %llu %llu",
        &user,&nice,&system,&idle_time,&iowait,&irq,&softirq,&steal);

    if (scanned < 8)
        return CPU_READ_ERROR;

    *idle = idle_time + iowait;

    *total = user + nice + system +idle_time +iowait +irq +softirq +steal;

    return OK;
}