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
 */





/*
 * Control Connection
 */

#define prepare_reply(send_buf, reply_fmt, ...) sprintf(send_buf, reply_fmt, __VA_ARGS__)
int send_reply(int connection, const char * send_buf, int len);

/*
 * Data Connection
 */


#endif /* !_MYFTP_CONNECTIONS_H_ */
