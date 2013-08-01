#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct huff_node {
	unsigned char ch;
	unsigned int freq; 
	int left;
	int right;
	char flag;
	char *pc;
};

void show_char_table(struct huff_node *huff, int char_type);
struct huff_node *get_char_table(char *filename, int *char_type);
int find_min_freq(struct huff_node *huff, int char_type);
int creat_huff_tree(struct huff_node *huff, int char_type);
void make_huff_code(struct huff_node *huff, int root, char *code, int index);
void destory_huff(struct huff_node *huff, int char_type);
int get_bite(struct huff_node *huff, int char_type);
void write_file_head(FILE *fp, struct huff_node *huff, int char_type);
int find_char_code(struct huff_node *huff, int char_type, char ch);

struct huff_node *get_char_table(char *filename, int *char_type)
{
	unsigned int buffer[256] = {0};
	FILE *fp;
	unsigned char ch;
	struct huff_node *huff = NULL;

	if((fp = fopen(filename, "r"))) {
		int i = 0, k = 0;
		ch = fgetc(fp);
		while(!feof(fp)) {
			buffer[ch]++;
			ch = fgetc(fp);
		}
		for(; i < 256; i++)
			buffer[i] && (*char_type)++;
		huff = (struct huff_node *)malloc(sizeof(struct huff_node)
			 * (2 * (*char_type) - 1));
		for(i = 0; i < 256; i++)
			if(buffer[i]) {
				huff[k].ch = i;
				huff[k].freq = buffer[i];
				huff[k].left =	huff[k].right = -1;
				huff[k].flag = 0;
				huff[k].pc = (char *)malloc(sizeof(char) * (*char_type));
				k++;
			}
		fclose(fp);

		return huff;
	} else {
		perror("Open file: ");
		exit(-1);
	}
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

int get_bite(struct huff_node *huff, int char_type)
{
	int i = 0;
	int word_count = 0;

	for(; i < char_type; i++)
		word_count += huff[i].freq;

	return word_count;
}

void write_file_head(FILE *fp, struct huff_node *huff, int char_type)
{
	int i = 0;

	fwrite(&char_type, sizeof(int), 1, fp);
	for(; i < char_type; i++) {
		fwrite(&huff[i].ch, sizeof(char), 1, fp);
		fwrite(&huff[i].freq, sizeof(int), 1, fp);
	}
	i = get_bite(huff, char_type);
	fwrite(&i, sizeof(int), 1, fp);
}

int find_char_code(struct huff_node *huff, int char_type, char ch)
{
	int tail = char_type;
	int head = 0;
	int mid = (tail + head) / 2;

	while (head < tail) {
		if (ch < huff[mid].ch)
			tail = mid - 1;
		else if (ch > huff[mid].ch)
			head = mid + 1;
		else
			return mid;
		mid = (head + tail) / 2;
	}
}

void hoop_huff(char *filename, struct huff_node *huff, int char_type)
{
	FILE *fpin, *fpout;

	fpin = fopen(filename, "r");
	strcat(filename, ".yip");
	fpout = fopen(filename, "wb");
	if(fpin && fpout) {
		unsigned char c;
		unsigned int ch = 0;
		unsigned char num = (1<<7);
		int index, char_index;

		write_file_head(fpout, huff, char_type);
		c = fgetc(fpin);
		while (!feof(fpin)) { 
			char_index = find_char_code(huff, char_type, c);
			for(index = 0;huff[char_index].pc[index];index++) {
				if(num == 0) {
					fputc(ch, fpout);
					num = (1<<7);
					index--;
				} else {
					if (huff[char_index].pc[index] == '1')
						ch |= num;
					else
						ch &= (~num);
					num = num>>1;	
				}
			}
			c = fgetc(fpin);
		}
		if (num != 0)
			fputc(ch, fpout);
		fclose(fpout);
		fclose(fpin);
	} else {
			perror("Hoop file failed!\n");
		exit(-1);
	}
}

void destory_huff(struct huff_node *huff, int char_type)
{
	int i = 0;
	for(; i < char_type; i++)
		free(huff[i].pc);
	free(huff);
}

void show_char_table(struct huff_node *huff, int char_type)
{
	int i = 0;

	printf("char\tfreq\tleft\tright\tflag\tcode\n");
	for(; i < char_type; i++)
		printf("%c\t%u\t%d\t%d\t%d\t%s\n", huff[i].ch, huff[i].freq,
			huff[i].left, huff[i].right, huff[i].flag, huff[i].pc);
}

int main(int argc, char **argv)
{
	if(argc == 2) {
		struct huff_node *huff;
		int char_type = 0;
		int count;
		char code_str[256];

		huff = get_char_table(argv[1], &char_type);
		if(huff) {
			count = creat_huff_tree(huff, char_type);
			make_huff_code(huff, count - 1, code_str, 0);
			hoop_huff(argv[1], huff, char_type);
//			show_char_table(huff, char_type);
		}

		destory_huff(huff, char_type);
		printf("hoop done!\n");
	} else
		printf("Usage: yip <filename>\n");
	return 0;
}
