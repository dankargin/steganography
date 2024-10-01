#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#define F 20
#define G 40

int perevodv2(int a);
int count_of_prints = 0;
int read_int(int n, FILE* pic, short type);

void print_array(unsigned char A[])
{
	for (int i = 0; i < 54; ++i) {
		char string[8];
		_itoa(A[i], string, 16);
		printf("%5s ", string);
	}
	printf("\n");
}
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

print_byte(unsigned char x, short type) { // � ���

	if (type == 1) printf("����� ��� �����: ");
	if (type == 2) printf("��� �����: ");
	if (type == 3) printf("����� ��� ��������: ");
	if (type == 4) printf("��� ��������: ");

	int mass[16];
	memset(mass, 0, sizeof(mass));
	perevodv2(x, mass);

	for (int i = 0; i < 8; ++i)
		printf("%d", mass[i]);
	int d = 0;
	d = d | x;
	printf(", (10) = %d", d);

	if ((type == 1) || (type == 2)) printf(", ch = %c", x); // ���� ��� ����� - ������� ������
	if ((type == 3) || (type == 4)) {  // ���� ��� �������� - ������� 16 ������ ������
		printf(", (16) = ");
		char sstring[10];
		_itoa(x, sstring, 16);
		printf("%s", sstring);
	}

	printf("\n");
}

