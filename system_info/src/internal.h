#ifndef SYSTEM_INFO_INTERNAL_H
#define SYSTEM_INFO_INTERNAL_H

#include "system_info.h"

Error_Code get_cpu_usage(double *usage);
Error_Code read_cpu_times(unsigned long long *idle, unsigned long long *total);


#endif