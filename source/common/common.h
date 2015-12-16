/*
 * common
 * @Author:     StrayWarrior
 * @Date:       2015-12-16
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_ value

/*
 * C header
 */
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#ifndef _WIN32
#include <errno.h>
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
 * Common constants
 */
#define MAX_PATH_LEN        256           // Max length of working path
#define MAX_READ_BUF        512
#define MAX_SEND_BUF        512

#define MAX_USER_NAME       32
#define MAX_USER_PASS       32

/*
 * Log Definition
 * FIXME: Real level needs to be added
 * 原谅我ffmpeg和x264看得太多对此类日志系统中毒太深...
 */
#define MYFTP_LOG_FATAL    0
#define MYFTP_LOG_ERROR    1
#define MYFTP_LOG_WARNING  2
#define MYFTP_LOG_INFO     3
#define MYFTP_LOG_DEBUG    4
#define myftp_log(level, fmt...) fprintf(stderr, fmt)

#endif /* !_COMMON_H_ */
