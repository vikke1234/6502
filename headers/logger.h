#ifndef LOGGER_H
#define LOGGER_H

#include "cpu.h"

void log_cpu(registers_t reg);
void init_log(void);
void close_log(void);


#endif /* LOGGER_H */
