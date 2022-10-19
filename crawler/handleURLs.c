#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#if defined(__APPLE__)
#include <sys/event.h> // use kqueue
#elif defined(__linux__) || defined(__unix__)
#include <sys/epoll.h>
#endif
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "bloomfilter.h"

#define buf       1024
#define EVENTSNUM 20
#define SOCKETNUM 100
#define DELAYTIME 1000

char sourceCode[BUF];
FILE *url, *tempurlfile;
char *URLs[maxu]; // 存储所有状态码为200且不重复的链接
int urlsNum;      // 数组URLs中元素的个数

int handleURLs(Queue *queue_url, BF bf, char *serverip, int port)
{
    int len_queue, i;
    int flag = 0; // 退出循环标志
    int para_num = 1024;
    char *para_url[para_num];

#if defined(__APPLE__)
    struct kevent event, event_list[EVENTSNUM];
#elif defined(__linux__) || defined(__unix__)
    struct epoll_event ev, events[EVENTSNUM]; // ev用于注册事件，events数组用于回传要处理的事件
#endif

    char text[buf];
    int epfd;
#if defined(__APPLE__)
    epfd = kqueue();
#elif defined(__linux__) || defined(__unix__)
    epfd = epoll_create(para_num);
#endif

    while (!queue_empty(queue_url)) {
        len_queue = queue_length(queue_url) < SOCKETNUM ? queue_length(queue_url)
                                                        : SOCKETNUM; // 最多同时放出去SOCKETNUM条socket连接
        flag += len_queue;
        for (i = 0; i < len_queue; ++i) {
            para_url[i] = dequeue(queue_url);
            int sockfd;
            buildConnect(&sockfd, serverip, port);
            setnoblocking(sockfd);
            sendRequest(para_url[i], sockfd, serverip);

            Ev_arg *arg = (Ev_arg *)calloc(sizeof(Ev_arg), 1);
            arg->urlName = para_url[i];
            arg->fd = sockfd;

#if defined(__APPLE__)
            EV_SET(&event, sockfd, EVFILT_READ, EV_ADD, 0, 0, arg);
            kevent(epfd, &event, 1, NULL, 0, NULL);
#elif defined(__linux__) || defined(__unix__)
            ev.data.ptr = arg;
            ev.events = EPOLLIN | EPOLLET;               // 设置要处理的事件类型。可读，边缘触发
            epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 注册ev
#endif
        }

        while (flag) {
            int events_num = 1;
#if defined(__APPLE__)
            events_num = kevent(epfd, NULL, 0, event_list, EVENTSNUM, NULL);
#elif defined(__linux__) || defined(__unix__)
            events_num = epoll_wait(epfd, events, EVENTSNUM, DELAYTIME); // 等待sockfd可读，即等待http response
#endif
            for (i = 0; i < events_num; ++i) {
#if defined(__APPLE__)
                if (EVFILT_READ == event_list[i].filter) {
                    Ev_arg *arg = (Ev_arg *)(event_list[i].udata);
#elif defined(__linux__) || defined(__unix__)
                if (events[i].events & EPOLLIN)                          // 接收到数据，读socket
                {
                    Ev_arg *arg = (Ev_arg *)(events[i].data.ptr);
#endif
                    memset(sourceCode, 0, BUF);
                    while (read(arg->fd, text, buf)) // 读
                    {
                        strcat(sourceCode, text);
                        memset(text, '\0', buf);
                    }
                    // 输出状态码
                    char *s = sourceCode;
                    s += 9;
                    char d[3];
                    memcpy(d, s, 3);
                    int code = atoi(d);
                    if (code == 200) {
                        URLs[urlsNum] = arg->urlName;
                        fprintf(url, "%d http://news.sohu.com%s\n", urlsNum, arg->urlName); // 写入 url.txt 文件
                        // 在此处加入到树中
                        DFA(sourceCode, queue_url, bf, urlsNum);
                        urlsNum++; // 从0开始编号并存入数组
                    } else {
                        printf("code = %d\n", code);
                        free(arg);
                    }
                    close(arg->fd);
                    flag--;
                    continue;
                } else {
#if defined(__APPLE__)
                    printf("Closed connection on descriptor %d\n", (int)event_list[i].ident);
                    close((int)event_list[i].ident);
#elif defined(__linux__) || defined(__unix__)
                    printf("Closed connection on descriptor %d\n", events[i].data.fd);
                    close(events[i].data.fd);
#endif
                }
            }
        }
    }
    close(epfd);
    return 0;
}


// 建立连接
int buildConnect(int *sockfd, char *serverip, int port)
{
    if (-1 == (*sockfd = socket(AF_INET, SOCK_STREAM, 0))) // 穿件套接字  | SOCK_NONBLOCK 异步
    {
        printf("sockfd open error!\n");
        return -1;
    }

    struct sockaddr_in addr;
    int sock_len = sizeof(struct sockaddr);
    bzero(&addr, sock_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverip); // 按照网络字节顺序存储IP地址
    addr.sin_port = htons(port);

    if (-1 == connect(*sockfd, (struct sockaddr *)(&addr), sock_len)) {
        printf("connect fail! try again...\n");
        close(*sockfd);
        return -1;
    }
    return 0;
}

// 设置文件描述符为非阻塞模式
int setnoblocking(int sockfd)
{
    int opts;
    opts = fcntl(sockfd, F_GETFL, 0); // 获取文件标志和访问模式
    opts |= O_NONBLOCK;               // 非阻塞
    fcntl(sockfd, F_SETFL, opts);

    return 0;
}


// 发送http request
int sendRequest(char *url, int sockfd, char *serverip)
{
    char myurl[BUFSIZ];
    char *pHost = 0, *pGET = 0;
    char host[BUFSIZ], GET[BUFSIZ];
    char header[BUFSIZ] = "";

    strcpy(myurl, serverip);
    strcat(myurl, url);
    for (pHost = myurl; *pHost != '/' && *pHost != '\0'; ++pHost)
        ;
    if ((int)(pHost - myurl) == strlen(myurl))
        strcpy(GET, "/");
    else
        strcpy(GET, pHost);
    *pHost = '\0';
    strcpy(host, myurl);

    strcat(header, "GET ");
    strcat(header, GET);
    strcat(header, " HTTP/1.1\r\n");
    strcat(header, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/106.0.0.0 Safari/537.36\r\n");
    strcat(header, "HOST: ");
    strcat(header, host);
    strcat(header, "\r\nConnection: Close\r\n\r\n");
    // strcat(header, "\r\nKeep-Alive: 1000\r\n");
    // strcat(header, "\r\nConnection: keepalive\r\n\r\n");
    // strcat(header, "\r\nCache-Control: no-cache\r\n\r\n");

    printf("header: %s\n", header);

    write(sockfd, header, strlen(header));
    return 0;
}
