#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#define F 20
#define G 40

funcsize(FILE* origpic) {

	int fsymbol = 0, fsizeofpic = 0;
	for (int b = 0; b < 4; b++) {

		fsymbol = fgetc(origpic);

		if (b == 0) fsizeofpic = fsizeofpic + fsymbol;
		else if (b == 1) fsizeofpic = fsizeofpic + (fsymbol * 256);
		else if (b == 2) fsizeofpic = fsizeofpic + (fsymbol * 65536);
		else if (b == 3) fsizeofpic = fsizeofpic + (fsymbol * 16777216);

	}
	return(fsizeofpic);
}

int getlength(FILE* fp) {
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return(size);
}

int getoffset(FILE* bmp_offset) {
	fseek(bmp_offset, 10, 0);
	int offset;
	offset = (int)fgetc(bmp_offset);
	return offset;
}


void code()
{
	FILE* origpic;
	FILE* newpic;
	FILE* text;
	int n = 0;										
	int symbol = 0, symboltxt = 0;
	int sdvigforsize = 18;				
	int width = 0, height = 0, bitperpix = 0;		
	int sizeofpic = 0, sizeoftxt = 0, newsizeoftxt = 0;
	int point = 10;						 							
	unsigned short sizetxt[10];
	unsigned char formattxt[F], infotxt[G];
	printf("\nEnter stepen upakovki:  ");
	scanf("%d", &n);
	if ((n > 0) && (n < 9)) printf("OK\n");
	else {
		printf("error : nevernaya stepen upakovki");
		return 0;
	}
	printf("\n");
	printf("Enter the name of pic:  ");
	char name[20];
	scanf("%20s", &name);
	puts(name);
	origpic = fopen(name, "rb");
	if (!origpic) {
		printf("\nerror: failed to open a picture\n");
		return 0;
	}
	printf("\n");
	int sdvig = getoffset(origpic);
	rewind(origpic);
	fseek(origpic, sdvigforsize, 0);
	width = funcsize(origpic);
	height = funcsize(origpic);
	for (int i = 0; i < 3; i++) symbol = fgetc(origpic);
	bitperpix = symbol;
	sizeofpic = width * height * bitperpix;
	printf("witdh = %d, height = %d, bpp = %d, sizeofpic = %d\n\n", width, height, bitperpix, sizeofpic);
	printf("Enter the name of file:  ");
	char nametxt[20];
	scanf("%20s", &nametxt);
	puts(nametxt);
	printf("\n");
	text = fopen(nametxt, "rb");
	if (!text) {
		printf("\nerror: failed to open a file\n");
		return 0;
	}
	sizeoftxt = getlength(text);
	memset(infotxt, 0, sizeof(infotxt));
	memset(formattxt, 0, sizeof(formattxt));
	int z = strlen(nametxt);
	char* dot = strrchr(nametxt, '.');
	dot++;
	for (int i = 0; (*(dot + i) != '\0'); i++) formattxt[i] = *(dot + i);
	int formattxtlen = strlen(formattxt);
	int spoint = 0;
	char string[20];
	_itoa(n, string, 10); // itoa переводит число в строку, просто int в char не меняя значения
	infotxt[spoint++] = string[0];
	int j = sizeoftxt;
	int sizeoftxtlen = 0;
	for (; j > 0; sizeoftxtlen++) {  // сначала считаем
		j = j / 10;
	}
	memset(string, 0, sizeof(string));
	_itoa(sizeoftxtlen, string, 10);
	infotxt[spoint++] = string[0];
	memset(string, 0, sizeof(string));
	_itoa(sizeoftxt, string, 10);
	strcat(infotxt, string);
	spoint = spoint + strlen(string);
	_itoa(formattxtlen, string, 10);
	infotxt[spoint++] = string[0];
	strcat(infotxt, formattxt);
	spoint = spoint + strlen(formattxt);
	sizeoftxt + strlen(infotxt);
	sizeoftxt *= 8;
	int availsizeofpic = 0;
	if (n == 1) availsizeofpic = sizeofpic / 8;
	else if (n == 2) availsizeofpic = sizeofpic / 4;
	else if (n == 4) availsizeofpic = sizeofpic / 2;
	else if (n == 8) availsizeofpic = sizeofpic;
	// проверяем, можно ли вместить файл
	if (sizeoftxt > availsizeofpic) {
		//printf("\n\nОшибка, файл не помещается");
		printf("\n\nОшибка, файл не помещается (степень упаковки = %d, размер файла в битах = %d, размер картинки = %d)\n\n", n, sizeoftxt, sizeofpic);
		return 0;
	} else  printf("\n\nОК, файл поместится (степень упаковки = %d, размер файла в битах = %d, доступное место = %d, размер картинки = %d\n\n", n, sizeoftxt, availsizeofpic, sizeofpic);
	sizeoftxt = sizeoftxt / 8; // переводим размер файла обратно в байты
	char newpicname[] = "npicture.bmp";
	newpic = fopen(newpicname, "wb");
	if (!newpic) {
		printf("error: failed to open a new picture");
		return 0;
	}
	unsigned char head[150];
	rewind(origpic);
	fread(head, 1, sdvig, origpic);
	fwrite(head, 1, sdvig, newpic);
	rewind(origpic);
	fseek(origpic, sdvig, 0);
	rewind(newpic);
	fseek(newpic, sdvig, 0);
	unsigned char bit_text = 0, dbit_text = 0; // бит с картинки
	unsigned char bit_pic = 0, dbit_pic = 0; // бит с файла
	int dspoint = spoint;
	spoint = 1;
		bit_pic = fgetc(origpic);
		bit_pic = bit_pic & 252;
		if (n == 1) bit_pic = bit_pic | 0;
		else if (n == 2) bit_pic = bit_pic | 1;
		else if (n == 4) bit_pic = bit_pic | 2;
		else if (n == 8) bit_pic = bit_pic | 3;
		fputc(bit_pic, newpic);
	while (!feof(origpic)) {
		if (spoint < dspoint) {
			bit_text = infotxt[spoint]; 
			spoint++;
		}
		else {
			bit_text = fgetc(text);
			dbit_text = bit_text;
		}
		for (int i = 0; i < (8 / n); i++) { 
			if (feof(text)) break;
			dbit_text = bit_text;
			dbit_text = (dbit_text << (n * i));
			char one = 255 << (8 - n);
			dbit_text = dbit_text & one;
			dbit_text = dbit_text >> (8 - n);
			bit_pic = fgetc(origpic);
			one = 255 << n;
			bit_pic = bit_pic & one;
			dbit_text = dbit_text | bit_pic;
			fputc(dbit_text, newpic);
		}
		if (feof(text)) { 

			while (!feof(origpic)) {
				bit_text = fgetc(origpic);
				fputc(bit_text, newpic);
			}
		}
	}
	fclose(origpic);
	fclose(newpic);
	fclose(text);
	return 0;
}


