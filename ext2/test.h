#include <stdio.h>

#define BLOCK_SIZE 1024	/* ���С */
#define BLOCK_COUNT 8708	/* ������� 1+1+1+1+512+8192*/

#define SUPER_BLOCK_SIZE 64
#define BLOCK_GROUP_DES_SIZE 32
#define INODE_SIZE 64 
#define INODE_TABLE_SIZE 524288

#define SUPER_BLOCK_START 0 // 
#define BLOCK_GROUP_DES_START 1024 //
#define BLOCK_BITMAP_START 2048 // 
#define INODE_BITMAP_START 3072 //
#define INODE_TABLE_START 4096 //1024*4 
#define DATA_BLOCK_START 528384 // 1024*516k

#define VOLUME_NAME "EXT2"

#pragma comment(lib, "WS2_32.lib")
/*
int 4 4294967296  10λ�� 

*/ 


/*********
��СΪ 64 bytes 
����һ��������
64 - (16+4*6+20) = 4

���� 
inode��block������
inode��block��ʣ����
block��inode�Ĵ�С
�����ļ�ϵͳ��ʱ�� 
**********/
struct super_block{
	char sb_volume_name[16];   //���� 
	unsigned int sb_inodes_count;  //inode���� = 8*1024
	unsigned int sb_blocks_count;  //block���� = 8708
	unsigned int sb_free_inodes_count;  //ʣ��inode�� 
	unsigned int sb_free_blocks_count;  //ʣ��block�� 
	unsigned int sb_block_size;  //ѡ����򵥵� 1024byte 
	unsigned int sb_inode_size;  // 64byte 
	char sb_wtime[20];  //2016-06-01  24:24:24 һ��20bytes  
	char sb_pad[4]; //��� 
	 
};

/********* 
����һ������������ 32byte 

32 - 4*6 = 8
block bitmap�ӵڼ��鿪ʼ
inode bitmap�ӵڼ��鿪ʼ
inode table�ӵڼ��鿪ʼ
inode��block��ʣ���� 
�������Ŀ¼���� 
**********/

struct block_group_desc{
	unsigned int bgd_block_bitmap_start;  //block bitmap��ʼ���� 
	unsigned int bgd_inode_bitmap_start;  //inode bitmap��ʼ���� 
	unsigned int bgd_inode_table_start;  //inode table ��ʼ���� 
	unsigned int bgd_free_inode_count;  //inodeʣ���� 
	unsigned int bgd_free_block_count;  //blockʣ���� 
	unsigned int bgd_used_dirs_count;  //Ŀ¼���� 
	char bgd_pad[8];  //��� 
}; 

/********
����һ��inode_table   64
64 -  7*4 - 2*8 =  20

�ļ����ͺ�����Ȩ�� 0/1 000 000 000 
���ļ�����Ŀ¼�Ĵ�С 
����ʱ�� atime 
����ʱ�� ctime 
�޸�ʱ�� mtime
ɾ��ʱ�� dtime
block������  
̫�����ˣ�ֱ�ӽ�8��ֱ������ ����һ���ļ�����ܹ�8*1024 = 8K��С 

ָ������ݿ�� 8����6��ֱ�ӣ�һ����ӣ�һ��˫��� �ܵ����ݿ�1024*8 = 8192  ֻ��Ҫ2���ֽڼ�¼ 
	��Ϊblock=1024bytes��
		6��ֱ�ӣ�6k
		1����ӣ�1*512*1K = 512k
		1��˫��ӣ�1*512*512*1k = 256M  
*******/
struct inode{
	unsigned int i_mode;  //�ļ�Ȩ�� 
	unsigned int i_size;  //�ļ�����Ŀ¼��С 
	unsigned int i_atime;  //����ʱ�� 
	char i_ctime[20];      //����ʱ�� 
	unsigned int i_mtime;  //�޸�ʱ�� 
	unsigned int i_dtime;  //ɾ��ʱ�� 
	unsigned int i_blocks_count; //ռ�õ�block������ 
	unsigned short i_block[8];  //����ָ��, ���block�� 
	char i_pad[4];  //��� 
};

