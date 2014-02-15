/**
* argv[1]をサーバに投げて帰ってきた値をplayコマンドで鳴らすクライアントコマンド
*
*
*
*
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <err.h>

#define COMMAND_LENGTH 1024

int
main(int argc, char** argv)
{
 struct sockaddr_in server;
 int sock;
 char buf[COMMAND_LENGTH];
 char cmd[COMMAND_LENGTH];
 char input[COMMAND_LENGTH];
 sprintf(cmd, "%s", "/usr/local/bin/play ");
 int n;
 FILE *fp = NULL;
 printf("argc=%d\n",argc);
 sprintf(buf,"%s",argv[1]);
 /* ソケットの作成 */
 sock = socket(AF_INET, SOCK_STREAM, 0);
 /* 接続先指定用構造体の準備 */
 server.sin_family = AF_INET;
 server.sin_port = htons(12345);
 server.sin_addr.s_addr = inet_addr("127.0.0.1");

 /* サーバに接続 */
 connect(sock, (struct sockaddr *)&server, sizeof(server));

 write(sock, buf, sizeof(buf));
 printf("write %s\n",buf);
 /* サーバからデータを受信 */
 memset(buf, 0, sizeof(buf));
 n = read(sock, buf, sizeof(buf));

 printf("%d, %s\n", n, buf);
 strcat(cmd, buf);
 //if ( (fp=popen(cmd,"r")) == NULL){
 // err(EXIT_FAILURE, "%s", cmd);
 //}
 //(void)pclose(fp);
 printf ( "cmd: %s\n", cmd);
 /* socketの終了 */
 close(sock);

 return 0;
}

