/*
 * myftpserver.h
 * @Author:     StrayWarrior
 * @Date:       2015-12-02
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#ifndef _MYFTP_SERVER_H_
#define _MYFTP_SERVER_H_ value

/*
 * C header
 */
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

/*
 * C++ STL header
 * C++ 11 is needed
 */

/*
 * My header
 */

/*
 * Server Constants Definition
 */
#define DEFAULT_IPV4_ADDR   INADDR_ANY    // Selected IP by System
#define DEFAULT_PORT        21
#define RETRY_TIME          1             // Retry after 1 minutes.
#define EOL                 "\r\n"

#define MAX_PATH_LEN        256           // Max length of working path
#define MAX_READ_BUF        1024
#define MAX_USER_NAME       32
#define MAX_USER_PASS       32

/*
 * The config structure
 */
typedef struct myftpserver_t{
    unsigned int port;
    unsigned int ipv4addr;
    unsigned int max_conns;
    //TODO: add ipv6 support?
    bool allow_anonymous;
}myftpserver_t;

/*
 * Define reply codes
 */
#define REPCODE_110 "110 Restart marker reply.\r\n"
#define REPCODE_120 "120 Try again in " ## RETRY_TIME ##" minutes.\r\n"
#define REPCODE_125 "125 Data connection already open; transfer starting.\r\n"
#define REPCODE_150 "150 File status okay; about to open data connection.\r\n"

#define REPCODE_200 "200 Command okay.\r\n"
#define REPCODE_202 "202 Command not implemented, superfluous at this site.\r\n"
#define REPCODE_211 "221 System status, or system help reply.\r\n"
#define REPCODE_212 "212 Directory status.\r\n"
#define REPCODE_213 "213 File status.\r\n"
#define REPCODE_214 "214 Help message.\r\n"
#define REPCODE_215 "215 %s system type.\r\n"
#define REPCODE_220 "220 Service ready for new user.\r\n"
#define REPCODE_221 "221 Service closing control connection.\r\n"
#define REPCODE_225 "225 Data connection open; no transfer in progress.\r\n"
#define REPCODE_226 "226 Closing data connection.\r\n"
#define REPCODE_227 "227 Entering Passive Mode (%s,%s,%s,%s,%s,%s).\r\n"
#define REPCODE_230 "230 User logged in, proceed.\r\n"
#define REPCODE_250 "250 Requested file action okay, completed.\r\n"
#define REPCODE_257 "257 "%s" created.\r\n"

#define REPCODE_331 "331 User name okay, need password.\r\n"
#define REPCODE_332 "332 Need account for login.\r\n"
#define REPCODE_350 "350 Requested file action pending further information.\r\n"

#define REPCODE_421 "421 Service not available, closing control connection.\r\n"
#define REPCODE_425 "425 Can't open data connection.\r\n"
#define REPCODE_426 "426 Connection closed; transfer aborted.\r\n"
#define REPCODE_450 "450 Requested file action not taken.\r\n"
#define REPCODE_451 "451 Requested action aborted: local error in processing.\r\n"
#define REPCODE_452 "452 Requested action not taken.\r\n"

#define REPCODE_500 "500 Syntax error, command unrecognized.\r\n"
#define REPCODE_501 "501 Syntax error in parameters or arguments.\r\n"
#define REPCODE_502 "502 Command not implemented.\r\n"
#define REPCODE_503 "503 Bad sequence of commands.\r\n"
#define REPCODE_504 "504 Command not implemented for that parameter.\r\n"
#define REPCODE_530 "530 Not logged in.\r\n"
#define REPCODE_532 "532 Need account for storing files.\r\n"
#define REPCODE_550 "550 Requested action not taken.\r\n"
#define REPCODE_551 "551 Requested action aborted: page type unknown.\r\n"
#define REPCODE_552 "552 Requested file action aborted.\r\n"
#define REPCODE_553 "553 Requested action not taken.\r\n"

/*
 * Log Definition
 * FIXME: Real level needs to be added
 * 原谅我ffmpeg和x264看得太多对此类日志系统中毒太深...
 */
#define SERVER_LOG_FATAL    0
#define SERVER_LOG_ERROR    1
#define SERVER_LOG_WARNING  2
#define SERVER_LOG_INFO     3
#define SERVER_LOG_DEBUG    4
#define server_log(level, fmt...) fprintf(stderr, fmt)

/*
 * Main Functions Definition
 */
int start_server(myftpserver_t * server_t);

#endif
