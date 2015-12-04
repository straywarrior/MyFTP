/*
 * worker.h
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#ifndef _MYFTP_WORKER_H_
#define _MYFTP_WORKER_H_ value

#include "myftpserver.h"

typedef struct myftpserver_worker_t{
    myftpserver_t * server;
    int connection;
    char rootdir[MAX_PATH_LEN];     // Root dir. User cannot escape out of the dir!!
    char rela_path[MAX_PATH_LEN];   // Relative path to rootdir
}myftpserver_worker_t;

int worker_run(myftpserver_worker_t * worker_t);


#endif /* !_MYFTP_WORKER_H_ */
