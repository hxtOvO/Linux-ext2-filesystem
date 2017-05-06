#include <stdio.h>
#include <string.h>
#include <time.h>
#include "test.h"
#include <stdlib.h>
#include <ctype.h> //��Сдת��
#include <winsock2.h>
#include <windows.h>



int main(void){
	printf("Welcome to sking's simple ext2\n");
	printf("******************************\n");
	printf("**  Author��@sking        ****\n");
	printf("**  Create��2016-6-8      ****\n");
	printf("**  Blog  ��www.sking.xin ****\n");
	printf("******************************\n\n");
	
	pf = fopen("fileSystem.dat", "r+b");
	if(!pf){		
		printf("The File system does not exist!, it will be creating now!\n");
		create_fileSystem();
	}
	
	pf = fopen("fileSystem.dat", "r+b");
	last_inode_bit=1;  
	last_block_bit=0; 
	read_super_block();
	read_block_group_desc();

	//Ŀ¼�� 
	strcpy(current_path, "[root@sking ");
	strcpy(path_name, "/");
	strcpy(path_last, "]# ");
	
	current_dir_inode=1; 
	current_dir_length=1;
	
	int flag = 1;
//	printf("[root@sking /]# ");
	while(1){
		strcpy(current_path, "[root@sking ");
		strcat(current_path, path_name);
		strcat(current_path, path_last);

		char command[10]="", temp[9]="";  //�洢����ͱ��� 

		fflush(stdin); //���stdin���Ͳ������������ʾ���� 
		printf("%s", current_path);
			//����ǻس��������� 

		if(flag){
			command[0] = getchar();
			if(command[0] == '\n'){
				continue;
			}
		}
		scanf("%s", command+1);
		
		if(!strcmp(command, "ls")){
			ls();

		}else if(!strcmp(command, "mkdir")){
			scanf("%s", temp);
			mkdir(temp);
			
		}else if(!strcmp(command, "cd")){
			scanf("%s", temp);
			cd(temp);
			
		}else if(!strcmp(command, "format")){
			format();
			
		}else if(!strcmp(command, "rmdir")){
			scanf("%s", temp);
			rmdir(temp);
			
		}else if(!strcmp(command, "help")){
			help();
			
		}else if(!strcmp(command, "logout")){
			logout();
			
		}else if(!strcmp(command, "dumpe2fs")){
			dumpe2fs();
			
		}else if(!strcmp(command, "vi")){
			scanf("%s", temp);
			vi(temp);
			
		}else if(!strcmp(command, "cat")){
			scanf("%s", temp);
			cat(temp);
			
		}else if(!strcmp(command, "rm")){
			scanf("%s", temp);
			rm(temp);
			
		}else if(!strcmp(command, "ifconfig")){
			ifconfig();

		}else if(!strcmp(command, "ping")){
			char ip[128];
			scanf("%s", ip);
			ping(ip);

		}else if(!strcmp(command, "rename")){
			char oldname[9];
			char newname[9];
			scanf("%s %s", oldname, newname);
			reName(oldname, newname);

		}else if(!strcmp(command, "data")){
			data();

		}else if(!strcmp(command, "chmod")){
			int mode; 
			scanf("%s", temp);
			scanf("%d", &mode);
			chmod(temp, mode);

		}else{
			printf("Can't find this command��\n");
		}

	}
	
	return 0;

}




void create_fileSystem(void){

	//�����ļ�ϵͳ�Ĵ洢λ�� ���������
	//inode��1��ʼ��block��0��ʼ 
	last_inode_bit=1;
    last_block_bit=0;
	int i = 0;
	
   	printf("Please wait..\n");
	while(i<20){
		printf(".");
		Sleep(100);
		i++;
	}
	
	i=0;
	//��fileSystem.dat��Ϊģ����� 
	pf = fopen("fileSystem.dat", "w+b");

	if(!pf){
		printf("open file filed!"); //���ļ�ʧ�� 
		exit(0);
	}
	
	
	//��buffer��գ��öԴ��̽��и�ʽ�� 
	for(i=0; i<BLOCK_SIZE; i++){		
		block_buffer[i] = 0;
		//printf("%d", i); 
	}

	//��ʽ������
	fseek(pf, 0, SEEK_SET); 
	for(i=0; i < BLOCK_COUNT; i++){		
		fwrite(block_buffer, BLOCK_SIZE, 1, pf);  //�����д��0���и�ʽ�� 
	}
	fflush(pf);
	
	 
//	fseek(pf, INODE_TABLE_START, SEEK_SET);
//	fread(inode_table_buffer, INODE_TABLE_SIZE, 1, pf); 
//	for(i=0; i < INODE_TABLE_SIZE; i++){
//		printf("%d", inode_table_buffer[i]);
//	} 
	

	//��ʼ��super_block, ��д������е�block 
	read_super_block();
	strcpy(super_block_buffer->sb_volume_name, VOLUME_NAME);
	super_block_buffer->sb_inodes_count = 8192;
	super_block_buffer->sb_blocks_count = 8708;
	super_block_buffer->sb_free_inodes_count = 8192-1; //inode�Ŵ�1��ʼ�����Կ��е���һ�� 
	super_block_buffer->sb_free_blocks_count = 8708;
	super_block_buffer->sb_block_size = BLOCK_SIZE;
	super_block_buffer->sb_inode_size = 64;
	current_time();
	strcpy(super_block_buffer->sb_wtime, time_now);
	write_super_block();

	

	
	//��ʼ��block_group_desc, ��д����� 
	read_block_group_desc();
	block_group_desc_buffer->bgd_block_bitmap_start = BLOCK_BITMAP_START;
	block_group_desc_buffer->bgd_inode_bitmap_start = INODE_BITMAP_START;
	block_group_desc_buffer->bgd_inode_table_start = INODE_TABLE_START;
	block_group_desc_buffer->bgd_free_inode_count = 1024*8-1;
	block_group_desc_buffer->bgd_free_block_count = 1024*8;
	block_group_desc_buffer->bgd_used_dirs_count = 0;
	write_block_group_desc();
	
	//��ʽʱ�ѳ�ʼ��block bitmap
	//���뵽block_bitmap_buffer������ 
	read_block_bitmap();
	//���� 
//	for(i=0; i<1024; i++){
//		printf("%d", block_bitmap_buffer[i]);
//	}
	//��ʽʱ�ѳ�ʼ��inode bitmap 
	//���뵽inode_bitmap_buffer������ 
	read_inode_bitmap();
	//���� 
//	for(i=0; i<1024; i++){
//		printf("%d", inode_bitmap_buffer[i]);
//	}

	//���ø�Ŀ¼
	read_inode(current_dir_inode);//����һ��inode����inode_buffer
	
	inode_buffer->i_mode = 755; //rwx r-x r-x
	inode_buffer->i_size = 16*2; //һ��blockռ1024bytes 
	inode_buffer->i_atime = 0;
	current_time();
	strcpy(inode_buffer->i_ctime, time_now);
	inode_buffer->i_mtime = 0;
	inode_buffer->i_dtime = 0;
	inode_buffer->i_blocks_count = 1;
	inode_buffer->i_block[0] = get_free_block(); //����һ�����е�block bit 
	
	current_dir_inode = get_free_inode(); //����һ�����е�inode bit 
	current_dir_length = 1; //�������ĳ��ȡ�/�� 


	//printf("%u\n", current_dir_inode);
	write_inode(current_dir_inode);

	
	
	read_dir(0); //д�������� 
	//��ʼ����Ŀ¼��..���͡�.�� 
	dir[0].inode = dir[1].inode = current_dir_inode;
	dir[0].name_len = 1;
	dir[1].name_len = 1;
	dir[0].file_type = dir[1].file_type = 2;
	strcpy(dir[0].name, ".");
	strcpy(dir[1].name, "..");
	for(i=2; i<64; i++){
		dir[i].inode = 0;
	}
	write_dir(inode_buffer->i_block[0]);

	printf("\nthe ext2 file system has been installed!\n\n\n");

	fclose(pf);
}


