# muduoZ
My project for learning webServer and multi-thread programing, refer to moduo and linyacool/WebServer


## introduce
### Logging System
The logging system running in asynchronous way, we have a frontend Logger and a backend AsyncLogging.
1. Frontend:  
We have a temporary logger object record the info and use the API of backend for recording info in file.
2. Backend:   
Backend Provides the API， and also work in asynchronous way, here are two buffers in both back-frontend and back-backend, exchange by turns.  

### Multithread Network Framework  
The Network Framework can be divided into three parts.
1. EventLoop-Channel-Poller


2. 
