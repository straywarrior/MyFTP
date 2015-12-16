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
enum class FTPCMD{
    // Access Control Commands
    USER, PASS, ACCT, CWD, CDUP, SMNT, REIN, QUIT,
    // Transfer Paramater Commands
    PORT, PASV, TYPE, STRU, MODE,
    // FTP Service Commands
    RETR, STOR, STOU, APPE, ALLO, REST, RNFR, RNTO,
    ABOR, DELE, RMD, MKD, PWD, LIST, NLST, SITE,
    SYST, STAT, HELP, NOOP,
    // Commands not in RFC 959
    SIZE,
    // Unknown Commands
    UNKNOWN, ERROR, UNIMPL
};

FTPCMD read_command(int ctl_conn, char * arg_buf); 
FTPCMD parse_command(const char * read_buf, char * arg_buf);

int split_arg(const char * cmd_buf, char * arg_bug);

/*
 * Control Connection
 */

#define prepare_reply(send_buf, reply_fmt, ...) sprintf(send_buf, reply_fmt, __VA_ARGS__)
int send_reply(int ctl_conn, const char * send_buf);
int send_reply(int ctl_conn, const char * send_buf, int len);

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