//��д super block 
void read_super_block(void){
	fseek(pf, SUPER_BLOCK_START, SEEK_SET); 
	fread(super_block_buffer, SUPER_BLOCK_SIZE, 1, pf);
}

void write_super_block(void){
	fseek(pf, SUPER_BLOCK_START, SEEK_SET); 
	fwrite(super_block_buffer, SUPER_BLOCK_SIZE, 1, pf);
	fflush(pf);
}


//��д block_group_desc 
void read_block_group_desc(void){
	fseek(pf, BLOCK_GROUP_DES_START, SEEK_SET); 
	fread(block_group_desc_buffer, BLOCK_GROUP_DES_SIZE, 1, pf);
}

void write_block_group_desc(void){
	fseek(pf, BLOCK_GROUP_DES_START, SEEK_SET); 
	fwrite(block_group_desc_buffer, BLOCK_GROUP_DES_SIZE, 1, pf);
	fflush(pf);
}


//��дblock_buffer 
void read_block_bitmap(void){
	fseek(pf, BLOCK_BITMAP_START, SEEK_SET); 
	fread(block_bitmap_buffer, BLOCK_SIZE, 1, pf);
}

void write_block_bitmap(void){
	fseek(pf, BLOCK_BITMAP_START, SEEK_SET); 
	fwrite(block_bitmap_buffer, BLOCK_SIZE, 1, pf);
	fflush(pf);
}



//��дinode_buffer 
void read_inode_bitmap(void){
	fseek(pf, INODE_BITMAP_START, SEEK_SET); 
	fread(inode_bitmap_buffer, BLOCK_SIZE, 1, pf);
}

void write_inode_bitmap(void){
	fseek(pf, INODE_BITMAP_START, SEEK_SET); 
	fwrite(inode_bitmap_buffer, BLOCK_SIZE, 1, pf);
	fflush(pf);
}


//��inode table�ж�дָ��λ�õ�inode 
void read_inode(int inode_num){
	fseek(pf, INODE_TABLE_START+(inode_num-1)*INODE_SIZE, SEEK_SET);
	fread(inode_buffer, INODE_SIZE, 1, pf); 
}

void write_inode(int inode_num){
	fseek(pf, INODE_TABLE_START+(inode_num-1)*INODE_SIZE, SEEK_SET);
	fwrite(inode_buffer, INODE_SIZE, 1, pf); 
	fflush(pf);
}

//��block data�ж�д���� 
void read_block(int block_num){
	fseek(pf, DATA_BLOCK_START+block_num*BLOCK_SIZE, SEEK_SET);
	fread(block_buffer, BLOCK_SIZE, 1, pf); 
}

void write_block(int block_num){
	fseek(pf, DATA_BLOCK_START+block_num*BLOCK_SIZE, SEEK_SET);
	fwrite(block_buffer, BLOCK_SIZE, 1, pf); 
	fflush(pf);
}

//��data block�ж�дdir��Ϣ 
void read_dir(int block_num){
	fseek(pf, DATA_BLOCK_START+block_num*BLOCK_SIZE, SEEK_SET);
	fread(dir, BLOCK_SIZE, 1, pf); 
} 

void write_dir(int block_num){
	fseek(pf, DATA_BLOCK_START+block_num*BLOCK_SIZE, SEEK_SET);
	fwrite(dir, BLOCK_SIZE, 1, pf);
	fflush(pf);
} 


//���ص�ǰʱ�� 
void current_time(void){
	time_t t = time(0); //��ȡ��ǰϵͳ��ʱ�� 
	strftime(time_now, sizeof(time_now), "%Y-%m-%d %H:%M:%S", localtime(&t));
}

