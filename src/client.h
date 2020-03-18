/**
 * @file client.h
 * @author yongliang.li (yongliang_li_ms@163.com)
 * @brief 
 * @version 0.1
 * @date 2020-03-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__
#include "ipc.h"
#ifdef __cplusplus
extern 'c'
{
#endif
/**
 * @brief Connect to specify Request handling server
 * 
 * @param clientName : Name of client
 * @param svrName : Name of Reuqest server
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int ClientConnect(char *clientName, char *svrName);
/**
 * @brief Disconnect from specify Request handling server
 * 
 * @param clientName : Name of client
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int ClientDisconnect(char *clientName);
/**
 * @brief Send request to Request handling server connected by clientName
 *  
 * @param clientName : Name of client
 * @param rqst : Request to send
 * @param msg : pointer to data needed by rqst.NOTE: NULL is valid
 * @param msgLen : data length of msg.NOTE: 0 is valid
 * @param rst : pointer to data for storing result of request:NOTE: NULL is valid
 * @param rstLen pointer to data length of result.NOTE: NULL is valid
 * @return int : 
 *          0:success
 *         <0：fail @see IPC_ERRORCODE
 */
int ClientRequest(char *clientName, Request rqst, void *msg, int msgLen, void *rst, int *rstLen);
#ifdef __cplusplus
}
#endif
#endif