# ccerve

An HTTP server written from scratch in C++.

### How to run:

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

### How to contribute
1. Fork the repo.
2. Clone the forked repo into your machine.
3. Make the desired changes.
4. Commit and push.
5. Open the pull request.

### Scope of this Project
1. Basic server
2. Static File Serving
3. Server configuration through files
4. Logger

If I have extra time then I will also do:
1. Implement HTTPS for security
2. Implement Asynchronous I/O
3. Basic GUI

### Resources used to learn
https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7  
https://www.tutorialspoint.com/http/index.htm  
https://en.cppreference.com/w/