//����һ�����е�block 
//block_bitmap_buffer[]   block map������ 
unsigned int get_free_block(void){

	unsigned int temp_block_bit = last_block_bit;  //��¼��һ�η����block�� 
	unsigned int temp = temp_block_bit/8;  //8bit = 1char 
	char flag = 0;
	unsigned char con = 128;
	read_block_group_desc();
	
	//���û��ʣ��blockֱ������ 
	if(block_group_desc_buffer->bgd_free_block_count==0){
		printf("there is no free block.\n");
		return -1; 
	}
	
	//��block bitmap������  block_bitmap_buffer[] 
	//�鿴�Ƿ��ĸ��ֽ���bitλ��ȱ����Ϊһ���ļ�һ����8�������� 
	read_block_bitmap();
	while(block_bitmap_buffer[temp] == 255){
		if(temp == 1023)
			temp=0;
		else
			temp++;
	}
	
	//��λ��ķ������õ����е�bitλ 
	while(block_bitmap_buffer[temp]&con){
		flag++;
		con /= 2;
	}

	//�õ�bitλ֮��ֱ���޸�����charֵ 
	block_bitmap_buffer[temp] = block_bitmap_buffer[temp]+con; 
	last_block_bit = temp*8+flag;
	write_block_bitmap(); //���µ�block bitmap 
	
	//���µ�Super Block 
	read_super_block(); 
	super_block_buffer->sb_free_blocks_count--;
	write_super_block();
	
	 //���µ�GDT 
	block_group_desc_buffer->bgd_free_block_count--;
	write_block_group_desc();  
	//printf("%u", last_block_bit);  //������� 
	return last_block_bit;
}

//����һ�����е�inode�� 
//inode_bitmap_buffer[] 
unsigned int get_free_inode(void){
	unsigned int temp_inode_bit = last_inode_bit; //�ʼ����1 
	unsigned int temp = (temp_inode_bit-1)/8;  //8bit = 1char 

	char flag = 0;
	unsigned char con = 128; //1000 0000b
	
	read_block_group_desc();
	if(block_group_desc_buffer->bgd_free_inode_count==0){
		printf("there is no free inode.\n");
		return -1; 
	}

	//��inode bitmap������  inode_bitmap_buffer[] 
	read_inode_bitmap();
	while(inode_bitmap_buffer[temp] == 255){
		if(temp == 1023)
			temp=0;
		else
			temp++;
	}
	
//	printf("temp_inode_bit: %d\n", temp_inode_bit);
//	printf("temp: %d\n", temp);
//	printf("inode_bitmap_buffer[temp]: %d\n", inode_bitmap_buffer[temp]);
//	printf("con: %d\n", con);
	//��whileд����if,���� 
	while(inode_bitmap_buffer[temp]&con){
		flag++;
		con /= 2;
		//printf("con: %d\n", con);
	}

	inode_bitmap_buffer[temp] = inode_bitmap_buffer[temp]+con; 
	//printf("inode_bitmap_buffer[temp]: %d", inode_bitmap_buffer[temp]);
	last_inode_bit = temp*8+flag+1;
	write_inode_bitmap(); //���µ�inode bitmap 
	
	//���µ�Super Block 
	read_super_block(); 
	super_block_buffer->sb_free_inodes_count--;
	write_super_block();

	block_group_desc_buffer->bgd_free_inode_count--;
	write_block_group_desc(); //���µ��������� 
	
	//printf("%d\n", flag);
	//printf("%d\n", con);
	//printf("%u\n", last_inode_bit); 
	return last_inode_bit;
} 

//ɾ�� block 
void remove_block(int remove_block_bit){
	unsigned temp = remove_block_bit/8;
	
	read_block_bitmap();
	//�ҵ�bitλ�������ֽ�Ȼ����ض�����������Ϳ��Խ�����0 
	switch(remove_block_bit%8){
		case 0: block_bitmap_buffer[temp]&127;break; //0111 1111
		case 1: block_bitmap_buffer[temp]&191;break; //1011 1111
		case 2: block_bitmap_buffer[temp]&223;break; //1101 1111
		case 3: block_bitmap_buffer[temp]&239;break; //1110 1111
		case 4: block_bitmap_buffer[temp]&247;break; //1111 0111
		case 5: block_bitmap_buffer[temp]&251;break; //1111 1011
		case 6: block_bitmap_buffer[temp]&253;break; //1111 1101
		case 7: block_bitmap_buffer[temp]&254;break; //1111 1110
	}
	
	//����Block Bitmap 
	write_block_bitmap();
	
	//����GDT 
	read_block_group_desc();
	block_group_desc_buffer->bgd_free_block_count++;
	write_block_group_desc();
	
	//����Super Block 
	read_super_block(); 
	super_block_buffer->sb_free_blocks_count--;
	write_super_block();
}

//ɾ��inode 
void remove_inode(int remove_inode_bit){
	unsigned temp = (remove_inode_bit-1)/8;
	
	read_inode_bitmap();
	switch((remove_inode_bit-1)%8){
		case 0: inode_bitmap_buffer[temp]&127;break; //0111 1111
		case 1: inode_bitmap_buffer[temp]&191;break; //1011 1111
		case 2: inode_bitmap_buffer[temp]&223;break; //1101 1111
		case 3: inode_bitmap_buffer[temp]&239;break; //1110 1111
		case 4: inode_bitmap_buffer[temp]&247;break; //1111 0111
		case 5: inode_bitmap_buffer[temp]&251;break; //1111 1011
		case 6: inode_bitmap_buffer[temp]&253;break; //1111 1101
		case 7: inode_bitmap_buffer[temp]&254;break; //1111 1110
	}
	
	//����Inode Bitmap 
	write_inode_bitmap();
	
	//����GDT 
	read_block_group_desc();
	block_group_desc_buffer->bgd_free_inode_count++;
	write_block_group_desc();
	
	//����Super Block 
	read_super_block(); 
	super_block_buffer->sb_free_inodes_count++;
	write_super_block();
}

