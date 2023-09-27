#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock;

void* server(void* arg)
{
    int port = 1234, counter = 0, sockfd, n;
    struct sockaddr_in si_me, si_other;
    char buffer[1024];
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&si_me, '\0', sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me));
    addr_size = sizeof(si_other);
    
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)& si_other, &addr_size);
    printf("server: Number of Message Received: %s \n", buffer);
    n = std::stoi(buffer);

    while (counter < n)
    {
        pthread_mutex_lock(&lock);
        counter++;
        recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)& si_other, &addr_size);
        printf("server: %dth Data Received: %s\n", counter, buffer);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(arg);
}

void* client(void* arg)
{
    int port = 1234, counter = 0, sockfd, n;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    socklen_t addr_size;
    std::string str;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&serverAddr, '\0', sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    pthread_mutex_lock(&lock);

    std::cout<<"client: how many times do you want to send a message?"<<std::endl;
    std::cin>>n;
    
    strcpy(buffer, std::to_string(n).c_str());
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("client: Number of Message Sent: %s \n", buffer);
    pthread_mutex_unlock(&lock);

    while (counter < n )
    {
        counter++;
        
        std::cout<<"client: Enter a "<<counter<<"th string: "<<std::endl;
        std::cin>>str;

        strcpy(buffer, str.c_str());
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        printf("client: %dth Data Send: %s\n", counter, buffer);
    }
    close(sockfd);
    pthread_exit(arg);

}

int main()
{
    pthread_t thread1, thread2;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&thread1, NULL, server, NULL);
    pthread_create(&thread2, NULL, client, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}

