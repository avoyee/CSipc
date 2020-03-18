#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/shm.h>
#include <pthread.h>

#include "server.h"
#include "ipc_internal.h"
#ifdef __cplusplus
extern 'c'
{
#endif
#define REQUEST_SIZE 8192
#define MAP_REQUEST(rqst) ((rqst - RQST_MAGIC)&0xFF)

	static pthread_t work_thread;
	static pthread_mutex_t requestlock;
	static REQEST_HANDLER request_handler_map[256];

	static unsigned char isRequestValid(Request rqst)
	{
		if ((rqst ^ RQST_MAGIC)&0xFFFF00)
			return 0;
		return 1;
	}

	int SVRResgisterRequest(char *svrName, Request rqst, REQEST_HANDLER rqstHandler)
	{
		if (!isRequestValid(rqst))
		{
			return IPC_ERR_INVALID_REQUEST;
		}
		pthread_mutex_lock(&requestlock);
		if (request_handler_map[MAP_REQUEST(rqst)] != NULL)
		{
			pthread_mutex_unlock(&requestlock);
			return IPC_ERR_HANDLER_EXISTS;
		}
		request_handler_map[MAP_REQUEST(rqst)] = rqstHandler;
		pthread_mutex_unlock(&requestlock);
		return 0;
	}
	int SVRUnresgisterRequest(char *svrName, Request rqst)
	{
		if (!isRequestValid(rqst))
		{
			return IPC_ERR_INVALID_REQUEST;
		}
		pthread_mutex_lock(&requestlock);
		request_handler_map[MAP_REQUEST(rqst)] = NULL;
		pthread_mutex_unlock(&requestlock);
		return 0;
	}

	void *request_handler(void *args)
	{
		struct sockaddr_un from;
		socklen_t fromlen = sizeof(from);
		int len = 0;
		int fd = *((int *)args);
		char buff[REQUEST_SIZE];
		while (((len = recvfrom(fd, buff, REQUEST_SIZE, 0, (struct sockaddr *)&from, &fromlen)) > 0))
		{
			Request rqst;
			char result[REQUEST_SIZE / 2];
			int rstLen = 0;
			int ret = -1;
			memcpy(&rqst, buff, sizeof(Request));
			pthread_mutex_lock(&requestlock);
			if (!isRequestValid(rqst))
			{
				ret = IPC_ERR_INVALID_REQUEST;
			}
			else if(request_handler_map[MAP_REQUEST(rqst)] == NULL)
			{
				ret = IPC_ERR_NO_REQUEST_HANDLER;
			}
			else
			{
				char* rstPtr = result;
				char* buffPtr = NULL;
				if (!(rqst & RQST_FLAG_RAWCMD))
				{
					if((len = recv(fd, buff, REQUEST_SIZE, 0))>0)
					{
						buffPtr = buff;
					}
					
				}
				if (rqst & RQST_FLAG_ONEWAY)
				{
					rstPtr = NULL;
				}
				else
				{
					rstPtr +=sizeof(ret); 
				}
				
				ret = (*(request_handler_map[MAP_REQUEST(rqst)]))(rqst&(((Request)-1)>>8),buffPtr, len, rstPtr, &rstLen);
			}
			pthread_mutex_unlock(&requestlock);
			memcpy(result, &ret,sizeof(ret));
			ret = sendto(fd, result, rstLen + sizeof(ret), 0, (struct sockaddr *)&from, fromlen);
			if (ret < 0)
			{
				perror("sendto");
				//break;
			}
		}
	}
	int SVRConnect(char *svrName)
	{
		char *svrSocketFileaddr;
		struct sockaddr_un addr;
		int key;
		int fd;
		int shmid = 0;
		hash32(svrName, strlen(svrName), SEED, &key);
		shmid = shmget(key, 0, 0666);

		if (shmid < 0)
		{
			shmid = shmget(key, 256, IPC_CREAT | IPC_EXCL | 0666);
			if (shmid < 0)
			{
				perror("parent shmget");
				return -1;
			}
		}
		svrSocketFileaddr = shmat(shmid, 0, 0);
		if (svrSocketFileaddr == (void *)-1)
		{
			perror("shmat");
			return -1;
		}

		pid_t pid = getpid();
		pid_t tid = pthread_self();

		snprintf(svrSocketFileaddr, 256, "/tmp/%s.%d.%u", svrName, pid, tid);

		if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)
		{
			perror("socket");
			return -1;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, svrSocketFileaddr);
		unlink(svrSocketFileaddr);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			perror("bind");
			return -1;
		}

		memset(request_handler_map, 0, sizeof(request_handler_map));
		pthread_mutex_init(&requestlock, NULL);
		if (0 != pthread_create(&work_thread, NULL, request_handler, &fd))
		{
			perror("pthread_create\n");
			return -1;
		}
	}

	int SVRDisonnect(char *svrName)
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
		shmdt(svrSocketFileaddr);
		shmctl(shmid, IPC_RMID, NULL);
		if (fd >= 0)
		{
			close(fd);
		}
		return 0;
	}
#if 0
int main()
{
	int fd;
	struct sockaddr_un addr;
	int ret;
	char buff[8192];
	struct sockaddr_un from;
	int ok = 1;
	int len;
	socklen_t fromlen = sizeof(from);

	if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		ok = 0;
	}

	if (ok)
	{
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, SERVER_SOCK_FILE);
		unlink(SERVER_SOCK_FILE);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			perror("bind");
			ok = 0;
		}
	}

	while ((len = recvfrom(fd, buff, 8192, 0, (struct sockaddr *)&from, &fromlen)) > 0)
	{

		ipcRequest rqst;
		memcpy(&rqst, buff, sizeof(ipcRequest));
		printf("recv request %d %d\n", rqst.request, len);
		for (int i = 0; i < len; i++)
		{
			printf("%x ", buff[i]);
		}
		printf("\n");
		if (rqst.request == RQST_dummpy0)
		{
			//struct_dummpy0 msg ;//= *((struct_dummpy0*)(buff+sizeof(ipcRequestIdx)));

			printf("%d\n", rqst.msg0.a);
			printf("%f\n", rqst.msg0.b);
			printf("%s\n", rqst.msg0.c);
		}
		strcpy(buff, "transmit good!");
		ret = sendto(fd, buff, strlen(buff) + 1, 0, (struct sockaddr *)&from, fromlen);
		if (ret < 0)
		{
			perror("sendto");
			break;
		}
	}

	if (fd >= 0)
	{
		close(fd);
	}

	return 0;
}

#endif
#ifdef __cplusplus
}
#endif