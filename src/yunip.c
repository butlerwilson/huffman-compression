#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct huff_node {
	unsigned char ch;
	unsigned int freq; 
	int left;
	int right;
	char flag;
	char *pc;
};

struct huff_node *get_char_table(char *filename, int *char_type);
int creat_huff_tree(struct huff_node *huff, int char_type);
int find_min_freq(struct huff_node *huff, int char_type);
void make_huff_code(struct huff_node *huff, int root, char *code, int index);
void dehuff_code(char *filename, struct huff_node *huff, int char_type);
void show_char_table(struct huff_node *huff, int char_type);
void destory_huff(struct huff_node *huff, int char_type);

struct huff_node *get_char_table(char *filename, int *char_type)
{
	FILE *fp;
        unsigned char ch;
        struct huff_node *huff = NULL;

        if((fp = fopen(filename, "rb"))) {
                int i = 0;

		fread(char_type, sizeof(int), 1, fp);
		huff = (struct huff_node *)malloc(sizeof(struct huff_node) *
			2 * (*char_type) - 1);
                for(; i < *char_type; i++) {
			fread(&huff[i].ch, sizeof(char), 1, fp);
			fread(&huff[i].freq, sizeof(int), 1, fp);
			huff[i].left =  huff[i].right = -1;
                      	huff[i].flag = 0;
                        huff[i].pc = (char *)malloc(sizeof(char)* (*char_type));
                }
               fclose(fp);

               return huff;
       } else {
                perror("Open file: ");
                exit(-1);
        }
}

int creat_huff_tree(struct huff_node *huff, int char_type)
{
	int i = 0;
	int m, n;
	int cnt = char_type;

	for(i = 0; i < char_type - 1; i++){
		m = find_min_freq(huff, cnt);
		n = find_min_freq(huff, cnt);
		huff[cnt].ch = '#';
		huff[cnt].freq = huff[m].freq + huff[n].freq;
		huff[cnt].left = m;
		huff[cnt].right = n;
		huff[cnt].flag = 0;
		huff[cnt].pc = NULL;
		cnt++;
	}

	return cnt;
}

int find_min_freq(struct huff_node *huff, int char_type)
{
	int i = 0,m, flag;
	while(huff[i].flag == 1)
		i++;
	for(m= i; i < char_type; i++)
		if((huff[i].flag == 0) && (huff[i].freq < huff[m].freq)) {
				m = i;
		}
	huff[m].flag = 1;

	return m;
}

void make_huff_code(struct huff_node *huff, int root, char *code, int index)
{
		if(huff[root].left == -1) {
			code[index] = 0;
			strcpy(huff[root].pc, code);
		} else {
			code[index] = '1';
			make_huff_code(huff, huff[root].left, code, index+1);
			code[index] = '0';
			make_huff_code(huff, huff[root].right, code, index+1);
		}
}

void dehuff_code(char *filename, struct huff_node *huff, int char_type)
{
	FILE *fpin, *fpout;
	char *p;
	int bite_count = 0;
	int count = 0;

	fpin= fopen(filename, "rb");
	p = strrchr(filename, '.');
	*p = 0;
	strcat(filename, ".unyip");
	fpout = fopen(filename, "w");
	if(fpin && fpout) {
		unsigned char ch = 0;
		unsigned char sp = 0;
		unsigned char num;
		int char_index, index;
		int root = 2 * char_type - 2;
		fseek(fpin, sizeof(int) + 5 * char_type, SEEK_SET);
		fread(&bite_count, sizeof(int), 1, fpin);
		fread(&ch, sizeof(char), 1, fpin);
		while(!feof(fpin)) {
			for(num = (1<<7); num != 0;) {
				if (count == bite_count) {
					printf("dehoop done!\n");
					exit(0);
				}
				if (huff[root].left == -1) {
					fputc(huff[root].ch, fpout);
					root = 2 * char_type - 2;
					count++;
				} else {
					sp = ch & num;
					if(sp)
						root = huff[root].left;
					else
						root = huff[root].right;
					num = num>>1;
				}
			}
			fread(&ch, sizeof(char), 1, fpin);
		}
	} else {
		perror("Open file: ");
		exit(-1);
	}
}

void show_char_table(struct huff_node *huff, int char_type)
{
	int i = 0;

	printf("char\tfreq\tleft\tright\tflag\tcode\n");
	for(; i < char_type; i++)
		printf("%c\t%u\t%d\t%d\t%d\t%s\n", huff[i].ch, huff[i].freq,
			huff[i].left, huff[i].right, huff[i].flag, huff[i].pc);
}

void destory_huff(struct huff_node *huff, int char_type)
{
	int i = 0;
	for(; i < char_type; i++)
		free(huff[i].pc);
	free(huff);


int main(int argc, char **argv)
{
	struct huff_node *huff;
	int char_type;
	int count;
	unsigned char code[256];

	if (argc == 2) {
		huff = get_char_table(argv[1], &char_type);
		count = creat_huff_tree(huff, char_type);
		make_huff_code(huff, count - 1, code, 0);
		dehuff_code(argv[1], huff, char_type);
	//	show_char_table(huff, char_type);

		destory_huff(huff, char_type);
	} else {
		printf("Usage: yunip filename(.yip)\n");
		exit(-1);
	}

	return 0;
}
