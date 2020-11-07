#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<errno.h>

#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>
#include<time.h>

int is_dir( char* file_name);
void cp_file( char *source_path , char *destination_path, char* compare_path)
{
	char command[1030];
	sprintf(command,"diff -q --speed-large-files \"%s\" \"%s\"",source_path,compare_path);
	if(system(command)==0){
		printf("%s is no new. Skip.\n", source_path);
		return;
	}
	FILE* fp_src = NULL;
	FILE* fp_dst = NULL;
//	int in,out;
	if((fp_src = fopen(source_path,"r"))==NULL){//打开源文件的文件流
		printf("Error in opening %s. Abort.\n", source_path);
		exit(1);
	}
	if((fp_dst=fopen(destination_path,"w"))==NULL){//打开目标文件的文件流
		printf("Error in creating %s. Abort.\n", destination_path);
		exit(1);
	}
	int c, total = 0;
	while(1)
	{
		c = fgetc( fp_src);
		if( c == EOF && feof(fp_src))
		{
			break;
		}
		else if( ferror(fp_src))
		{
			perror("fget()");
			break;
		}
		fputc(c,fp_dst);
	} 
	fclose(fp_src); 
	fclose(fp_dst); 
}

int endwith(char* s,char c){//用于判断字符串结尾是否为“.”
	if(s[strlen(s)-1]==c){
		return 1;
	}
	else{
		return 0;
	}
}

void copy_folder(char* source_path, char* destination_path, char* compare_path)
{
	DIR *dst_dp = opendir(destination_path);
	if(dst_dp  == NULL)// if destination_path is not exist , mkdir new one
	{
		printf("Working on dir %s.\n", source_path);
		if(mkdir(destination_path,0777) == -1)
		{
			printf("Error in creating dir. Abort.\n");
			exit(-1);
		}
	}
	else{
		printf("Destination dir exists. May overwrite existing data. Abort.\n");
		exit(2);
	}
	DIR *src_dp = opendir(source_path);	
	struct dirent *ep_src =  readdir(src_dp);
	char address[512] = {0};
	char toaddress[512] = {0};
	char compaddress[512] = {0};
	while(1)//this is a recursion,break until ( all files in source_path have been copied , or error occured)
	{
		sprintf(address,"%s/%s",source_path,ep_src->d_name);
		sprintf(toaddress,"%s/%s",destination_path,ep_src->d_name);
		sprintf(compaddress,"%s/%s",compare_path,ep_src->d_name);
		if(endwith(address,'.') == 1)//if the file is . or .. pass
		{
		//	In this case, dir should be ignored.
		//	ep_src = readdir(src_dp);	
		}
		else if( ( is_dir(address) != 1) )//if the file is not dir just copy file
		{
			cp_file(address,toaddress,compaddress);	
		//	ep_src = readdir(src_dp);
		}
		else
		{
			copy_folder(address,toaddress,compaddress);// when test the file is a dir , call copy_folder function again
		}

		if((ep_src = readdir(src_dp)) == NULL )// if all files in address have been copied , break;
			break;	
	
		memset(address,sizeof(address),0);
		memset(toaddress,sizeof(toaddress),0);
		memset(compaddress,sizeof(compaddress),0);
	}
	    closedir(dst_dp);
		closedir(src_dp);
		char command[520];
		sprintf(command, "rmdir \"%s\"", destination_path);
		system(command);
		return;
}
int is_dir( char* file_name)
{
	struct stat info;
	stat(file_name,&info);
	if(S_ISDIR(info.st_mode))
		return 1;
	else
		return 0;
}

int main( int argc , char** argv)
{
	printf("(GCG) Differented files extractor by i@C86.moe\n");
	if(argc != 4)
	{
		printf("Usage: %s <Origin Dir> <Target Dir> <Old Dir>\n",argv[0]);
		exit(1);
	}
	copy_folder(argv[1],argv[2],argv[3]);
	printf("Done.\nIt is common to see 'Directory not empty'.\n");
	return 0;
}