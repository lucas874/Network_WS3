### For compiling and testing the TCP client and server:
gcc -o tptcpserver tptcpserver.c server.c<br>
gcc -o tptcpclient tptcpclient.c<br>

### Now in two different terminals run:
./tptcpserver <some integer x used as port><br>
./tptcpclient 127.0.0.1 <x>

The first time the client is run it does not display a reply, but after this it works fine, we do not know why and it's only for the tcp client/server... The port specified in the RFC\cite{RFC-868} is 67, we used number we used was 50000. To avoid clashing with some port already in use use a number \> 49151.\\

### For compiling and testing the UDP client and server:
gcc -o tpudpserver tpudpserver.c server.c<br>
gcc -o tpudpclient tpudpclient.c

### Now in two different terminals run 
./tpudpserver <some integer x used as port><br>
./tpudpclient 127.0.0.1 <x>
