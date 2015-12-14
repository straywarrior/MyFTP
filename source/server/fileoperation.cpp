/*
 * fileoperation.cpp
 * @Author:     StrayWarrior
 * @Date:       2015-12-14
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#include "fileoperation.h"

int init_working_dir(myftpserver_worker_t * worker_t){
    return 0;
}

int change_dir(myftpserver_worker_t * worker_t, const char * pathname);
int list_dir(myftpserver_worker_t * worker_t, char * result);
int close_working_dir(myftpserver_worker_t * worker_t);


