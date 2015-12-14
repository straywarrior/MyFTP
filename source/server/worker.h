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

struct myftpserver_worker_t{
    myftpserver_t * server;
    int connection;
    char rootdir[MAX_PATH_LEN];     // Root dir. User cannot escape out of the dir!!
    int rootdirfd;
    char reladir[MAX_PATH_LEN];   // Relative path to rootdir
    char curdir[MAX_PATH_LEN];    // Complete path.
    int curdirfd;
    char username[MAX_USER_NAME];

    unsigned int data_v4addr;     // IPv4 Address of Data Connection
    unsigned int data_port;       // Port of Data Connection
    ~myftpserver_worker_t();
};

int worker_run(myftpserver_worker_t * worker_t);

#include "fileoperation.h"

#endif /* !_MYFTP_WORKER_H_ */
