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
int get_cur_path(myftpserver_worker_t * worker_t, char * result){
    result[0] = '\0';
    strcat(result, "/");
    strcat(result, worker_t->reladir);
    return 0;
}
int get_absolute_path(myftpserver_worker_t * worker_t, char * result){
    result[0] = '\0';
    strcat(result, worker_t->rootdir);
    strcat(result, "/");
    strcat(result, worker_t->reladir);
    return 0;
}
int list_dir(myftpserver_worker_t * worker_t, char * result);
int open_dir(char * path);
int close_working_dir(myftpserver_worker_t * worker_t);


