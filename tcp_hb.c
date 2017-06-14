/**************
gcc tcp_hb.c tcp_client.c util.c -o client -I./jsonx86/out/include/json/   -L./jsonx86/out/lib  -ljson
export LD_LIBRARY_PATH=/home/sintai/heartbeat/jsonx86/out/lib/:$LD_LIBRARY_PATH    
****************/
#include "etcp.h"
#include "heartbeat.h"
#define HOMEPWD  "/etc/config/"
//#define JSPWD  "/usr/lib/js/"
#define JSPWD  "./js/"
#include "json.h"
/*

gcc tcp_hb.c tcp_client.c util.c -o client

*/
char * GetValByEtype(json_object * jobj, const  char  *sname)
{
    json_object     *pval = NULL;
    enum json_type type;
    pval = json_object_object_get(jobj, sname);
    if(NULL!=pval){
        type = json_object_get_type(pval);
        switch(type)
        {
            case    json_type_string:
                return  json_object_get_string(pval);

            default:
                                return NULL;
        }
    }
        return NULL;
}


json_object   *GetValByEdata(json_object * jobj, const  char  *sname)
{
    json_object     *pval = NULL;
    enum json_type type;
    pval = json_object_object_get(jobj, sname);
    if(NULL!=pval){
        type = json_object_get_type(pval);
        switch(type)
        {
                case  json_type_object:
                                return  pval;

                        case  json_type_array :
                                return  pval;
            default:
                                return  NULL;

        }
    }
        return NULL;
}

char   *GetValByKey(json_object * jobj, const  char  *sname)
{
    json_object     *pval = NULL;
    enum json_type type;
    pval = json_object_object_get(jobj, sname);
    if(NULL!=pval){
        type = json_object_get_type(pval);
        switch(type)
        {
            case    json_type_string:
                return json_object_get_string(pval);

                        case  json_type_object:
                                return  json_object_to_json_string(pval);

            default:
                                return NULL;
        }
    }
        return NULL;
}
void getConfigFile(char *msg,char *filename)
{
	char temp[64];
	sprintf(temp,"%s%s",JSPWD,filename);
	FILE *pFile = fopen(temp, "r"); //获取文件的指针

	fseek(pFile, 0, SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
	int len = ftell(pFile);	//获取文件长度

	rewind(pFile);				   //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
	fread(msg, 1, len, pFile); //读文件
	msg[len] = 0;				   //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束

	fclose(pFile); // 关闭文件
}


void getFileData(char *msg,char *filename)
{
	char temp[64];
	sprintf(temp,"%s%s",JSPWD,filename);
	FILE *pFile = fopen(temp, "r"); //获取文件的指针

	fseek(pFile, 0, SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
	int len = ftell(pFile);	//获取文件长度

	rewind(pFile);				   //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
	fread(msg, 1, len, pFile); //读文件
	msg[len] = 0;				   //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束

	fclose(pFile); // 关闭文件
}

int main(int argc, char **argv)
{
	fd_set allfd;
	fd_set readfd;
	msg_t msg;
	char sendmsg[1024];
	char infomsg[2000];
	char recvmsg[2000];
	struct timeval tv;
	SOCKET s;
	int rc;
	int heartbeats = 0;
	int cnt = sizeof(msg);
	int commandkey = 0;
	int uptime = 0;
	int workmode = 0;

	FILE *pFile = fopen("inform.json", "r"); //获取文件的指针

	fseek(pFile, 0, SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
	int len = ftell(pFile);	//获取文件长度

	rewind(pFile);				   //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
	fread(infomsg, 1, len, pFile); //读文件
	infomsg[len] = 0;				   //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束

	fclose(pFile); // 关闭文件

	getFileData(infomsg,"inform.json");



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
			sprintf(sendmsg,infomsg,"112233445566",commandkey,"112233445566",uptime);
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