//��ʾ��ǰĿ¼���ļ���Ϣ 
void ls(){
	printf("%-15s %-10s %-5s %-13s %-22s %-10s %-10s\n", "name", "type", "user", "group", "create_time", "mode", "size");
	read_inode(current_dir_inode); //��ȡ��ǰĿ¼�Ľڵ���Ϣ 
//	printf("current_dir_inode: %d\n", current_dir_inode);
//	printf("i_size: %u\n", inode_buffer->i_size);

	unsigned int i=0, k=0, temp=0, j=0, n=0;
	char mode[9]="";
//	for(i=0; i< 8; i++){
//		printf("%d\n", inode_buffer->i_block[i]);
//	}
	i=0;
	while(i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			if(dir[k].inode){ //�ж��Ƿ���� 
				printf("%-15s", dir[k].name); 
				if(dir[k].file_type == 1){
					printf("%-12s", "<File>");
				}else if(dir[k].file_type == 2){
					printf("%-12s", "<Dir>");
				}
				printf("%-5s %-10s", "root", "root");
				read_inode(dir[k].inode);
				printf("%-25s", inode_buffer->i_ctime);
				temp = inode_buffer->i_mode;
				//printf("%d", temp);
				j=0, n=100;
				//1 2 4 
				while(1){
					switch(temp/n){
						case 0:strcpy(mode+j, "---");break;
						case 1:strcpy(mode+j, "r--");break; 
						case 2:strcpy(mode+j, "-w-");break; 
						case 3:strcpy(mode+j, "rw-");break; 
						case 4:strcpy(mode+j, "--x");break; 
						case 5:strcpy(mode+j, "r-x");break; 
						case 6:strcpy(mode+j, "-wx");break; 
						case 7:strcpy(mode+j, "rwx");break; 
					}
					if(n==1){
						break;
					}
					temp %= n;
					n /= 10;
					j += 3;
				}
				printf("%-15s", mode);
				printf("%d\n", inode_buffer->i_size);
				
			} 
			k++; 
		}
		i++;
		read_inode(current_dir_inode);
	}
} 

//�½�Ŀ¼��׼������ 
void dir_prepare(unsigned int dir_inode, int dir_len){
	read_inode(dir_inode);
//	printf("dir_inode: %d\n", dir_inode);
//	printf("current_dir_inode: %d\n", current_dir_inode);
//	printf("dir_len: %d\n", dir_len);
//	printf("current_dir_length: %d\n", current_dir_length);
	inode_buffer->i_mode=755;  
	inode_buffer->i_size=32; 
	inode_buffer->i_blocks_count=1; //ռ�õ�block������ 
	inode_buffer->i_block[0] = get_free_block();
	current_time();
	strcpy(inode_buffer->i_ctime, time_now);
	//printf("inode_buffer->i_block[0]:%d\n", inode_buffer->i_block[0]);
	read_dir(inode_buffer->i_block[0]);
	dir[0].inode = dir_inode;
	dir[1].inode = current_dir_inode; //��ǰĿ¼��inode 
	dir[0].name_len = dir_len;
	dir[1].name_len = current_dir_length;
	dir[0].file_type = dir[1].file_type = 2;
	//���Ǻܹؼ���һ������Ȼһֱ����ʾ�ϴ���������Ϣ 
	int i=0;
	for(i=2; i<64; i++){
		dir[i].inode=0;
	}
	strcpy(dir[0].name, ".");
	strcpy(dir[1].name, "..");
	write_dir(inode_buffer->i_block[0]);
	write_inode(dir_inode);
	//����GDT
	read_block_group_desc();
	block_group_desc_buffer->bgd_used_dirs_count++;
	write_block_group_desc();
}

//������Ŀ¼ 
void mkdir(char temp[9]){
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int i=0, k=0, dir_inode=0, flag=1;
	//printf("current_dir_inode: %d\n", current_dir_inode);

//	for(i=0; i<9; i++){
//		printf("%c", temp[i]); 
//	} 
	
	//�ж��Ƿ���ͬ�����������˳� 
	while(i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			//����ڵ���ڲ���������ͬ 
			//�����ǲ����ڣ���Ҫע��һ�� 
			if(dir[k].inode && !strcmp(dir[k].name, temp)){
				printf("filename has already existed! \n");
				return;
			}
			k++; 
		}
		i++;
	} 
	//k����������ʾ��һ�����õ�Ŀ¼��inode�� i��ʾ��һ�����õ�block[i] 
//	printf("k: %d\n", k); 
//	printf("i: %d\n", i); 
	
	if(inode_buffer->i_size == 1024*8){
		printf("Directory has no room to be alloced!\n");
		return; 
	} 
	
	flag=1;
	read_inode(current_dir_inode);
	//�ж�һ��Ŀ¼�����Ƿ��п��� 
	if(inode_buffer->i_size != (inode_buffer->i_blocks_count)*1024){
		i=0;
		//printf("i: %d\n", i);
		//�ѿ���block�ҵ� 
		while(flag && i < inode_buffer->i_blocks_count){
			read_dir(inode_buffer->i_block[i]);
//			printf("inode_buffer->i_block[i]: %d\n", inode_buffer->i_block[i]);
//			for(k=0; k<64; k++){
//				printf("%d", dir[k].inode);
//			}
			k=0;
			while(k < 64){
				if(dir[k].inode == 0){
					flag=0; //����пռ�ֱ����������ѭ�� 
					break;
				}
				//printf("\nk: %d\n", k);
				k++; 
			}
			i++;
		} 
		
		//printf("k: %d\n", k);
		dir_inode = get_free_inode();
		dir[k].inode = dir_inode;
		dir[k].name_len = strlen(temp);
		dir[k].file_type=2;
		strcpy(dir[k].name, temp);

		//printf("inode: %d\n", dir_inode);
		write_dir(inode_buffer->i_block[i-1]);
		
	}else{
		inode_buffer->i_block[inode_buffer->i_blocks_count] = get_free_block();
		inode_buffer->i_blocks_count++;
		read_dir(inode_buffer->i_block[inode_buffer->i_blocks_count-1]);
		dir_inode = get_free_inode();
		dir[0].inode = dir_inode;
		dir[0].name_len = strlen(temp);
		dir[0].file_type=2;
		strcpy(dir[0].name, temp);
		for(k=1; k<64; k++){
			dir[k].inode=0;
		}
		
		//printf("%d", dir_inode);
		write_dir(inode_buffer->i_block[i-1]);	
	}
	

	inode_buffer->i_size += 16;
	write_inode(current_dir_inode);
	//Ϊÿ���¼�Ŀ¼��Ӷ�����Ϣ 
