furry-octo-bear
===============

CS470 Distributed Service Project

There are two parts. A server application and a Client appliction. 
There can be many servers that work together. Each server provides some set of services that can be requested
by the client. If the Server cannot fulfil the request, it will send the client the hostname of a server that can
fulfill the request, or an error message if no such server exists.

On initialization, the server sends a hello message to other servers that it knows.
This hello message lets the other servers know what services this server offers. 
The other server lets this server know what services it provides in response.

The services are determined based on a plugin architecture, where, on startup, the server reads the /services/ directory
for executables with the .service extension.

All communication is done through a tcp and sockets using a custom protocol.
@See: protocol_examples.txt for more details
