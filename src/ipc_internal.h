#ifndef __IPC_INTERNAL_H__
#define __IPC_INTERNAL_H__
#include "murmur.h"

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"

#define hash32 MurmurHash3_x86_32
#define SEED ('S' << 24 | 'T' << 16 | 'A' << 8 | 'R')

#define RQST_FLAG_ONEWAY (0x1<<24)
#define RQST_FLAG_RAWCMD (0x2<<24)

#endif