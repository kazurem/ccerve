# ccerve

An HTTP server written from scratch in C++.

## Project Status
As of yet, the project is still uncomplete and refined. Alot of the  features still need to be implemented
and bugs need to be ironed out.  These are my goals as of now:
- [ ]  Either Non-Blocking I/O or ThreadPool pattern for handling many multiple connections. I came across
[libevent](https://libevent.org/) which I found promising.
- [ ]  Tests using [Googletest](https://github.com/google/googletest).
- [ ]  Refine documentation (Read through the doxygen documentation and use proper syntax).
- [ ]  Feature for specifying directory which needs to be hosted. 
- [ ]  Serving HTML showing directory contents in case filename isn't specified in HTTP request.
- [ ]  Add support for more file types and extend the HTTP protocol.
- [ ] Use enums for file types, status codes etc.

### TODO for final phase:
- [ ] CONTRIBUTING.md file
- [ ]  Security file
- [ ] Code of Conduct file
- [ ] Maybe add documentation to Wiki?

## How to run:

First, get the copy of the repo into your local machine:  
```bash
git clone https://github.com/kazurem/ccerve
```
  
Then go into the main directory:
```bash
cd ccerve
```

Run the following command to configure the build directory (named 'build'):
```bash
meson setup build
```

Build the configured project
```bash
meson compile -C build
```

The server executable would now be in the _build_ directory.
```bash
#Format
./build/cerve ip_address port
```
If you run the executable without the `ipaddress` and `port` arguments, it will resort to the defaults which are `127.0.0.1` and `8000`.

**Example**
```bash
./build/cerve 127.0.0.1 6666
```

## Performance using [wrk](https://github.com/wg/wrk)
```bash
# wrk -t1 -c1 -d60s http://127.0.0.1:8000            
Running 1m test @ http://127.0.0.1:8000
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   374.56us  607.06us  20.47ms   92.70%
    Req/Sec     4.00k     1.22k    7.49k    66.17%
  238850 requests in 1.00m, 66.29MB read
Requests/sec:   3978.39
Transfer/sec:      1.10MB
```

## Resources used to learn
1. [Osasazamegbe Medium](https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7 )  
2. [TutorialsPoint](https://www.tutorialspoint.com/http/index.html)  
3. [cppreference](https://en.cppreference.com/w/)
