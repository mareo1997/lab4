#include <stdio.h>
#include <stdlib.h>

void main(){
	File *fptr;
	char filename[15];
	char ch;
	printf("open file\n");
	fptr = fopen("test.txt", "r");
	if(fptr == NULL){
		printf("no\n");
		exit(0);
	}
	ch = fgetc(fptr);
	while(ch != EOF){
		printf("%c", ch);
		ch = fgetc(fptr);
	}
	fclose(fptr);
}
