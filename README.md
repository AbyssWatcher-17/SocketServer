# SocketServer
Simple Server-Client Socket in Linux (Ubuntu) in C programming language

![5](https://user-images.githubusercontent.com/64128266/127317754-26b0a1d0-4779-4ba3-ae55-5f25a215791a.PNG)


You need to Linux operating system for this project!!!

//Server 
1. Compile:		gcc -o main main.c -lpthread
2. Server start:		./main 5678

//Client 
1. to connect server with Client:	telnet localhost 5678


Note:5678 is number of Port.If you change port number you may change it from main.c but never forget to compile with that port number 

Functions:
**********
PUT --> key1 value1 : It will create key1.txt file(if file's not exist) and put the value in it

GET --> key1 :It will search key1.txt file .If it's found then print the value.If not print key_nonexistent

DEL --> key1 value1 :  It will search key1.txt file .If it's found then delete the file .If not print key_nonexistent

BEG --> Start sole access.Only that client change that key others cannot
(You're going to need at least 2 Client to test this function properly.To connect another client to server just open new terminal page and connect just like first client  ) 

END --> End Sole access

SUB --> key1 value1 :Client can "Subscribe" to a key and when one of Other Clients  change this key's value then "Subscribed Client" will know it. 
(You're going to need at least 2 Client to test this function properly.To connect another client to server just open new terminal page and connect just like first client  ) 

OP -->  you may learn:<br>
      1-  Who's using this Linux.By typing --> OP anything who <br>
      2-  Uptime .By typing                --> OP anything uptime <br>
      3-  Date. By typing                  --> OP anything date <br>

QUIT --> End the server
