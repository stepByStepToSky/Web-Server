## Web-Server

Guide to header files:
- **base/log.h** Interface for log system, which is from [Asynchronous Log](https://github.com/stepByStepToSky/ThreadPool-and-Asynchronous-log).

- **epoller.h** Encapsulate related functions of epoll.
- **netutil.h** Encapsulate related functions of socket fd.
- **simplebuffer.h** Application layer buffer(we only implement the LT model now).
- **channel.h** Encapsulate channel(event) related callback functions (read/write/error functions).
- **eventloop.h** It manage the channel(event), which is registed to the epoller, and process the active channels(events).
- **tcpserver.h** When read event happends, it automatically read the byte stream into the application buffer of the channel, and execute the read callback function. The same as write and error events happend.
- **httpmessage.h** Parse the HTTP request, build HTTP respond message. Here, we use a state machine to process HTTP requests.
- **basecgi.h** The abstract class of Cgi instance class. It provides a Process interface, we use it to process the cgi request.
- **includecgiheader.h** Include the Cgi instance class, which is used for cgidispatch.
- **cgidispatch.h** The Cgi dispatcher, which use the url to find the Cgi instance, to execute the cgi program.
- **httpcgiserver.h** When read event happend, it firstly parses the http request in the application buffer of the channel, then finds the cgi instance and process the request.
- **httpcgiservermain.cpp** HTTP server main.
- **exampleCgi/examplecgi.h and loginCgi/logincgi.h** Example of how to write a Cgi instance class. webFile/login includes the .html, .js, .css files needed by the LoginCgi class.