void code()
{
	FILE* origpic;
	FILE* newpic;
	FILE* text;
	int n = 0;										// ������� ��������
	int symbol = 0, symboltxt = 0, sp = 0, sn = 0;
	int sdvigforsize = 18;				// ������ (������� � ���������� .bmp)
	int width = 0, height = 0, bitperpix = 0;		// ��� ������� �������� ��������
	int sizeofpic = 0, sizeoftxt = 0, newsizeoftxt = 0;
	int point = 10;						 // ���� "���������"							
	unsigned short sizetxt[10];
	unsigned char formattxt[F], infotxt[G];
	long bitcount = 0;
	// 1 ������ ������� ��������

	//printf("Enter stepen upakovki:  ");
	//scanf("%d", &n);
	n = 2;

	if ((n > 0) && (n < 9)) printf("OK\n");
	else {
		printf("error : nevernaya stepen upakovki");
		return 0;
	}
	printf("\n");

	// 2 ��������� ��������
	printf("Enter the name of pic:  ");
	//scanf("%20s", &name);
	char name[] = "wave.bmp";
	//char name[] = "sun.bmp";
	puts(name);

	origpic = fopen(name, "rb");
	if (!origpic) {
		printf("\nerror: failed to open a picture\n");
		return 0;
	}
	printf("\n");

	// 3 ������� ������ ��������

		// ��������� �������� ������, ����� ����� �������� ���� � ��������
	int sdvig = getoffset(origpic);

	// ���������� �� ����� biSizeImage, ����� ������� ������ �������� (��� ��������)
	rewind(origpic);
	fseek(origpic, sdvigforsize, 0);
	// ���������, ������ ����� �������� � �����
	width = funcsize(origpic);
	height = funcsize(origpic);
	for (int i = 0; i < 3; i++) symbol = fgetc(origpic);
	bitperpix = symbol;
	sizeofpic = width * height * bitperpix;
	printf("witdh = %d, height = %d, bpp = %d, sizeofpic = %d\n\n", width, height, bitperpix, sizeofpic);


	// 4 ��������� ����
	printf("Enter the name of file:  ");
	//scanf("%20s", &nametxt);
	char nametxt[] = "engtext.txt";
	//char nametxt[] = "shorttext.txt";
	puts(nametxt);

	printf("\n");
	text = fopen(nametxt, "rb");
	if (!text) {
		printf("\nerror: failed to open a file\n");
		return 0;
	}
	// 5 ������� ������ ����� (sizeoftxt)
	sizeoftxt = getlength(text);
	printf("\nsizeoftxt = %d\n", sizeoftxt);

	// 6 ��������� � infotxt --------------- 
	printf("\n\n\n��������� infotxt:\n 1 - ������� �������� (n) \n 2 - formattxtlen (���-�� �������� � �������, �������� txt = 3)\n 3 - formattxt (��� ������ �������, �������� ""jpeg"")\n 4 - ���-�� ���� � sizeoftxt \n 5 - sizeoftxt ������ (���-�� ����)\n\n");

	// �������� ������
	memset(infotxt, 0, sizeof(infotxt));
	memset(formattxt, 0, sizeof(formattxt));

	// 6.1 ���������� ���������� � ����� � ������ formattxt
		//������� ����� �����
	int z = strlen(nametxt);
	printf("strlen(nametxt) = %d\n", z);

	// ���� ��������� ��������� ����� � ���
	// ���� ��������� ������������ � ������, ������� �� ������ ��������� �� ����� � �����, � ������������ ���������� ������ �� ������� ����� ������
	char* dot = strrchr(nametxt, '.');
	dot++;

	// ���������� ������ � formattxt
	for (int i = 0; (*(dot + i) != '\0'); i++) formattxt[i] = *(dot + i);
	printf("formattxt:");
	printf(" %s", formattxt);

	// 6.2 ������� ����� ���������� ������� � ���������� � formattxtlenght
	int formattxtlen = strlen(formattxt);

	// 6.3 �������� ���������� - ���������� �� �� � ������ ������
	int spoint = 0;
	char string[20];
	_itoa(n, string, 10); // itoa ��������� ����� � ������, ������ int � char �� ����� ��������
	infotxt[spoint++] = string[0];

	// 6.4 ���������� ���-�� ���� sizeoftxt ��� ����� ����� ������� ����������� � ������ �������

	int j = sizeoftxt;
	int sizeoftxtlen = 0;
	for (; j > 0; sizeoftxtlen++) {  // ������� �������
		j = j / 10;
	}
	memset(string, 0, sizeof(string));
	_itoa(sizeoftxtlen, string, 10);
	infotxt[spoint++] = string[0];


	// 6.5 ���������� ������ ������ � ������ � infotxt
		//�.�. �������� ������ � ������ ����� (sizeoftxt) �� �����

	memset(string, 0, sizeof(string));
	_itoa(sizeoftxt, string, 10);
	strcat(infotxt, string);
	spoint = spoint + strlen(string);
	// 6.6 ���������� ����� ���������� �� ������ ������
	_itoa(formattxtlen, string, 10);
	infotxt[spoint++] = string[0];

	// 6.7 ����� ���� ���������� � ������� ������
	strcat(infotxt, formattxt);
	spoint = spoint + strlen(formattxt);
	//puts(infotxt);



	// ����� infotxt
	printf("����� infotxt = %s", infotxt);

	// 7 ���������, ����������� �� ���� + ����������

	// 7.1 ��������� �������
	sizeoftxt + strlen(infotxt);

	// ��������� sizeoftxt � ����
	sizeoftxt *= 8;

	// �������� ������� �������� ������� ��������� ��� ����� �����
	int availsizeofpic = 0;
	if (n == 1) availsizeofpic = sizeofpic / 8;
	else if (n == 2) availsizeofpic = sizeofpic / 4;
	else if (n == 4) availsizeofpic = sizeofpic / 2;
	else if (n == 8) availsizeofpic = sizeofpic;
	// ���������, ����� �� �������� ����
	if (sizeoftxt > availsizeofpic) {
		printf("\n\n������, ���� �� ���������� (������� �������� = %d, ������ ����� � ����� = %d, ������ �������� = %d)\n\n", n, sizeoftxt, sizeofpic);
		return 0;
	}
	else {
		printf("\n\n��, ���� ���������� (������� �������� = %d, ������ ����� � ����� = %d, ��������� ����� = %d, ������ �������� = %d\n\n", n, sizeoftxt, availsizeofpic, sizeofpic);
	}

	sizeoftxt = sizeoftxt / 8; // ��������� ������ ����� ������� � �����

	//-----------------------------------------------------------------

// 8 ��������� �������� ��� ������
	newpic = fopen("picture.bmp", "wb");
	if (!newpic) {
		printf("error: failed to open a new picture");
		return 0;
	}

	printf("\nsdvig = %d\n", sdvig);

	// 10 �������� ���������� � �������� � ����� ��������
		/*rewind(origpic);
		for (int i = 0; i < sdvig; i++) {
			symbol = fgetc(origpic);
			fputc(symbol, newpic);
			symbol = 0;
		}*/

	unsigned char head[150];
	rewind(origpic);
	fread(head, 1, sdvig, origpic);
	fwrite(head, 1, sdvig, newpic);
	print_array(head);

	// 11 ���������� ������ �� ������ �� sdvig, ����� �������� ���� � ����� � ����� ������� � ��������
	rewind(origpic);
	fseek(origpic, sdvig, 0);
	rewind(newpic);
	fseek(newpic, sdvig, 0);

	// 12 ����������� ���������� ����������
	unsigned char bit_text = 0, dbit_text = 0; // ��� � ��������
	unsigned char bit_pic = 0, dbit_pic = 0; // ��� � �����

	int dspoint = spoint;
	spoint = 1;

	// ������� ���������� ������� �������� � ������ ���� (��������� ��� 2 ����) �� �������� 1 = 00, 2 = 01, 4 = 10, 8 = 11
		bit_pic = fgetc(origpic);
		// ��������� ��������� ��� ����
		bit_pic = bit_pic & 252;
		// ������� ���� ���� ��������
		if (n == 1) bit_pic = bit_pic | 0;
		else if (n == 2) bit_pic = bit_pic | 1;
		else if (n == 4) bit_pic = bit_pic | 2;
		else if (n == 8) bit_pic = bit_pic | 3;
		fputc(bit_pic, newpic);
		//printf("\nbit with n = %d\n", bit_pic);

	while (!feof(origpic)) {


		if (spoint < dspoint) {
			
			bit_text = infotxt[spoint]; // ����� ���������� ��� infotxt
			//printf("\n����� infotxt[spoint] = %c\n", infotxt[spoint]);
			spoint++;
		}
		else {
			bit_text = fgetc(text);
			//printf("\n����� bit_text = fgetc(text) = %c\n", bit_text);
			dbit_text = bit_text;
		}
		

		for (int i = 0; i < (8 / n); i++) { 

			if (feof(text)) break;

			//printf("\n ------ \n");
			dbit_text = bit_text;
			//print_byte(dbit_text, 2);

			//printf("\n �������� ����� - \n");
			dbit_text = (dbit_text << (n * i));
			//print_byte(dbit_text, 2);

			//printf("\n 3 (one) - ");
			char one = 255 << (8 - n);
			//print_byte(one, 2);

			//printf("\n �������� �� ������ ����� n ��� - \n");
			dbit_text = dbit_text & one;
			//print_byte(dbit_text, 2);

			//printf("\n ������ �� ������� - \n");
			dbit_text = dbit_text >> (8 - n);
			//print_byte(dbit_text, 2);

			//printf("\n 6 - ");
			bit_pic = fgetc(origpic);
			//print_byte(bit_pic, 4);

			//printf("\n 7 (one) - ");
			one = 255 << n;
			//print_byte(one, 2);

			//printf("\n ������ � �������� ��������� n ��� - \n");
			bit_pic = bit_pic & one;
			//print_byte(bit_pic, 4);

			//printf("\n �������� ���� ���� ������ � �������� ��� � ����: - \n");
			dbit_text = dbit_text | bit_pic;
			//print_byte(dbit_text, 2);

			fputc(dbit_text, newpic);

			sp++; // ��� ����� ������

		}


		if (feof(text)) { // ���� ����� ����������, �� �������� ��� ���������� ����� �� ������ �������� � �����

			while (!feof(origpic)) {
				bit_text = fgetc(origpic);
				fputc(bit_text, newpic);

				sn++; // ��� ����� ������
			}
		}
	}
	fclose(origpic);
	fclose(newpic);
	fclose(text);

	printf("\n\n���������� %d ����, �� ������� %d ����\n", sp, sn);

	return 0;
}


