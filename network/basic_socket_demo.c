#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
	int servfd = 0;				// 与服务端通信的文件描述符
	
	struct sockaddr_in servaddr;		// 存放服务端ip和端口的结构体
	memset(&servaddr, 0, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;		// 指定服务端的协议族(ipv4)

	char buffer[BUFSIZE] = {0};		// 发送/接收缓冲数组
	
	// write(STDOUT_FILENO,"Hello world!\n",14);
	
	// 第1步:创建客户端socket / 服务端监听socket
	if((servfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("create socket fd fail.");
		return -1;
	}
#ifdef CLIENT
	// 客户端代码
	if(argc < 3) {
		perror("Using:./basic_client 服务端的IP 服务端的端口\nExample:./demo1 192.168.101.139 5005\n\n");
		close(servfd);
		return -1;
	}

	// 第2步:连接服务端
	struct hostent *h = NULL;		// 存放服务端ip的结构体 
	if(!(h = gethostbyname(argv[1]))) {	// 将字符串格式的ip转换成结构体
		perror("get host info fail.");
		close(servfd);
		return -1;
	}
	memcpy(&servaddr.sin_addr, h->h_addr, h->h_length); // 指定服务端的ip地址
	servaddr.sin_port = htons(atoi(argv[2]));// 从命令行参数中读取端口号
	
	// 接下来向服务端发起连接请求
	if(connect(servfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {    // 向服务端发起连接请求
		perror("connect server fail.");
		close(servfd); return -1;
	}

	// 第3步:与服务端通信
	for(size_t i = 0; i < 5; ++i) {
		memset(buffer,0,sizeof(buffer));
		printf("transmit data: ");
		fflush(stdout);
		if(read(STDIN_FILENO, buffer , sizeof(buffer) - 1) < 0) {
			perror("read stdin error.");
		}else if(send(servfd, buffer, sizeof(buffer) - 1, 0) < 0) {
			perror("transmit message error");
			continue;
		}
		memset(buffer,0,sizeof(buffer));
		
		// recv接收服务端回应报文，如果没有接收到将会阻塞等待
		if(recv(servfd, buffer, sizeof(buffer) - 1, 0) < 0) {
			perror("receive message error");
			continue;
		}
		printf("receive message: %s\n\n",buffer);
		sleep(1);
	}

#else
	// 服务端代码
	if(argc < 2) {
		perror("Using:./demo2 通讯端口\nExample:./demo2 5005\n\n");   // 端口大于1024，不与其它的重复
		close(servfd);
		return -1;
	}
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	    // 指定服务器的任意网卡都可以用于通信
	servaddr.sin_port = htons(atoi(argv[1]));		    // 指定绑定的端口号
	
	// 第2步: 绑定服务端的ip和端口号
	if(bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind port error.");
		close(servfd);
		return -1;
	}

	// 第3步: 服务端监听连接(将服务端设置为可连接的状态)
	if(listen(servfd, 5) < 0) {
		perror("server end listen error.");
		close(servfd);
		return -1;
	}
	printf("server listening......\n");

	// 第4步: 受理客户端的连接请求(如果没有客户端连接上楼来，accept函数将会阻塞等待)
	int clientfd;
	if((clientfd = accept(servfd, 0, 0)) < 0) {
		perror("accept connent error");
		close(servfd); return -1;
	}

	// 第5步: 与客户端通信
	int err;
	printf("client connect success!\n");
	while(true) {
		/* 具体通信逻辑 */
		if((err = recv(clientfd, buffer, sizeof(buffer) - 1, 0)) < 0) {
			perror("receive message from client error.");
			continue;
		}else if(!err) {
			// recv 返回零说明连接已经断开
			printf("connect release\n");
			break;
		}
		printf("receive message from client: %s\n", buffer);
		if(send(clientfd, "ok", sizeof("ok"), 0) < 0) {
			perror("send response error.");
			continue;
		}
		printf("response: %s\n\n","ok");
	}

	// 通信结束，关闭通信socket，释放资源
	close(clientfd);
#endif
	
	// 最后一步: 关闭socket，释放资源
	close(servfd);
	return 0;
}
