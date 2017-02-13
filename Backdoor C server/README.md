# Backdoor webserver
A compromised webserver in C for HTTP 1.1 requests and responses. The servers responds with 404 errors for everything except when the backdoor is invoked, then it executes system command after the ```/exec/<system command>``` and displays the output as the response.

## Usage

```
./normal_web_server <port number>

```
