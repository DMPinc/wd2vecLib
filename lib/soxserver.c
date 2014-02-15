#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define COMMAND_LENGTH 1024

int
main()
{
 int sock0;
 struct sockaddr_in addr;
 struct sockaddr_in client;
 socklen_t len;
 int sock;
 int n;
 char keycode[COMMAND_LENGTH];

 /* ソケットの作成 */
 sock0 = socket(AF_INET, SOCK_STREAM, 0);

 /* ソケットの設定 */
 addr.sin_family = AF_INET;
 addr.sin_port = htons(12345);
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_len = sizeof(addr);
 //sprintf(keycode, "%s", " -n synth pl G2 pl B2 pl D3 pl G3 pl D4 pl G4                delay 0 .05 .1 .15 .2 .25 remix - fade 0 4 .1 norm -1");
 bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

 /* TCPクライアントからの接続要求を待てる状態にする */
 listen(sock0, 5);
while(1){
 /* TCPクライアントからの接続要求を受け付ける */
 len = sizeof(client);
 sock = accept(sock0, (struct sockaddr *)&client, &len);
 printf("sock:%d\n",sock);
 memset(keycode,0,sizeof(keycode));
 n = read(sock, keycode, sizeof(keycode));
 printf("read: %s\n", keycode);
/* 5文字送信 */
 write(sock, keycode, sizeof(keycode));
 /* TCPセッションの終了 */
 close(sock);
}
 /* listen するsocketの終了 */
 close(sock0);

 return 0;
}
