/**
 * @file   login.h
 * @author mathslinux <riegamaths@gmail.com>
 * @date   Sun May 20 02:25:51 2012
 *
 * @brief  Linux WebQQ Login API
 *
 *
 */

#ifndef LWQQ_LOGIN_H
#define LWQQ_LOGIN_H

#include "type.h"

typedef struct LwqqAsyncEvent LwqqAsyncEvent;

/**
 * WebQQ login function
 *
 * @param client Lwqq Client
 * @param err Error code
 */
void lwqq_login(LwqqClient* client, LwqqStatus status, LwqqErrorCode* err);

LwqqAsyncEvent* lwqq_relink(LwqqClient* lc);

/**
 * WebQQ logout function
 *
 * @param client Lwqq Client
 * @param wait_time block wait to quit until wait_time
 */
LwqqErrorCode lwqq_logout(LwqqClient* client, unsigned wait_time);

LwqqAsyncEvent* lwqq_get_version(LwqqClient* lc, LwqqErrorCode* err);
#endif /* LWQQ_LOGIN_H */

