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
    // Unknown Commands
    UNKNOWN, ERROR
};

FTPCMD read_command(int connection, char * arg_buf); 
FTPCMD parse_command(char * read_buf, char * arg_buf);
int split_arg(const char * cmd_buf, char * arg_bug);



/*
 * Control Connection
 */

#define prepare_reply(send_buf, reply_fmt, ...) sprintf(send_buf, reply_fmt, __VA_ARGS__)
int send_reply(int connection, const char * send_buf, int len);
int send_help(int connection);

/*
 * Data Connection
 */
int open_data_connection(int connection, unsigned int v4addr, unsigned int port);
int close_data_connection(int connection, int data_conn);

#endif /* !_MYFTP_CONNECTIONS_H_ */