//	printf("dir_inode: %d\n", dir_inode);
	dir_prepare(dir_inode, strlen(temp));
}

//�л�Ŀ¼
void cd(char temp[9]){
	int i=0, k=0;
//	printf("%u", current_dir_inode); 
//	fseek(pf, INODE_TABLE_START, SEEK_SET);
//	fread(inode_table_buffer, INODE_TABLE_SIZE, 1, pf); 
//	for(i=0; i < INODE_TABLE_SIZE; i++){
//		printf("%d", inode_table_buffer[i]);
//	} 
//	temp[strlen(temp)] = '\0';
//	printf("%d", strlen(temp));
	if(!strcmp(temp, ".")){
		//printf("test");//ɶ������ 
		return;
	}else if(!strcmp(temp, "..")){
		read_inode(current_dir_inode); //��ȡ��ǰĿ¼�Ľڵ���Ϣ 
		
		while(i < inode_buffer->i_blocks_count){
			read_dir(inode_buffer->i_block[i]); //��ȡÿһ��blockָ������Ӧ��dir 
			k=0;
			while(k < 64){
				if(!strcmp(dir[k].name, "..")){
					current_dir_inode = dir[k].inode;
					path_name[strlen(path_name)-dir[k-1].name_len-1] = '\0';
					current_dir_length = dir[k].name_len;
					//printf("%d", current_dir_inode);
					//  / sking/123/123/ 
					return;
				}
				k++;
				
			}
			i++;
		}
	}

	
	read_inode(current_dir_inode); //��ȡ��ǰĿ¼�Ľڵ���Ϣ 
	while(i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]); //��ȡÿһ��blockָ������Ӧ��dir 
		k=0;
		while(k < 64){
			if(!strcmp(dir[k].name, temp) && dir[k].inode && dir[k].file_type==2){
				current_dir_inode = dir[k].inode;
				current_dir_length = dir[k].name_len;
				//printf("%d", current_dir_inode); 
				
				strcat(path_name, temp);
				strcat(path_name, "/");
				return;
			}
			k++;
			
		}
		i++;
	}
	printf("Can't find this Directory! \n");


}

//��ʽ������ 
void format(){
	char answer = 'Y';
	while(1){

		scanf("%c", &answer);
		if(toupper(answer) == 'Y'){
			printf("Ready to format......\n"); 
			create_fileSystem();
			pf = fopen("fileSystem.dat", "r+b");
			last_inode_bit=1;  
			last_block_bit=0; 
			read_super_block();
			read_block_group_desc();
		
			//Ŀ¼�� 
			strcpy(current_path, "[root@sking ");
			strcpy(path_name, "/");
			strcpy(path_last, "]# ");
			
			current_dir_inode=1; 
			current_dir_length=1;
			return;
		}else if(toupper(answer) == 'N'){
			return;
		}else{
			fflush(stdin);
		}
		printf("Are you sure you want to format the disk ?[y / n]: ");

	}
	
	
}

//ɾ����Ŀ¼ 
void rmdir(char temp[9]){
	int i=0, k=0, flag=0;
	
	if(!strcmp(temp, "..") || !strcmp(temp, ".")) {
		printf("This directory is not allowed to be deleted!\n");
		return;
	}
	
	read_inode(current_dir_inode);

	while(!flag && i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			if(dir[k].inode && !strcmp(dir[k].name, temp) && dir[k].file_type==2){
				flag = 1; //�ҵ���Ŀ¼�� 
				break;
			}
			k++; 
		}
		i++;
	} 
	
	if(!flag){
		printf("Please enter the correct directory name!\n");
		return;
	}else{
		//������Ҫɾ���Ľڵ���Ϣ 
		read_inode(dir[k].inode);
		//�ж��Ƿ�Ϊ��Ŀ¼ 
		//ֻ�� .. �� . Ŀ¼ 
//			unsigned int i_mode;  //�ļ����ͺ�Ȩ�� 
//			unsigned int i_size;  //�ļ�����Ŀ¼��С 
//			unsigned int i_blocks_count; //ռ�õ�block������ 
//			unsigned short i_block[8];  //����ָ��, ���block�� 
		if(inode_buffer->i_size == 32){
			inode_buffer->i_mode = 0;
			inode_buffer->i_size = 0;
			inode_buffer->i_blocks_count = 0;
			inode_buffer->i_size = 0;
			//����Ŀ¼�µ������ļ�ɾ�� 
			read_dir(inode_buffer->i_block[0]);
			dir[0].inode=0;
			dir[1].inode=0;
			write_dir(inode_buffer->i_block[0]);
			//ɾ��block�� 
			remove_block(inode_buffer->i_block[0]);
			//ɾ����Ŀ¼�µ����Ŀ¼ 
			read_inode(current_dir_inode); 
			read_dir(inode_buffer->i_block[i-1]);
			dir[k].inode=0;
			write_dir(inode_buffer->i_block[i-1]);
			inode_buffer->i_size -= 16;
			
			//����Щ����block���ǿյ����ֿ�ȥ��
			 i=1;
			 flag=0;
			 while(flag<64 && i<inode_buffer->i_blocks_count){
			 	k=0;
			 	read_dir(inode_buffer->i_block[i]);
			 	while(k<64){
			 		if(!dir[k].inode){
			 			flag++;
			 		}
			 		k++;
			 	}
			 	if(flag==64){
			 		remove_block(inode_buffer->i_block[i]);
			 		inode_buffer->i_blocks_count--;
			 		while(i<inode_buffer->i_blocks_count){
			 			inode_buffer->i_block[i] = inode_buffer->i_block[i+1];
						i++;
			 		}
			 	}
			 	
			 }
			 write_inode(current_dir_inode);
			 
			
			
			
		}else{
			printf("Can't delete, directory is not null! ");
			return;
		}
	}

}

