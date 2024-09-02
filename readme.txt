a.  Jason Guerrero

b.  5466170786

c. N/A

d. 

submission files:
    - serverM.cpp
        - works as described in the specification.  uses 2D vectors for intersection algorithm.
    - serverA.cpp
        - works as described in the specification.  uses 2D vectors for intersection algorithm.
    - serverB.cpp
        - works as described in the specification.  uses 2D vectors for intersection algorithm.
    - client.cpp
        - does not work exactly as described in the specification.  There are too many names in the existing names output (i.e. nonexisting names).

e.
serverM:
Main Server is up and running.
Main Server received the username list from server A using UDP over port 23786.
Main Server received the username list from server B using UDP over port 23786.
Main Server received the request from client using TCP over port 24786.
jason do not exist. Send a reply to the client.
Found khloe theodore at Server A. Send to Server A.
Main Server received from Server A the intersection result using UDP over port 23786:
[[11,12],[21,22],[26,27]]
Found luis ava at Server B. Send to Server B.
Main Server received from Server B the intersection result using UDP over port 23786:
[[11,12]]
Found the intersection between the results from server A and B:
[[11,12]]
Main Server sent the result to the client.

serverA:
Server A is up and running using UDP on port 21786.
Server A finished sending a list of usernames to Main Server.
Server A received the usernames from Main Server using UDP over port 21786.
Found the intersection result: [[11,12],[21,22],[26,27]] for khloe theodore 
Server A finished sending the response to Main Server.

serverB:
Server B is up and running using UDP on port 22786.
Server B finished sending a list of usernames to Main Server.
luis ava 
Server B received the usernames from Main Server using UDP over port 22786.
Found the intersection result: [[11,12]] for luis ava 
Server B finished sending the response to Main Server.

client:
Client is up and running.
Please enter the usernames to check schedule availability:
jason luis ava khloe theodore
Client received the reply from Main Server using TCP over port 24786:
jason does not exist
Client received the reply from Main Server using TCP over port 24786:
Time intervals [[11,12]] works for jason luis ava khloe theodore

g.
for the client, in the last phase, I am printing out all names for the intersection output rather than just the existing names.

extra files:
    - just headers.h

h.
 Most of the code is my own except pieces from
Beej's Code: http://www.beej.us/guide/bgnet/
        Create sockets (TCP / UDP);
        Bind a socket;
        Send & receive;