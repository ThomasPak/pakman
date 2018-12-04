#ifndef COMMON_H
#define COMMON_H

/**** Results ****/
const int REJECT = 0;
const int ACCEPT = 1;
const int ERROR = 2;
const int CANCEL = 3;

typedef enum
{
    forked_process,
    mpi_process
} process_t;

#endif // COMMON_H