void decode() {
	FILE* pic;
	FILE* newtext;
	//printf("Enter the name of pic:  ");
	//scanf("%20s", &name);
	char name[] = "npicture.bmp";
	pic = fopen(name, "rb");
	if (!pic) {
		printf("\nerror: failed to open a picture\n");
		return 0;
	}
	int sdvig = getoffset(pic);
	unsigned char ch = 0;
	fseek(pic, sdvig, 0);
	ch = fgetc(pic);
	ch = ch << 6;
	ch = ch >> 6;
	int n = 0;
	if (ch == 0) n = 1;
	else if (ch == 1) n = 2;
	else if (ch == 2) n = 4;
	else if (ch == 3) n = 8;
	char newstring[9];
	int sizeoftxtlen = read_int(n, pic, 1);
	int sizeoftxt = 0;
	int buf = 0;
	for (double i = 1; i <= sizeoftxtlen; i++) {
		buf = read_int(n, pic, 1);
		sizeoftxt += buf * pow((double)10, (double)(sizeoftxtlen - i));
	}
	int formattxtlen = read_int(n, pic, 1);
	char* formattxt;
	formattxt = (char*)malloc(sizeof(char)*(formattxtlen + 1)); 
	for (int i = 0; i < formattxtlen; i++) {
		formattxt[i] = read_int(n, pic, 2);
		if (i == (formattxtlen - 1)) {
			i++;
			formattxt[i] = '\0'; 
		}
	}
	char textbase[] = "out_text.";
	char* textname;
	textname = (char*)malloc(sizeof(char) * (strlen(textbase) + strlen(formattxt))); 
	memset(textname, 0, sizeof(textname));
	strcat(textname, textbase);
	strcat(textname, formattxt);
	printf("\ntextname = %s\n", textname);
	newtext = fopen(textname, "wb");
	if (!newtext) {
		printf("\nerror: failed to open a file for text\n");
		return 0;
	}
	printf("\n");
	unsigned char bit_text = 0, dbit_text = 0; // бит с картинки
	unsigned char bit_pic = 0, dbit_pic = 0; // бит с текста
	int count = 0;
	while (count < sizeoftxt) {
		bit_text = read_int(n, pic, 2);
		fputc(bit_text, newtext);
		count++;
	}
	fclose(newtext);
	fclose(pic);
	return(0);
}


int main() {
	setlocale(LC_ALL, "Rus");
	printf("\nВыберите:\n1 - Поместить файл в картинку\n2 - Извлечь файл из картинки\n");
	int choice = 0;
	scanf_s("%d", &choice);
	if (choice == 1) code();
	else if (choice == 2) decode();
	else printf("\nНеверный выбор\n");
	return 0;
}

int read_int(int n, FILE* pic, short type) { 
	unsigned char number = 0;
	unsigned char bit_from_pic = 0;
	for (int i = 0; i < (8 / n); i++) {
		bit_from_pic = fgetc(pic);
		bit_from_pic = bit_from_pic << (8 - n); 
		bit_from_pic = bit_from_pic >> (8 - n);
		number = bit_from_pic | number;
		if (i == ((8 / n) - 1)) break;	
		number = number << n; 
	}
	if (type == 2) return(number); 
	int intnumber = 0;
	for (int i = 0; i < 10; i++) {
		if (number == (48 + i)) intnumber = i;
	}
	return (intnumber);
}
