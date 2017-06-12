#include "etcp.h"
#include "heartbeat.h"
/*

gcc tcp_hb.c tcp_client.c util.c -o client

*/


int main(int argc, char **argv)
{
	fd_set allfd;
	fd_set readfd;
	msg_t msg;
	char sendmsg[2000];
	char recvmsg[2000];
	struct timeval tv;
	SOCKET s;
	int rc;
	int heartbeats = 0;
	int cnt = sizeof(msg);

	FILE *pFile = fopen("inform.json", "r"); //获取文件的指针

	fseek(pFile, 0, SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
	int len = ftell(pFile);	//获取文件长度

	rewind(pFile);				   //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
	fread(sendmsg, 1, len, pFile); //读文件
	sendmsg[len] = 0;				   //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束

	fclose(pFile); // 关闭文件

	INIT();
	s = tcp_client(argv[1], argv[2]);
	FD_ZERO(&allfd);
	FD_SET(s, &allfd);
	tv.tv_sec = T1;
	tv.tv_usec = 0;
	for (;;)
	{
		readfd = allfd;
		rc = select(s + 1, &readfd, NULL, NULL, &tv);
		if (rc < 0)
			error(1, errno, "select failure");
		if (rc == 0) /* timed out */
		{
			if (++heartbeats > 13)
				error(1, 0, "connection dead\n");
			error(0, 0, "sending heartbeat #%d\n", heartbeats);
			msg.type = htonl(MSG_HEARTBEAT);

			rc = send(s, (char *)sendmsg, sizeof(sendmsg), 0);
			if (rc < 0)
				error(1, errno, "send failure");
			tv.tv_sec = T2;
			continue;
		}
		printf("jiangyib jjj");
		if (!FD_ISSET(s, &readfd))
			error(1, 0, "select returned invalid socket\n");
		rc = recv(s, (char *)recvmsg,
				  2000, 0);
		if (rc == 0)
			error(1, 0, "server terminated\n");
		if (rc < 0)
			error(1, errno, "recv failure");
		heartbeats = 0;
		tv.tv_sec = T1;

		if (rc > 2000)
			continue;
		printf("jiangyibo %s", recvmsg);

		/* process message */
	}
}
