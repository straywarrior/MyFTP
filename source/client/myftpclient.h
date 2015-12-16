/*
 * myftpclient.h
 * @Author:     StrayWarrior
 * @Date:       2015-12-16
 * @Email:      i@straywarrior.com
 * Copyright (C) 2015 StrayWarrior
 *
 */

#ifndef _MYFTP_CLIENT_H_
#define _MYFTP_CLIENT_H_ value

#include "../common/common.h"

/*
 * The config structure
 */
typedef struct myftpclient_t{
    unsigned int port;
    unsigned int ipv4addr;
    char user[MAX_USER_NAME];
}myftpclient_t;



#endif /* !_MYFTP_CLIENT_H_ */
