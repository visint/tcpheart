# tcpheart
gcc hb_client.c tcp_client.c util.c -o client
gcc hb_server.c tcp_server.c util.c -o server
gcc vrc.c tcp_client.c util.c -o client
gcc vrs.c tcp_server.c util.c readvrec.c readn.c  -o server