/**********
Ŀ¼��ṹ  16bytes   һ����1024/16 = 64��Ŀ¼�� 

�����ڵ��  <8192
Ŀ¼���  
�ļ�������
�ļ�����
�ļ��� 
**********/
struct dir_entry{
	unsigned short inode; //�����ڵ��
	unsigned short rec_len; //Ŀ¼���
	unsigned short name_len; //�ļ�������
	char file_type; //�ļ����ͣ�1����ͨ�ļ���2������Ŀ¼��
	char name[9]; //�ļ���	
};



//�������棬��Ϊ���������� 

static char block_buffer[1024];  
char test[8708*1024];  //������ 
char inode_table_buffer[522240]; //������ 
char filebuffer[8192]=""; //�ļ����ݻ���
 
//�ص㣺ΪʲôҪָ�� ------fwrite 
//ʹ��ָ��ʱ��������ַ������������ 
struct super_block super_block_buffer[1];	//�����黺���� 
struct block_group_desc block_group_desc_buffer[1];	/* �������������� */
struct inode inode_buffer[1];  /* �ڵ㻺���� */
struct dir_entry dir[64]; //64*16 = 1024 = 1block
 
unsigned char block_bitmap_buffer[1024]=""; //block bitmap������ 
unsigned char inode_bitmap_buffer[1024]=""; //inode bitmap������ 


unsigned int last_inode_bit=1;  //��¼���һ�ε�inode bit 
unsigned int last_block_bit=0;  //��¼���һ�ε�block bit 
unsigned int current_dir_inode; //��ǰĿ¼��inode 
unsigned int current_dir_length; //��ǰĿ¼�ĳ��� 
char current_path[256]="";    //��ǰ·����  [root@sking ~]#   16bytes  ��13λ�滻
//char path_first[13]=""; 
char path_last[4]="";
char path_name[241]=""; 
char time_now[32] = "";  //�洢��ǰʱ�� 

FILE *pf;



//��������

void read_super_block(void); //�Ӵ����ж���super_block ��������
void write_super_block(void); //����������super_block д����� 

void read_block_group_desc(void); // �Ӵ����ж���block_group_des �������� 
void write_block_group_desc(void);  //����������block_group_desc д����� 

void read_block_bitmap(void); //��bit map ����������block_buffer
void write_block_bitmap(void); //��������block_bufferд����� 

void read_inode_bitmap(void); //��bit map ����������inode_buffer
void write_inode_bitmap(void); //��������inode_bufferд����� 

void read_inode(int);  //��ȡһ��inode��Ϣ 
void write_inode(int); //д��һ��inode��Ϣ 

void read_block(int);  //��ȡһ��block��Ϣ 
void write_block(int); //д��һ��block��Ϣ 

void read_dir(int); //��ȡһ��dir��Ϣ 
void write_dir(int); //д��һ��dir��Ϣ 

unsigned int get_free_block(void); //����һ�����е�block
unsigned int get_free_inode(void); //����һ�����е�inode 

void remove_block(int); //����һ��block bit
void remove_inode(int); //����һ��inode bit

void current_time(void); //���õ�ǰʱ�� 

void create_fileSystem(); //�½��ļ�ϵͳ 

//���ܺ������� 
void ls(void); //��ʾĿ¼�µ��ļ�
void mkdir(char temp[9]); //�½�һ��Ŀ¼ 
void dir_prepare(unsigned int, int); //���½���Ŀ¼���г�ʼ��
void cd(char temp[9]); //�л�Ŀ¼
void format(void); //��ʽ������ 
void rmdir(char temp[9]); //ɾ����Ŀ¼
void logout(void); //�˳���¼
void help(void); //��ʾ��������
void dumpe2fs(void); //��ʾ������Ϣ
void vi(char temp[9]); //�������ļ� 
void cat(char temp[9]); //�鿴�ļ����� 
void rm(char temp[9]); //ɾ���ļ� 
void ifconfig(void); //��ʾIP 
void ping(char ip[128]); //ping���� 
void reName(char oldname[128], char newname[128]); //�ļ������� 
void data(void); //��ʾ��ǰʱ�� 
void chmod(char temp[9], int mode); //�޸�Ȩ�� 
int search_file(char temp[9], int *i, int *k); //Ѱ�Ҹ�Ŀ¼�Ƿ��д��ļ������з���1��û�з���0 
