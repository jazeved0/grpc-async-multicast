# grpc-async-multicast

Demonstrates a performance bug I am running into on a project. Serves as a MWE.

Async multicast code is based on: https://groups.google.com/g/grpc-io/c/4MVVoSb0uZ0.

## Running

```
$ make
...
$ ./run.sh
Server listening on 0.0.0.0:50544
Server listening on 0.0.0.0:50543
Server listening on 0.0.0.0:50546
Server listening on 0.0.0.0:50545
Starting ping multicast
Server at 0.0.0.0:50544 responded in 3ms
Server at 0.0.0.0:50545 responded in 3ms
Server at 0.0.0.0:50543 responded in 3ms
Server at 0.0.0.0:50546 responded in 3ms
Starting ping multicast
Server at 0.0.0.0:50543 responded in 1ms
Server at 0.0.0.0:50544 responded in 1ms
Server at 0.0.0.0:50545 responded in 1ms
Server at 0.0.0.0:50546 responded in 2ms
Starting ping multicast
Server at 0.0.0.0:50543 responded in 0ms
Server at 0.0.0.0:50544 responded in 0ms
Server at 0.0.0.0:50546 failed to respond after 1001ms
Server at 0.0.0.0:50545 failed to respond after 1002ms
Starting ping multicast
Server at 0.0.0.0:50543 responded in 0ms
Server at 0.0.0.0:50544 responded in 0ms
Server at 0.0.0.0:50546 failed to respond after 1001ms
Server at 0.0.0.0:50545 failed to respond after 1001ms
```

**The multicast ping starts exceeding its deadline after 1-2 rounds even though the servers are all local.**

When increasing the deadline (by manually increasing `DEADLINE_SEC` in `client.cpp` to 100), the actual time to respond varies, though it seems to cap out at 4 seconds:

```
Server listening on 0.0.0.0:50545
Server listening on 0.0.0.0:50544
Server listening on 0.0.0.0:50543
Server listening on 0.0.0.0:50546
Starting ping multicast
Server at 0.0.0.0:50543 responded in 3ms
Server at 0.0.0.0:50544 responded in 3ms
Server at 0.0.0.0:50546 responded in 3ms
Server at 0.0.0.0:50545 responded in 3ms
Starting ping multicast
Server at 0.0.0.0:50543 responded in 0ms
Server at 0.0.0.0:50544 responded in 0ms
Server at 0.0.0.0:50545 responded in 1ms
Server at 0.0.0.0:50546 responded in 1ms
Starting ping multicast
Server at 0.0.0.0:50543 responded in 0ms
Server at 0.0.0.0:50544 responded in 0ms
Server at 0.0.0.0:50545 responded in 1993ms
Server at 0.0.0.0:50546 responded in 1994ms
Starting ping multicast
Server at 0.0.0.0:50543 responded in 0ms
Server at 0.0.0.0:50544 responded in 0ms
Server at 0.0.0.0:50545 responded in 3997ms
Server at 0.0.0.0:50546 responded in 3997ms
```
