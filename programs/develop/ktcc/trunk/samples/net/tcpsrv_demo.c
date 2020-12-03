#include <net/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char msg1[]="Hello!"; 
char msg2='\0';

int main()
{
    sockaddr addr={AF_INET4, PORT(23) , 0, 0};
    
    int sk1=socket(AF_INET4, SOCK_STREAM, IPPROTO_TCP);
    printf("Open socket: %d. Error: %d\n",sk1, err_code);
    
    bind(sk1, &addr,sizeof(addr));
    printf("Socket binding. Error: %d\n", err_code);
    
    listen(sk1, 1);
    printf("Listening to a socket. Error: %d\n", err_code);
    int sk2 = accept(sk1, &addr, sizeof(addr));
    printf("Accept done. Error: %d\n", err_code);
    
    send(sk2, msg1, strlen(msg1),MSG_NOFLAG);
    printf("Send message: '%s'  Error: %d\n", msg1, err_code);
    puts("Received data:");
    while(msg2!='!')
    {
        recv(sk2, &msg2, 1, MSG_NOFLAG);
        printf("%c",msg2);
    }
    close(sk1);
    close(sk2);
    puts("\nGood bye!");
    exit(0);
}
