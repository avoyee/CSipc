## client-server mode ipc using unix socket 
----
### build
> + make clean .
> + make or make CC=gcc.
>>      default CC=arm-linux-gnueabihf-gcc.

> ### directory build/include and build/lib will be produced which store release header and libararies.

> ### main compiled from main.c gives a simple demo of using ipc apis.

> ### dusage:
>> + -s[d] start ipc server[in daemon mode] 
>> + -c    start ipc client     
----