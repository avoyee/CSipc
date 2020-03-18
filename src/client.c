#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>

#include "ipc_internal.h"
#include "client.h"

#ifdef __cplusplus
extern 'c'
{
#endif
	static int fd = 0;
	static char c_socketFile[256] = {0};

	int getSvrSocketFilePath(char *svrName, char *path, int len)
	{
		char *svrSocketFileaddr;
		int key;
		int fd;
		int shmid = 0;
		hash32(svrName, strlen(svrName), SEED, &key);
		shmid = shmget(key, 0, 0666);
		if (shmid < 0)
		{
			perror("shmget");
			return -1;
		}
		svrSocketFileaddr = shmat(shmid, 0, 0);

		if (svrSocketFileaddr == (void *)-1)
		{
			perror("shmat");
			return -1;
		}
		if (strlen(svrSocketFileaddr) > len)
		{
			fprintf(stderr, "svrSocketFile too short\n");
			return -1;
		}
		else
		{
			strcpy(path, svrSocketFileaddr);
		}
		shmdt(svrSocketFileaddr);
		return 0;
	}

	int ClientConnect(char *clientName, char *svrName)
	{
		struct sockaddr_un addr;
		char svrSocketFile[256] = {0};
		pid_t pid = getpid();
		pid_t tid = pthread_self();

		if (clientName != NULL)
		{
			if (strlen(clientName) >= sizeof(c_socketFile))
			{
				fprintf(stderr, "clientSocketFile too long\n");
				return -1;
			}
			else
			{
				sprintf(c_socketFile, "/tmp/%s.%d.%u", clientName, pid, tid);
			}
		}
		else
		{
			snprintf(c_socketFile, sizeof(c_socketFile), "/tmp/%s.%d.%u", CLIENT_SOCK_FILE, pid, tid);
		}
		getSvrSocketFilePath(svrName, svrSocketFile, sizeof(svrSocketFile));
		if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)
		{
			perror("socket");
			return -1;
		}
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, c_socketFile);
		unlink(c_socketFile);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			perror("bind");
			return -1;
		}
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		if (svrSocketFile[0] == '\0')
		{
			strcpy(addr.sun_path, SERVER_SOCK_FILE);
		}
		else
		{
			memcpy(addr.sun_path, svrSocketFile, strlen(svrSocketFile));
		}
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		{
			perror("connect");
			return -1;
		}
		return 0;
	}
	int ClientRequest(char *clientName, Request rqst, void *msg, int msgLen, void *rst, int *rstLen)
	{
		int ret;
		int retLen;
		char *result;
		if ((rqst >> 24) == 0)
		{
			if (msg == NULL || msgLen == 0)
			{
				rqst |= RQST_FLAG_RAWCMD;
			}
			if (rst == NULL || rstLen == NULL)
			{
				rqst |= RQST_FLAG_ONEWAY;
			}
		}
		if (send(fd, &rqst, sizeof(rqst), 0) == -1)
		{
			perror("send request");
			return -1;
		}

		if (!(rqst & RQST_FLAG_RAWCMD) && (retLen = send(fd, msg, msgLen, 0)) == -1)
		{
			perror("send msg");
			return -1;
		}

		if (rqst & RQST_FLAG_ONEWAY)
		{
			result = malloc(sizeof(ret));
		}
		else
		{
			result = malloc(*rstLen + sizeof(ret));
		}
		if ((retLen = recv(fd, result, 8192, 0)) < 0)
		{
			perror("recv");
			return -1;
		}
		memcpy(&ret, result, sizeof(ret));
		if (!(rqst & RQST_FLAG_ONEWAY))
		{
			*rstLen = retLen - sizeof(ret);
			memcpy(rst, result + sizeof(ret), *rstLen);
		}
		free(result);
		return ret;
	}
	int ClientDisconnect(char *clientName)
	{
		if (fd >= 0)
		{
			close(fd);
		}

		unlink(c_socketFile);
	}
#ifdef __cplusplus
}
#endif