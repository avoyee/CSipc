## client-server mode ipc using unix socket 
----
### build
> + make clean .
> + make or give a CC like: make CC=gcc .
>>      default CC=arm-linux-gnueabihf-gcc.

> #### Directory `build/include` and `build/lib` will be produced which store release header and libararies.

### sample
> #### `main` compiled from `main.c` gives a simple demo of using ipc apis.

> ### usage:
>> + -s[d] start ipc server[in daemon mode] 
>> + -c    start ipc client     
----