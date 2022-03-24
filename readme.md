# What to run

## Running the webserver
Navigate to the directory containing mtwwwwd.c and compile it
```
gcc mtwwwd.c -o server -lpthread
```
The `-lpthread` argument isn't always necessary, but it's a good safety measure so that the linker can actually find the symbols necessary for `pthread.h` in the pthread library during the linking \

Once the program is compiled, start the server and provide path, port, threadcount, and buffersize
```
./server www-path port #threads #bufferslots
```
Example way of running the server:
```
./server ./doc 8080 16 128
```

### Connecting to the server

#### telnet
Since the server is running HTTP/0.9, using a browser does not work, as the browser expects a HTTP/1.1 response.
`telnet` can be used instead then.
Assuming the server is running on port 8080, you can establish a keep-alive connection to the server:
```
telnet localhost 8080
``` 
Request a file on the format `<HTTPMETHOD> <PATH>`:
```
GET /index.html
```
Note: All extra headers will be ignored.
#### wget
You can also attempt to download one of the files using `wget`, but be aware that it will try reconnect endlessly until you force exit if you specify a wrong path.
```
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


### e) Extra Credit:
As explained by the task e infotext, the webserver in its current state has security issues as the requesting client freely set the path to file they want to retrieve. This creates a security breach as a user can exit the subdirectory that contains
the files that are intended to be public and therefore perform a Local File Intrusion (LFI) attack, and access all files that the running process has access to . The intruder is limited to read-access, but this is still very harmful as the intruder can access sensitive info such as password, hashes, or private keys (like an ssh-rsa key).

**The first solution** is to search through the client given path for substrings that contain “ ../” as this
gives them the ability to exit the current directory (“./doc”). If the server notices that the client has included this in the requested filepath, the program can refuse the request and close the connection. Optimally the server can respond with a 403 Forbidden.

**The second solution** is to use the chmod() function which is imported from <sys/stat.h>. The chmod()
function uses two inputs; the pathname and a mode which specifies the permission case . In mode you
can restrict which directories the path has access to. If the pathname were to breach the
mode restrictions the function returns -1 and the program can stop the client from continuing until they
give an allowed path. 