#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/sendfile.h>


int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    std::string buf;
    buf.resize(1024);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);
    
    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }

        std::string metadata = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

        recv(sock, buf.data(), 1024, 0);
        std::cout<<buf;

        std::string file = buf.substr(5);
        
        file = file.substr(0, file.find_first_of(' '));

        int opened_fd = open(file.data(), O_RDONLY);

        send(sock, metadata.data(), metadata.size() , 0);

        sendfile(sock, opened_fd, 0, 256);

        close(sock);
    
    }

    close(listener);
    
    return 0;
}