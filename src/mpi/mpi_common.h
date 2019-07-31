#ifndef MPI_COMMON_H
#define MPI_COMMON_H

const int MASTER_RANK = 0;
const int WORKER_RANK = 0;

///// Tags /////
const int MASTER_MSG_TAG = 0;
const int MASTER_SIGNAL_TAG = 1;
const int MANAGER_MSG_TAG = 2;
const int MANAGER_SIGNAL_TAG = 3;
const int MANAGER_ERROR_CODE_TAG = 4;
const int WORKER_MSG_TAG = 5;
const int WORKER_ERROR_CODE_TAG = 6;

///// Master signals /////
// Terminate Manager
const int TERMINATE_MANAGER_SIGNAL = 0;
// Flush Worker
const int FLUSH_WORKER_SIGNAL = 1;

///// Manager signals /////
// Worker is cancelled
const int WORKER_FLUSHED_SIGNAL = 0;

///// Manager to Worker signals /////
// Terminate worker
const int TERMINATE_WORKER_SIGNAL = 0;

#endif // MPI_COMMON_H
