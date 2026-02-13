#include <stdio.h>
#include <stdlib.h>
#include <process.h>

void compile_n_run(){
	system("gcc -o data data.c && data");
}

int main(int,char *argv[]) {

	//Read block
	char ch[100][50];
	int count=0;
	FILE *fp = fopen(argv[1],"r");
	while( fscanf(fp, "%49s",ch[count]) ==1){
		count++;
	}
	fclose(fp);

	//Show block
	int count2=0;
	while(count2<count){
		printf("%s ",ch[count2]);
		count2++; 
	}
	
	printf("\n->");

	//compile
	int _compile_;
	const char *_args_for_compile_[] = {
		"gcc",
		"-o",
		"data",
		argv[1], //data.c
		NULL
		};
	_compile_ = _spawnvp(_P_WAIT,"gcc",_args_for_compile_); //parent wait
	if(_compile_!=0){
		printf("compile failed!");
		return 1;
	}
	
	//run
	int _run_;
	const char *_args_for_run_[]={
		"data",
		NULL
		};
	_run_ = _spawnvp(_P_WAIT,"data",_args_for_run_);
	if(_run_!=0){
		printf("Running failed");
		return 1;
	}

	printf("\n");
	
	printf(">>");
	
	char cursor_and_input[50];
	fgets(cursor_and_input,sizeof(cursor_and_input),stdin);
	
	printf("%s",cursor_and_input);

	return 0;
}