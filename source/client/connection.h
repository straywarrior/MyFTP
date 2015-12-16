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

int recv_reply(int ctl_conn, char * recv_buf);
int recv_and_show_reply(int ctl_conn);
int parse_reply(int ctl_conn, char * recv_buf);

/*
 * Data Connection
 */
int open_data_connection(int ctl_conn, struct sockaddr_in * data_sock_addr, unsigned int v4addr, unsigned int port);
int close_data_connection(int ctl_conn, int data_conn);
int open_and_send_data_port(int ctl_conn, struct sockaddr_in * data_sock_addr, unsigned int v4addr = 0);
int recv_and_show_data(int data_conn, struct sockaddr_in * data_sock_addr);

int store_file(int ctl_conn, int data_conn, struct sockaddr_in * data_sock_addr, const char * filename);
int retrieve_file(int ctl_conn, int data_conn, struct sockaddr_in * data_sock_addr, const char * filename);

#endif /* !_MYFTP_CONNECTIONS_H_ */
