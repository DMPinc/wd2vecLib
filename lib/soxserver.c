#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define COMMAND_LENGTH 1024
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc/malloc.h>

const long long max_size = 500;// max dim
const long long N=3; // 5 Best
const long long max_w = 50; // words

typedef struct WordVec_t{
  char *bestw[N];
  float bestd[N], vec[max_size];
  long long words /* trained words num */, size /* vector dim */,  bi[100];
  float *M;
  char *vocab;
}WordVec;

/**
*  binaryデータの読み込み
*  @return -1: error 0: success
*
*
*/
int loadFile(char* filename, WordVec* vec) {
  long long i = 0, j=0;
  char ch;
  float len = 0;
  FILE* f = fopen(filename, "rb");
  if (f == NULL){
    printf("Input file not found\n");
    return -1;
  }
  //vec = (WordVec*)malloc(sizeof(WordVec)*1);
  fscanf(f, "%lld", &(vec->words));
  fscanf(f, "%lld", &(vec->size));
  vec->vocab= (char*)malloc((long long)vec->words * (long long)vec->size * sizeof(char));
  for (i = 0; i < N ; i++) vec->bestw[i] = (char*)malloc(max_size * sizeof(char));
  vec->M = (float *)malloc((long long)vec->words *(long long)vec->size * sizeof(float));
  if (vec->M == NULL) {
    printf("cannot allocate memory: %lld MB %lld %lld\n",(long long)vec->words * vec->size * sizeof(float) / 1048576, vec->words, vec->size);
    return -1;
  }
  //Mの正規化
  for (i = 0; i < vec->words ; i++) {
    fscanf(f, "%s%c", &(vec->vocab[i * max_w]), &ch);
    for (j = 0; j < vec->size; j++) fread(&(vec->M[j + i * vec->size]), sizeof(float), 1, f);
    len = 0;
    for (j = 0; j < vec->size; j++) len += vec->M[j + i * vec->size] * vec->M[j + i * vec->size];
    len = sqrt(len);
    for (j = 0; j < vec->size; j++) vec->M[j + i * vec->size] /= len;
  }
  fclose(f);
  return 0;
}

/**
* 最も近い単語を探す
* @param vec 単語ベクトルデータ
* @param words 入力文字列（複数単語可 区切り文字はスペース）
* @param result 入力文字列に対する最近傍 文字列 
* @return -1 エラーの場合
*         0 
*/
int getNNResult(WordVec* vec, char* words, char *result) {
  long long i = 0, j = 0, k = 0, l = 0;
  long long wordnum = 0;
  long long wordIdx = -1;
  char input[max_size];
  char inputs[100][max_size];
  char *token;
  float *input_vec = (float*) malloc ( vec->size * sizeof(float) );
  float norm = 0, dist = 0;
  //初期化
  for( i = 0; i < vec->size ; i++) input_vec[i] = 0;
  for( i = 0; i < N ; i++ ) {
    vec->bestd[i] = 0;
    vec->bestw[i][0] = 0;
  }
  strcpy( input, words);
  
  if ( (token = strtok(input, " \n")) == NULL) {
    strcpy(inputs[0], words);
    wordnum = 1;
  } else {
    strcpy(inputs[0], token);
    wordnum++;
  }
  while((token = strtok(NULL, " \n")) != NULL) {
    strcpy(inputs[wordnum++],token);
  }
  //search word index for each input
  for ( j = 0; j < wordnum ; j++) {
    for ( i = 0; i < vec->words ; i++) {
      if ( !strcmp(&vec->vocab[i* max_w], inputs[j]) ) break;
      // 見つからなかった
      if ( i == vec->words ) {
        return -1;
      };
    }
    vec->bi[j] = i;
  }

  // vectorize input words
  for ( j = 0 ; j < wordnum ; j++) {
    for ( i = 0 ; i < vec->size; i ++ ) {
      input_vec[i] += vec->M[i + vec->bi[j] * vec->size];
    }
  }
  for ( i = 0; i < vec->size ; i ++) {
      norm += input_vec[i] * input_vec[i];
  }
  norm = sqrt(norm);
  if ( norm < 1.0e-9 ) norm = 1.0;
  for ( i = 0; i < vec->size ; i ++) {
      input_vec[i] /= norm;
  }


  // calc cosine similarity
  int sameWordFlag = 0;
  for ( k = 0; k < vec->words; k ++) {
      // 自分を入力単語に含む場合は比較対象から外す
      for ( j = 0; j < wordnum ; j++) if (vec->bi[j] == k) sameWordFlag = 1;
      if ( sameWordFlag == 1 ) continue;
      dist = 0;
      for ( i = 0 ; i < vec->size ; i ++ )  dist += input_vec[i] * vec->M[i + k * vec->size];
      // 差し替え
      for ( i = 0 ; i < N ; i++) {
        if ( dist > vec->bestd[i] ) {
          for ( l = N - 1 ; l > i ; l-- ) {
            vec->bestd[l] = vec->bestd[l-1];
            strcpy(vec->bestw[l], vec->bestw[l-1]);
          }
          vec->bestd[i] = dist;
          strcpy(vec->bestw[i], &(vec->vocab[k* max_w]));
          break;
        }
      }
  }

  for ( i = 0; i < N ; i++) {
    printf("%lld-th: %s\n", i+1, vec->bestw[i]);
    strcat ( result, vec->bestw[i] );
    strcat ( result, " ");
  }
  free(input_vec);
  return 0;
}

int
main(int argc, char** argv)
{
 int sock0;
 struct sockaddr_in addr;
 struct sockaddr_in client;
 socklen_t len;
 int sock;
 int n;
 char keycode[COMMAND_LENGTH];
 char msg[COMMAND_LENGTH] = {0};
 WordVec vec ;
 if ( loadFile(argv[1], &vec) != 0) {
    printf("trained model file loading error. exit\n");
    return -1;
 };
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
 printf("size:%lld words:%lld\n", vec.size, vec.words);
while(1){
 /* TCPクライアントからの接続要求を受け付ける */
 len = sizeof(client);
 sock = accept(sock0, (struct sockaddr *)&client, &len);
 printf("sock:%d\n",sock);
 memset(keycode,0,sizeof(keycode));
 memset(msg,0,sizeof(msg));
 n = read(sock, keycode, sizeof(keycode));
 printf("read: %s\n", keycode);
 if ( -1 == getNNResult(&vec, keycode, msg) ) {
  write(sock, "error", 6);
 }else {
  printf("write: %s\n", msg);
  /* 5文字送信 */
  write(sock, msg, sizeof(msg));
 }
 /* TCPセッションの終了 */
 close(sock);
}
 /* listen するsocketの終了 */
 close(sock0);

 return 0;
}