//�˳���¼ 
void logout(void){
	char answer = 'Y';
	while(1){
		scanf("%c", &answer);
		if(toupper(answer) == 'Y'){
			printf("\nbye bye ~~\n");
			exit(0);
		}else if(toupper(answer) == 'N'){
			return;
		}else{
			fflush(stdin);
		}
		printf("Are you sure you want to quit ?[y / n]: ");
	}
}

//��ʾ�������� 
void help(){
    printf("      *****************************************************************************************\n");
    printf("      *                                    command help                                       *\n");
    printf("      *                                                                                       *\n");
    printf("      * 01.command help  : help                  09.format disk       : format                *\n");
    printf("      * 02.create dir    : mkdir + dir_name      10.delete empty dir  : rmdir + dir_name      *\n");
    printf("      * 03.list dir      : ls                    11.chang dir         : cd + dir_name         *\n");
    printf("      * 04.edit file     : vi + file_name        12.read file         : cat + file_name       *\n");
    printf("      * 05.remove file   : rm + file_name        13.ping IP/Address   : ping + ip/host        *\n");
    printf("      * 06.list local IP : ifconfig              14.traceroute        : traceroute + ip/host  *\n");   
    printf("      * 07.show data now : data                  15.modify mode       : chmod + mode          *\n");
    printf("      * 08.logout        : logout                16.display disk info : dumpe2fs              *\n");
    printf("      *                                                                                       *\n");
    printf("      *****************************************************************************************\n\n");
} 

//��ʾ������Ϣ 
void dumpe2fs(){
	read_super_block();
	printf("volume name            : %s\n", super_block_buffer->sb_volume_name);
	printf("inodes counts          : %d\n", super_block_buffer->sb_inodes_count);
	printf("blocks counts          : %d\n", super_block_buffer->sb_blocks_count);
	printf("free inodes counts     : %d\n", super_block_buffer->sb_free_inodes_count);
	printf("free blocks counts     : %d\n", super_block_buffer->sb_free_blocks_count);
	printf("inode size             : %d(kb)\n", super_block_buffer->sb_inode_size);
	printf("block size             : %d(kb)\n", super_block_buffer->sb_block_size);
	printf("create time            : %s\n\n", super_block_buffer->sb_wtime);

}

