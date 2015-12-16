/*
 * connection.h
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#ifndef _MYFTP_CONNECTIONS_H_
#define _MYFTP_CONNECTIONS_H_ value

/*
 * Protocol Interpreter
 * Note: Need C++11
 */

/*
 * Control Connection
 */

int send_help(int ctl_conn);

/*
 * Data Connection
 */
int open_data_connection(int ctl_conn, unsigned int v4addr, unsigned int port);
int close_data_connection(int ctl_conn, int data_conn);

#endif /* !_MYFTP_CONNECTIONS_H_ */
