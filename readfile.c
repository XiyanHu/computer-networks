#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 1024
int main(int argc,char *argv[])
{
 char buf[1024];  /*缓冲区*/
 FILE *fp;            /*文件指针*/
 int len;             /*行字符个数*/
 int num_of_line = 0;
 char dot[] = ",";
 if (argc != 2) {
	    fprintf(stderr,"shabi,input text name\n");
	    exit(1);
	}
 if((fp = fopen(argv[1],"r")) == NULL){
	 perror("fail to read");
	 exit (1) ;
 }
 char *result = NULL;
 char res_sep[100][100];
 int ct = 0;
 while(fgets(buf,MAX_LINE,fp) != NULL){
	 num_of_line ++;
	 len = strlen(buf);
	 buf[len-1] = '\0';  /*去掉换行符*/
	 // printf("%s\n",buf);
	  result = strtok(buf, dot);
	  while( result != NULL ) {
	  	strcpy(res_sep[ct],result);
	  	ct ++;
        //printf( "%s", result );
        result = strtok( NULL, dot);
    	}
 }
 for (ct=0;ct<10;ct++){
 	printf( "%send\n", res_sep[ct] );	
 }
 printf( "number of line is %d\n", num_of_line );	
 return 0;
}