void vi(char temp[9]){ 
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int i=0, k=0, flag=1, m=0, file_inode=0;
	int local=0, file_length=0, file_block_count=0;
	
	//Ѱ���ļ��Ƿ���ڣ������Ŀ¼�����ܽ��б༭
	//������ļ�������ڽ��б༭�������ڽ��д��� 
	while(flag && i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			if(dir[k].inode && !strcmp(dir[k].name, temp)){
				if(dir[k].file_type == 2){
					printf("Directory can't be edit! \n");
					return;
				}else{
					flag = 0; //�����ļ� 
					break;
				}
			}
			k++; 
		}
		i++;
	} 
	
	//����ļ�������, ���������ļ� 
	if(flag){
		printf("Can't find the file name, will create a new file!\n");
		printf("Please input '\\q' to quit! \n\n");
		read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
		i=0, k=0, file_inode=0, flag=1;
		
		//�Ƿ���block 
		if(inode_buffer->i_size == 1024*8){
			printf("Directory has no room to be alloced!\n");
			return; 
		} 

		m=1; //������ֹѭ�� 
		//�ж�һ��Ŀ¼�����Ƿ��п��� 
		if(inode_buffer->i_size != (inode_buffer->i_blocks_count)*1024){
			i=0;
			while(m && i < inode_buffer->i_blocks_count){
				read_dir(inode_buffer->i_block[i]);
				k=0;
				while(k < 64){
					if(dir[k].inode == 0){
						m=0; //����пռ�ֱ����������ѭ�� 
						break;
					}
					//printf("\nk: %d\n", k);
					k++; 
				}
				i++;
			} 
			
			//printf("k: %d\n", k);
			file_inode = get_free_inode();
			dir[k].inode = file_inode;
			dir[k].name_len = strlen(temp);
			dir[k].file_type=1;
			strcpy(dir[k].name, temp);
	
			//printf("inode: %d\n", file_inode);
			write_dir(inode_buffer->i_block[i-1]);
			
		}else{
			inode_buffer->i_block[inode_buffer->i_blocks_count] = get_free_block();
			inode_buffer->i_blocks_count++;
			read_dir(inode_buffer->i_block[inode_buffer->i_blocks_count-1]);
			file_inode = get_free_inode();
			dir[0].inode = file_inode;
			dir[0].name_len = strlen(temp);
			dir[0].file_type=1;
			strcpy(dir[0].name, temp);
			for(k=1; k<64; k++){
				dir[k].inode=0;
			}
		
			//printf("%d", file_inode);
			write_dir(inode_buffer->i_block[i-1]);	
		}
		
		inode_buffer->i_size += 16;
		write_inode(current_dir_inode);//д�뵱ǰĿ¼ 
		
		//д���ļ���ʼ����Ϣ 
		read_inode(file_inode);
		inode_buffer->i_mode = 777;
		inode_buffer->i_size = 0;
		inode_buffer->i_blocks_count = 0;
		current_time();
		strcpy(inode_buffer->i_ctime, time_now);

		
		//��ʼд���ļ�����filebuffer��
		while(1){
			filebuffer[local] = getchar();
			if(filebuffer[local] == 'q' && filebuffer[local-1]=='\\'){
				filebuffer[local-1] = '\0';
				break;
			}

			if(local>=8191){
				printf("Sorry,the max size of a file is 4KB!\n"); 
				break;
			}
			
			local++;
		}
		
//		for(i=0; i<local-1; i++){
//			printf("%c", filebuffer[i]);
//		}
//		printf("\n");

		file_length = strlen(filebuffer); //�ļ����ݵĳ���
		file_block_count = file_length/1024;
		if(file_length%1024){
			file_block_count++;
		} 
//		printf("file_block_count: %d\n", file_block_count);
		//ֱ�Ӹ���д��
		for(i=0; i<file_block_count; i++){
			inode_buffer->i_blocks_count++;
			inode_buffer->i_block[i] = get_free_block();
			read_block(inode_buffer->i_block[i]); //�����ݶ��뻺����
			if(i==file_block_count-1)
				memcpy(block_buffer, filebuffer+i*BLOCK_SIZE, file_length-i*BLOCK_SIZE);
			else
				memcpy(block_buffer, filebuffer+i*BLOCK_SIZE, BLOCK_SIZE);
			write_block(inode_buffer->i_block[i]);
		}
		
		inode_buffer->i_size=file_length;
//		printf("file_inode: %d", file_inode);
//		printf("inode_buffer->i_blocks_count: %d\n", inode_buffer->i_blocks_count);
		write_inode(file_inode);
		printf("\nSave as ");
		for(i=0; i<strlen(temp); i++){
			printf("%c", temp[i]);
		} 
		printf("!\n");
	//�ļ�����ʱ 
	}else{
		fflush(stdin);
		printf("The file is exist!!\n");
		printf("Please input '\\q' to quit! \n\n");
		read_inode(dir[k].inode); //��ȡ��ǰ�ڵ���Ϣ,����inode_buffer
//		printf("dir[k].inode: %d\n", dir[k].inode);
//		printf("inode_buffer->i_blocks_count: %d\n", inode_buffer->i_blocks_count);

		for(i=0; i<inode_buffer->i_blocks_count; i++){
			read_block(inode_buffer->i_block[i]); //����block_buffer
			if(i == inode_buffer->i_blocks_count-1){
				memcpy(filebuffer+i*BLOCK_SIZE, block_buffer, inode_buffer->i_size-i*BLOCK_SIZE);	
			}else{
				memcpy(filebuffer+i*BLOCK_SIZE, block_buffer, i*BLOCK_SIZE);	
			}
			remove_block(inode_buffer->i_block[i]);	
		} 
		
		for(i=0; i<inode_buffer->i_size; i++){
			printf("%c", filebuffer[i]);
		}
		
		inode_buffer->i_blocks_count=0;
		local = inode_buffer->i_size;
		while(1){
			if(local>=8191){
				printf("Sorry,the max size of a file is 4KB!\n"); 
				break;
			}
			
			filebuffer[local] = getchar();
			if(filebuffer[local] == 'q' && filebuffer[local-1]=='\\'){
				filebuffer[local-1] = '\0';
				break;
			}
			
			local++;
		}
		

		file_length = strlen(filebuffer); //�ļ����ݵĳ���
		file_block_count = file_length/1024;
		if(file_length%1024){
			file_block_count++;
		}

		for(i=0; i<file_block_count; i++){
			inode_buffer->i_blocks_count++;
			inode_buffer->i_block[i] = get_free_block();
			read_block(inode_buffer->i_block[i]); //�����ݶ��뻺����
			if(i==file_block_count-1)
				memcpy(block_buffer, filebuffer+i*BLOCK_SIZE, file_length-i*BLOCK_SIZE);
			else
				memcpy(block_buffer, filebuffer+i*BLOCK_SIZE, BLOCK_SIZE);
			write_block(inode_buffer->i_block[i]);
		}
		
		inode_buffer->i_size=file_length;
		//����ʱ�� 
		current_time();
		strcpy(inode_buffer->i_ctime, time_now);
//		printf("file_inode: %d", file_inode);
//		printf("inode_buffer->i_blocks_count: %d\n", inode_buffer->i_blocks_count);
		write_inode(dir[k].inode);
		
		printf("\nSave!\n");
	} 

} 

void cat(char temp[9]){
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int i=0, k=0, flag=1;
	
	//Ѱ���ļ��Ƿ���ڣ������Ŀ¼�����ܽ��в鿴 
	while(flag && i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			if(dir[k].inode && !strcmp(dir[k].name, temp)){
				if(dir[k].file_type == 2){
					printf("That is a directory! \n");
					return;
				}else{
					flag = 0; //�����ļ� 
					break;
				}
			}
			k++; 
		}
		i++;
	} 
	if(!flag){
		read_inode(dir[k].inode); //��ȡ��ǰ�ڵ���Ϣ,����inode_buffer
		for(i=0; i<inode_buffer->i_blocks_count; i++){
			read_block(inode_buffer->i_block[i]); //����block_buffer
			if(i == inode_buffer->i_blocks_count-1){
				memcpy(filebuffer+i*BLOCK_SIZE, block_buffer, inode_buffer->i_size-i*BLOCK_SIZE);	
			}else{
				memcpy(filebuffer+i*BLOCK_SIZE, block_buffer, i*BLOCK_SIZE);	
			}
			remove_block(inode_buffer->i_block[i]);	
		} 
		
		for(i=0; i<inode_buffer->i_size; i++){
			printf("%c", filebuffer[i]);
		}
		printf("\n");
	}else{
		printf("Can't find the filename!\n");
		return;
	}

} 

