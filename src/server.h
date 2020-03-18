/**
 * @file server.h
 * @author yongliang.li (yongliang_li_ms@163.com)
 * @brief 
 * @version 0.1
 * @date 2020-03-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __SERVER_H__
#define __SERVER_H__
#include "ipc.h"
#ifdef __cplusplus
extern 'c'
{
#endif
/**
 * @brief type of handler for funtions registered to IPC server
 * 
 */
typedef int (*REQEST_HANDLER)(Request rqst,void *rqstData, int rqstLen, void *resultData, int *resultLen);
/**
 * @brief Start specific IPC server 
 * 
 * @param svrName : Name of server 
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int SVRConnect(char *svrName);
/**
 * @brief Stop specific IPC server 
 * 
 * @param svrName 
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int SVRDisonnect(char *svrName);
/**
 * @brief Register a REQEST_HANDLER for specific Request
 * 
 * @param svrName : Name of server 
 * @param rqst : Request to register
 * @param rqstHandler : REQEST_HANDLER bound for Request
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int SVRResgisterRequest(char *svrName, Request rqst, REQEST_HANDLER rqstHandler);
/**
 * @brief Unregister a REQEST_HANDLER for specific Request
 * 
 * @param svrName : Name of server 
 * @param rqst : Request to unregister
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int SVRUnresgisterRequest(char *svrName, Request rqst);
#ifdef __cplusplus
}
#endif
#endif
