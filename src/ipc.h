/**
 * @file ipc.h
 * @author yongliang.li (yongliang_li_ms@163.com)
 * @brief 
 * @version 0.1
 * @date 2020-03-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __IPC_H__
#define __IPC_H__
#ifdef __cplusplus
extern 'c'
{
#endif

/**
 * NOTE: DO NOT MODIFY THIS.
 */
#define RQST_MAGIC ((('R' + 'Q') << 16) | (('S' + 'T') << 8))

/**
 * @brief start enum and max enum of Request allowed user define.
 * 
 */
typedef enum
{
    /// starting index of user-defined Request
    RQST_START = RQST_MAGIC,
    /// max index of user-defined Request
    RQST_MAX = RQST_MAGIC + 0xFF,
} Request;

typedef enum
{
    CLIETN_ERR = (0XFF<<16),
    IPC_ERR_CONNECT_SERVER_FAIL,
    IPC_ERR_CONNECTION_EXISTS,
    SERVER_ERR = (0XFF<<24),
    IPC_ERR_INVALID_REQUEST,
    IPC_ERR_HANDLER_EXISTS,
    IPC_ERR_NO_REQUEST_HANDLER,
}IPC_ERRORCODE;

#ifdef __cplusplus
}
#endif
#endif