void rm(char temp[9]){
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int i=0, k=0, flag=1;
	int m=0, n=0;
	
	//�ж��ļ��Ƿ���ڣ����ļ����ڲſ�ɾ�� 
	while(flag && i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			if(dir[k].inode && !strcmp(dir[k].name, temp)){
				if(dir[k].file_type == 2){
					printf("'rm' only delete file! \n");
					return;
				}else{
					flag = 0; //�����ļ� 
					break;
				}
			}
			k++; 
		}
		i++;
	} 
	
	//flag = 1 �ļ������� 
	if(flag){
		printf("Can't find this filename!\n"); 
		return;
	}else{
		read_dir(inode_buffer->i_block[i]);
		read_inode(dir[k].inode); //����inode
		//�ѵ�ǰ�ļ���inode��Ϣɾ�� 
		for(i=0; i<inode_buffer->i_blocks_count; i++){
			remove_block(inode_buffer->i_block[i]);
		}
		inode_buffer->i_mode = 0;
		inode_buffer->i_size = 0;
		inode_buffer->i_blocks_count=0;
		
		//ɾ����Ŀ¼�´��ļ�����Ϣ
		read_inode(current_dir_inode);
		inode_buffer->i_size -= 16;
		read_dir(inode_buffer->i_block[i]);
		dir[k].inode = 0;
		write_dir(inode_buffer->i_block[i]);
		
		m=1;
		while(m < inode_buffer->i_blocks_count){
			read_dir(inode_buffer->i_block[m]);
			flag=n=0;
			while(n<64){
				if(!dir[n].inode){
					flag++;
				}
				n++;
			}
			
			if(flag == 64){
				remove_block(inode_buffer->i_block[m]);
				inode_buffer->i_blocks_count--;
				while(m < inode_buffer->i_blocks_count){
					inode_buffer->i_block[m] = inode_buffer->i_block[m+1];
					m++;
				}
			}
			m++;
			
		}
		write_inode(current_dir_inode);
		printf("The file has been deleted!\n");
		
	}
} 

void ifconfig(){
     char host_name[256]; 
     int WSA_return, i;
     WSADATA WSAData;
     HOSTENT *host_entry; 
     WORD wVersionRequested;

     wVersionRequested = MAKEWORD(2, 0);
     WSA_return = WSAStartup(wVersionRequested, &WSAData); 

     if (WSA_return == 0){
         gethostname(host_name, sizeof(host_name));
         host_entry = gethostbyname(host_name);
		 printf("\t%-15s\t  %s\n", "IP", "hostname");
         for(i = 0; host_entry != NULL && host_entry->h_addr_list[i] != NULL; ++i){
             const char *pszAddr = inet_ntoa (*(struct in_addr *)host_entry->h_addr_list[i]);
             printf("   %-24s%s\n", pszAddr, host_name);
         }
     }
     else{
         printf("Please check network!\n");
     }
     WSACleanup();
}

void ping(char ip[128]){
	char address[150];
	strcpy(address, "ping -n 4 ");
	strcat(address, ip);
	char buffer[128];
	
//	for(int i=0; i<128; i++){
//		printf("%c", ip[i]);
//	}
	FILE *pipe = _popen(address, "r");
	if(!pipe){
		printf("cmd failed\n");
	}
	
	while(!feof(pipe)){
		fgets(buffer, 128, pipe);
		printf("%s", buffer);
	} 
	_pclose(pipe);
	printf("\n");
} 

//���ļ������� 
void reName(char oldname[128], char newname[128]){ 
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int i=0, k=0, flag=1;
	int m=0, n=0;
	//�ж��ļ��Ƿ���ڣ����ļ����ڲſ��޸� 
	while(flag && i < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i]);
		k=0;
		while(k < 64){
			if(dir[k].inode && !strcmp(dir[k].name, oldname)){
				if(dir[k].file_type == 2){
					printf("The directory name can't be modified!\n");
					return;
				}else{
					flag = 0; //����
					break;
				}
			}
			k++; 
		}
		i++;
	}
	
	//�ļ�������flag=1 
	if(flag){
		printf("Can't find the filename!\n");
		return; 
	}else{
		flag=1;
		//�ж�newname�Ƿ����� 
		while(flag && m < inode_buffer->i_blocks_count){
			read_dir(inode_buffer->i_block[m]);
			n=0;
			while(n < 64){
				if(dir[n].inode && !strcmp(dir[n].name, newname)){
						flag = 0; //����
						break;
				}
				n++; 
			}
			m++;
		}
		if(!flag){
			printf("The filename \"%s\" is existed\n", newname);
			return;
		}else{
			read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
			read_dir(inode_buffer->i_block[i]);
			strcpy(dir[k].name, newname);
			dir[k].name_len=strlen(newname);
			write_dir(inode_buffer->i_block[i]);
		}
	}
} 
//��ʾʱ�� 
void data(){
	char data_time[128];
	time_t t = time(0); //��ȡ��ǰϵͳ��ʱ�� 
	strftime(data_time, sizeof(data_time), "%Y-%m-%d %H:%M:%S %A", localtime(&t));
	printf("%s\n", data_time);
}

//�޸�Ȩ�� 
void chmod(char temp[9], int mode){
	if(mode>777 || mode < 0){
		printf("mode <000-777>!\n");
		return;
	}
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int i=0, k=0, flag=1;

	//�ж��ļ��Ƿ���ڣ����ļ����ڲſ��޸� 
	flag = search_file(temp, &i, &k);
	
	//flag = 1 �ļ������� 
	if(flag){
		printf("Can't find this filename!\n"); 
		return;
	}else{
		read_dir(inode_buffer->i_block[i]);
		read_inode(dir[k].inode);
		inode_buffer->i_mode = mode;
		current_time();
		strcpy(inode_buffer->i_ctime, time_now);
		write_inode(dir[k].inode);
		printf("The file's mode has been modify!\n");
		
	}
}

//����1Ϊû�ҵ���0Ϊ�ҵ��� 
int search_file(char temp[9], int *i, int *k){
	read_inode(current_dir_inode); //��ȡ��ǰ�ڵ���Ϣ
	int flag=1, i_temp=0, k_temp=0; //������ʱ���� 
	
	while(flag && i_temp < inode_buffer->i_blocks_count){
		read_dir(inode_buffer->i_block[i_temp]);
		k_temp=0;
		while(k_temp < 64){
			if(dir[k_temp].inode && !strcmp(dir[k_temp].name, temp)){
					flag = 0; //����
					*i=i_temp;
					*k=k_temp;
					return 0;
			}
			k_temp++; 
		}
		i_temp++;
	}
	return 1;
} 
