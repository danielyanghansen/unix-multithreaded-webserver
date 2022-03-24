# What to run

## Running the webserver
Navigate to the directory containing mtwwwd.c and compile it
```
gcc mtwwwd.c -o server -lpthread
```
The `-lpthread` argument isn't always necessary, but it's a good safety measure so that the linker can actually find the symbols necessary for `pthread.h` in the pthread library during the linking \

Once the program is compiled, start the server and provide path, port, threadcount, and buffersize
```bash
./server www-path port #threads #bufferslots
```
Example way of running the server:
```bash
./server ./doc 8080 16 128
```

### Connecting to the server

#### telnet
Since the server is running HTTP/0.9, using a browser does not work, as the browser expects a HTTP/1.1 response.
`telnet` can be used instead then.
Assuming the server is running on port 8080, you can establish a keep-alive connection to the server:
```bash
telnet localhost 8080
``` 
Request a file on the format `<HTTPMETHOD> <PATH>`:
```
GET /index.html
```
Note: All extra headers will be ignored.
#### wget
You can also attempt to download one of the files using `wget`, but be aware that it will try reconnect endlessly until you force exit if you specify a wrong path.
```bash
wget localhost:8080/index.html
```

## Other Testing
As specified in the task, the multithreaded webserver was made by implementing a semaphore and a ring buffer.

### Testing the Semaphore
This is a "visual test" in the terminal that runs until cancelled. Compile `semtest.c` if you want to run the test. To test that it works, threads sleep for a short while to simulate working.

### Testing the Ring Buffer
This is a short testing script to confirm that the ring buffer returns correct values and errors when items are added or removed from it. Compile `bbuffertest.c` to be able to run the test.

### Integration testing between Semaphore and ring buffer
A test using both the Semaphore and the Ring buffer. This is also a "visual test" in the terminal that runs until forcefully cancelled. After a consumer thread has removed a task from the ring buffer, or the producer thread has added a task to it, the respective thread will sleep for a few seconds to simulate working. For correct buffer size testing, the consumer threads sleep significantly longer than the lone producer thread. Compile `sembufferintegrationtest.c` to be able to run the test.



# e) Extra Credit:
As explained by the task e infotext, the webserver in its current state has security issues as the requesting client freely set the path to file they want to retrieve. This creates a security breach as a user can exit the subdirectory that contains
the files that are intended to be public and therefore perform a Local File Intrusion (LFI) attack, and access all files that the running process has access to . The intruder is limited to read-access, but this is still very harmful as the intruder can access sensitive info such as password, hashes, or private keys (like an ssh-rsa key).

**The first solution** is to search through the client given path for substrings that contain “ ../” as this
gives them the ability to exit the current directory (“./doc”). If the server notices that the client has included this in the requested filepath, the program can refuse the request and close the connection. Optimally the server can respond with a 403 Forbidden.

**The second solution** is to use the chmod() function which is imported from <sys/stat.h>. The chmod()
function uses two inputs; the pathname and a mode which specifies the permission case . In mode you
can restrict which directories the path has access to. If the pathname were to breach the
mode restrictions the function returns -1 and the program can stop the client from continuing until they
give an allowed path.

# A few notes

## Avoiding consumer thread race condition deadlock
Although the chance is small, it is technically possible for the consumer threads to arrive in a collective deadlock IFF the ring buffer is full AND all the consumer threads are waiting on the (same) condition signal.
To fix this, all threads wake up after 60 seconds of no signal to recheck the SEM counter value.

## Other "Nice to Have", unimplemented features
(This section also includes how we have facilitated the possibility of these potential implementations)

### Proper server shutdown.
Currently, the server runs until forcefully stopped by sending the interrupt SIGINT (using `^C` / Ctrl+C). Because the current implementation has an empty `while(1) {}` loop, the main thread stays alive, which is necessary as it allows the interrupt SIGINT to propogate to the generated child threads and cause a (somewhat) proper shutdown that frees resources. \
However, it would be better to be able to send a custom shutdown signal, which causes the server to:
- stop all new incoming requests
- log all unresolved requests
- shut down (and maybe complete) all currently processing requests
- properly perform garbage collection and free resources using our methods `sem_del(SEM* sem)` and `bb_del(BNDBUF *bb)`\

A small step to facilitate this has been done by letting most thread functions run in a `while(\*running) {}` loop. `running` is set to be 1 true at the beginning of the program, but the main thread has the possibility to set this to 0 if implemented correctly. This would cause the producer thread to stop taking new requests, and cause all consumer threads to stop queueing up for new tasks to consume. It would however not properly join/exit any of the threads sleeping/waiting for a signal, as the `running` variable does not curruntly propogate to the internal SEM functions.

### HTTP/1.1
The server is currently only answering in HTTP/0.9, which means the response cannot be accessed in a browser. Because we allow sending arbitrarily large files in packets of 4096 bytes at a time in a direct stream, there's currently no implementation of a method that checks the total bytesize of the file content before the file is sent. Hence, we have no way of declaring filesize in the header.\