void decode() {

	FILE* pic;
	FILE* newtext;

	// ��������� ��������
	printf("Enter the name of pic:  ");
	//scanf("%20s", &name);
	char name[] = "picture.bmp";
	puts(name);
	pic = fopen(name, "rb");
	if (!pic) {
		printf("\nerror: failed to open a picture\n");
		return 0;
	}
	printf("\n");


	// ��������� �����
	int sdvig = getoffset(pic);
	printf("\nsdvig = %d\n", sdvig);
	unsigned char ch = 0;
	fseek(pic, sdvig, 0);

	// ��������� ������� ��������

	ch = fgetc(pic);
	ch = ch << 6;
	ch = ch >> 6;

	int n = 0;
	if (ch == 0) n = 1;
	else if (ch == 1) n = 2;
	else if (ch == 2) n = 4;
	else if (ch == 3) n = 8;
	printf("\nn = %d\n", n);

	char newstring[9];
	
	// ��������� ���-�� ���� � ������� ����� (� ����������� � infotxt
	printf("\n1\n");
	int sizeoftxtlen = read_int(n, pic, 1);
	
	// ��������� ������
	int sizeoftxt = 0;
	int buf = 0;
	for (double i = 1; i <= sizeoftxtlen; i++) {
		buf = read_int(n, pic, 1);
		sizeoftxt += buf * pow((double)10, (double)(sizeoftxtlen - i));
	}

	printf("\nsizeoftxtlen = %d, sizeoftxt = %d\n", sizeoftxtlen, sizeoftxt);

	// ��������� ���-�� ���� � �������
	int formattxtlen = read_int(n, pic, 1);
	char* formattxt;
	formattxt = (char*)malloc(sizeof(char)*(formattxtlen + 1)); // +1 ��� \0
	// ��������� ������
	for (int i = 0; i < formattxtlen; i++) {
		formattxt[i] = read_int(n, pic, 2);
		if (i == (formattxtlen - 1)) {
			i++;
			formattxt[i] = '\0'; // � ����� ������ \0
		}
	}
	printf("\nformattxt = %s\n", formattxt);







	// !! ���� �������� �������� ������ ����� � �������� � ����������� � ��� ��������� �����
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

	// ��������� �����
	unsigned char bit_text = 0, dbit_text = 0; // ��� � ��������
	unsigned char bit_pic = 0, dbit_pic = 0; // ��� � ������
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


	printf("\n������:\n1 - ��������� ���� � ��������\n2 - ������� ���� �� ��������\n");
	int choice = 0;
	scanf_s("%d", &choice);
	if (choice == 1) code();
	else if (choice == 2) decode();
	else printf("\n�������� �����\n");

	return 0;
}

