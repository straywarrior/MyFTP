/*
 * fileoperation.h
 * @Author:     StrayWarrior
 * @Date:       2015-12-14
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#ifndef _FILEOPERATION_H_
#define _FILEOPERATION_H_ value

#include "myftpserver.h"
#include "worker.h"
#include <fcntl.h>
#include <dirent.h>

int init_working_dir(myftpserver_worker_t * worker_t);
int change_dir(myftpserver_worker_t * worker_t, const char * pathname);
int get_cur_path(myftpserver_worker_t * worker_t, char * result);
int list_dir(myftpserver_worker_t * worker_t);
int close_working_dir(myftpserver_worker_t * worker_t);

#endif /* !_FILEOPERATION_H_ */
