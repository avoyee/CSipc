#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include "server.h"
#include "client.h"

typedef enum
{
    RQST_dummpy0 = RQST_START,
    RQST_dummpy1,
    RQST_dummpy2,
    RQST_dummpyMAX = RQST_MAX,
} MyRQST;

typedef struct
{
    char str[256];
    int num;

} B;
typedef struct
{
    char str[256];
    int num;
    B b;
} A;

int rqst_handlerDummy0(Request rqst, void *rqstData, int rqstLen, void *resultData, int *resultLen)
{
    printf("%s %d\n", __FUNCTION__, __LINE__);
    return -1;
}
int rqst_handlerDummy1(Request rqst, void *rqstData, int rqstLen, void *resultData, int *resultLen)
{
    printf("%s %d\n", __FUNCTION__, __LINE__);
    A a;
    B b;
    memcpy(&a, rqstData, rqstLen);
    strcpy(b.str, a.str);
    b.num = a.b.num;
    *resultLen = sizeof(B);
    memcpy(resultData, &b, *resultLen);
    return 0;
}

int rqst_handlerDummy2(Request rqst, void *rqstData, int rqstLen, void *resultData, int *resultLen)
{
    printf("%s %d\n", __FUNCTION__, __LINE__);
    strcpy(resultData, "rqst_handlerDummy2 return");
    *resultLen = strlen("rqst_handlerDummy2 return");
    return 0;
}

void usage(void)
{
    printf("\n \
            \t -s[d] start ipc server [in daemon mode] \n\
            \t -c    start ipc client    \n\
            \n");
}
int main(int argc, char **argv)
{
    int sflag = 0;
    int cflag = 0;
    int dflag = 0;
    int c;

    if(argc <2)
    {
        usage();
        return 0;
    }
    while ((c = getopt(argc, argv, "s::ch")) != -1)
        switch (c)
        {
        case 's':
            sflag = 1;
            if (optarg!=NULL&&strncasecmp(optarg, "d", 1) == 0)
            {
                dflag = 1;
            }
            break;
        case 'c':
            cflag = 1;
            break;
        default:
            usage();
            return 0;
        }

    if (sflag)
    {
        if (dflag)
        {
            daemon(1, 1);
        }
        SVRConnect("serverName");
        SVRResgisterRequest("serverName", RQST_dummpy0, rqst_handlerDummy0);
        SVRResgisterRequest("serverName", RQST_dummpy1, rqst_handlerDummy1);
        SVRResgisterRequest("serverName", RQST_dummpy2, rqst_handlerDummy2);
        //SVRUnresgisterRequest("serverName", RQST_dummpy0);
        while (1)
        {
            sleep(10);
        }
        //SVRDisonnect("serverName");
    }
    if (cflag)
    {
        int len = 0;
        ClientConnect("clientName", "serverName");

        printf("RQST_dummpy0 ret: %d\n", ClientRequest("clientName", RQST_dummpy0, NULL, 0, NULL, NULL));

        A a = {0};
        B b = {0};
        strcpy(a.str, "rqst RQST_dummpy1");
        a.b.num = 1;
        len = sizeof(B);
        printf("RQST_dummpy1 ret: %d\n", ClientRequest("clientName", RQST_dummpy1, &a, sizeof(A), &b, &len));
        printf("%s %d %s %d\n", __FUNCTION__, __LINE__, b.str, b.num);

        char rst[256] = {0};
        len = 256;
        printf("RQST_dummpy2 ret: %d\n", ClientRequest("clientName", RQST_dummpy2, "test", strlen("test"), rst, &len));
        printf("%s %d %s\n", __FUNCTION__, __LINE__, rst);

        ClientDisconnect("clientName");
    }

    return 0;
}
