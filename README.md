# Node event loop lockup with windows named pipes

- Node.js issue: https://github.com/nodejs/node/issues/58135
- Similar libuv issue: https://github.com/libuv/libuv/issues/4738

## Description of the bug

When reading from a child process, and the child writes exactly 65536 bytes, the event loop locks up.

It is important to note that the whole event loop is locked up, i.e. timers no longer run, etc.

The node main thread hangs in: 
```
NtWaitForSingleObject()
WaitForSingleObjectEx()
GetOverlappedResult()
uv__pipe_read_data(uv_loop_s * loop, uv_pipe_s * handle, unsigned long * bytes_read, unsigned long max_bytes) L2008
uv__process_pipe_read_req(uv_loop_s * loop, uv_pipe_s * handle, uv_req_s * req) L2162
uv__process_reqs(uv_loop_s *) L159
uv_run(uv_loop_s * loop, uv_run_mode mode) L639
node::SpinEventLoopInternal(node::Environment * env) L42
node::NodeMainInstance::Run(node::ExitCode *) L110
node::NodeMainInstance::Run() L100
node::StartInternal(int argc, char * * argv) L1540
node::Start(int argc, char * * argv) L1548
wmain(int argc, wchar_t * * wargv) L91
```

### Important details

- When using the `readable` event, the next write must be scheduled via `setImmediate`. If calling `write_req` directly in the event handler, the lockup does not happen.
- When using the `data` event, instead of the `readable` event, the lockup does not happen.

### Using libuv directly

There's example code in `parent.c` that tries to be as similar as possible to `parent.mjs`, but fails to reproduce the hang.
I don't know enough about node to figure out what the remaining difference is...

### Steps to reproduce
```
cl test.c
node parent.mjs
```

### Expected output
```
P: req
C: req
C: res: 65536
P: res: 65536
C: req
P: req
C: res: 65536
P: res: 65536
C: req
P: req
C: res: 65536
P: res: 65536
P: req
```

### Acutal output
```
C: req
C: res: 65536
P: req
P: res: 65536
<hangs forever>
```
