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

int worker_run(int connection, struct myftpserver_t * server_t);

int send_reply(int connection, const char * send_buf, int len);

#endif /* !_MYFTP_WORKER_H_ */