int read_int(int n, FILE* pic, short type) { // ������� ����� �� �����, type ��� ���� ��� 1 - �������� ����� int, ���� 2 - �� char
	unsigned char number = 0;
	unsigned char bit_from_pic = 0;
	for (int i = 0; i < (8 / n); i++) { //  � ������������ �� �������� �� ��������� ���� ����
		bit_from_pic = fgetc(pic);
		bit_from_pic = bit_from_pic << (8 - n); // ��������� ������ ��������� 2 ����
		bit_from_pic = bit_from_pic >> (8 - n);
		number = bit_from_pic | number; // ���������� �� � �����
		if (i == ((8 / n) - 1)) break;	// ���� ��� ��������� �������� �� ������� �� �����
		number = number << n; // �������
	}
	//printf("\nnumber in char = %c\n", number);
	if (type == 2) return(number); // ���� ����� char, �� ������ �� ����� ���� ������
	int intnumber = 0;
	// ��������� sizeoftxtlen �� char � int, ������� ��� ����� ��� (�� ����� �������� ���� � sizeoftxt ������ 9 ����)
	for (int i = 0; i < 10; i++) {
		if (number == (48 + i)) intnumber = i;
	}
	//printf("\nnumber in int = %d\n", intnumber);
	return (intnumber);
}

int perevodv2(int a, int* mass) {


	int i;
	for (i = 0; i < 8; i++)
	{
		mass[i] = 0;
	}

	i = 0;
	while ((a != 0) && (a != (-1)))
	{
		if (a % 2 == 0)
		{
			mass[i] = 0;
		}
		else
		{
			mass[i] = 1;
		}
		a = a >> 1;
		i++;
	}
	i = i - 1;

	//printf("mass in func before reverse:\t");
	//print_array(mass);

		//������
	int tmp;
	for (int i = 0; i < 8 / 2; ++i)  // �� ������ �� ��������
	{
		tmp = mass[i];      //���������� ������ ��������
		mass[i] = mass[8 - 1 - i]; //������ ������� �� ����� ������
		mass[8 - 1 - i] = tmp;  //������ ������ �� ����� �������
	}


	//printf("mass in func after reverse:\t");
	//print_array(mass);

	return(mass);
}