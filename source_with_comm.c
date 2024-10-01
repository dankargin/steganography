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

print_byte(unsigned char x, short type) { // Я ТУТ

	if (type == 1) printf("Новый бит файла: ");
	if (type == 2) printf("Бит файла: ");
	if (type == 3) printf("Новый бит картинки: ");
	if (type == 4) printf("Бит картинки: ");

	int mass[16];
	memset(mass, 0, sizeof(mass));
	perevodv2(x, mass);

	for (int i = 0; i < 8; ++i)
		printf("%d", mass[i]);
	int d = 0;
	d = d | x;
	printf(", (10) = %d", d);

	if ((type == 1) || (type == 2)) printf(", ch = %c", x); // если это текст - выводим символ
	if ((type == 3) || (type == 4)) {  // если это картинка - выводим 16 ричный формат
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
	int n = 0;										// степень упаковки
	int symbol = 0, symboltxt = 0, sp = 0, sn = 0;
	int sdvigforsize = 18;				// свдиги (связано с спецификой .bmp)
	int width = 0, height = 0, bitperpix = 0;		// для расчета размеров картинки
	int sizeofpic = 0, sizeoftxt = 0, newsizeoftxt = 0;
	int point = 10;						 // типо "указатели"							
	unsigned short sizetxt[10];
	unsigned char formattxt[F], infotxt[G];
	long bitcount = 0;
	// 1 вводим степень упаковки

	//printf("Enter stepen upakovki:  ");
	//scanf("%d", &n);
	n = 2;

	if ((n > 0) && (n < 9)) printf("OK\n");
	else {
		printf("error : nevernaya stepen upakovki");
		return 0;
	}
	printf("\n");

	// 2 открываем картинку
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

	// 3 считаем размер картинки

		// считываем значение сдвига, чтобы потом скипнуть инфо о картинке
	int sdvig = getoffset(origpic);

	// свдигаемся до блока biSizeImage, чтобы считать размер картинки (для проверки)
	rewind(origpic);
	fseek(origpic, sdvigforsize, 0);
	// считываем, размер самой картинки в битах
	width = funcsize(origpic);
	height = funcsize(origpic);
	for (int i = 0; i < 3; i++) symbol = fgetc(origpic);
	bitperpix = symbol;
	sizeofpic = width * height * bitperpix;
	printf("witdh = %d, height = %d, bpp = %d, sizeofpic = %d\n\n", width, height, bitperpix, sizeofpic);


	// 4 открываем файл
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
	// 5 считаем размер файла (sizeoftxt)
	sizeoftxt = getlength(text);
	printf("\nsizeoftxt = %d\n", sizeoftxt);

	// 6 переходим к infotxt --------------- 
	printf("\n\n\nструктура infotxt:\n 1 - степень упаковки (n) \n 2 - formattxtlen (кол-во символов в формате, например txt = 3)\n 3 - formattxt (сам формат строкой, например ""jpeg"")\n 4 - кол-во цифр в sizeoftxt \n 5 - sizeoftxt числом (кол-во байт)\n\n");

	// отчищаем строки
	memset(infotxt, 0, sizeof(infotxt));
	memset(formattxt, 0, sizeof(formattxt));

	// 6.1 записываем расширение в файла в строку formattxt
		//считаем длину имени
	int z = strlen(nametxt);
	printf("strlen(nametxt) = %d\n", z);

	// ищем последнее вхождение точки в ней
	// типо указатель одновременно и массив, поэтому мы ставим указатель на точку в имени, и одновременно получается строка до символа конца строки
	char* dot = strrchr(nametxt, '.');
	dot++;

	// выписываем формат в formattxt
	for (int i = 0; (*(dot + i) != '\0'); i++) formattxt[i] = *(dot + i);
	printf("formattxt:");
	printf(" %s", formattxt);

	// 6.2 считаем длину расширения формата и записываем в formattxtlenght
	int formattxtlen = strlen(formattxt);

	// 6.3 начинаем записывать - записываем ст уп в первую ячейку
	int spoint = 0;
	char string[20];
	_itoa(n, string, 10); // itoa переводит число в строку, просто int в char не меняя значения
	infotxt[spoint++] = string[0];

	// 6.4 записываем кол-во цифр sizeoftxt это нужно чтобы вовремя остановится в чтении формата

	int j = sizeoftxt;
	int sizeoftxtlen = 0;
	for (; j > 0; sizeoftxtlen++) {  // сначала считаем
		j = j / 10;
	}
	memset(string, 0, sizeof(string));
	_itoa(sizeoftxtlen, string, 10);
	infotxt[spoint++] = string[0];


	// 6.5 записываем размер текста в байтах в infotxt
		//т.е. записать просто в строку число (sizeoftxt) по цифре

	memset(string, 0, sizeof(string));
	_itoa(sizeoftxt, string, 10);
	strcat(infotxt, string);
	spoint = spoint + strlen(string);
	// 6.6 записываем длину расширения во вторую ячейку
	_itoa(formattxtlen, string, 10);
	infotxt[spoint++] = string[0];

	// 6.7 затем само расширение с третьей ячейки
	strcat(infotxt, formattxt);
	spoint = spoint + strlen(formattxt);
	//puts(infotxt);



	// вывод infotxt
	printf("Итого infotxt = %s", infotxt);

	// 7 Оцениваем, поместиться ли файл + информация

	// 7.1 Суммируем размеры
	sizeoftxt + strlen(infotxt);

	// Переводим sizeoftxt в биты
	sizeoftxt *= 8;

	// учитывая степень упаковки считаем доступное для файла место
	int availsizeofpic = 0;
	if (n == 1) availsizeofpic = sizeofpic / 8;
	else if (n == 2) availsizeofpic = sizeofpic / 4;
	else if (n == 4) availsizeofpic = sizeofpic / 2;
	else if (n == 8) availsizeofpic = sizeofpic;
	// проверяем, можно ли вместить файл
	if (sizeoftxt > availsizeofpic) {
		printf("\n\nОшибка, файл не помещается (степень упаковки = %d, размер файла в битах = %d, размер картинки = %d)\n\n", n, sizeoftxt, sizeofpic);
		return 0;
	}
	else {
		printf("\n\nОК, файл поместится (степень упаковки = %d, размер файла в битах = %d, доступное место = %d, размер картинки = %d\n\n", n, sizeoftxt, availsizeofpic, sizeofpic);
	}

	sizeoftxt = sizeoftxt / 8; // переводим размер файла обратно в байты

	//-----------------------------------------------------------------

// 8 открываем картинку для записи
	newpic = fopen("picture.bmp", "wb");
	if (!newpic) {
		printf("error: failed to open a new picture");
		return 0;
	}

	printf("\nsdvig = %d\n", sdvig);

	// 10 копируем информацию о картинке в новую картинку
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

	// 11 сдвигаемся вправо от начала на sdvig, чтобы скипнуть инфо о файле и сразу перейти к пикселям
	rewind(origpic);
	fseek(origpic, sdvig, 0);
	rewind(newpic);
	fseek(newpic, sdvig, 0);

	// 12 кодирование побитовыми операциями
	unsigned char bit_text = 0, dbit_text = 0; // бит с картинки
	unsigned char bit_pic = 0, dbit_pic = 0; // бит с файла

	int dspoint = spoint;
	spoint = 1;

	// сначала записываем степень упаковки в первый байт (последние его 2 бита) по принципу 1 = 00, 2 = 01, 4 = 10, 8 = 11
		bit_pic = fgetc(origpic);
		// освободим последние два бита
		bit_pic = bit_pic & 252;
		// положим туда наши значения
		if (n == 1) bit_pic = bit_pic | 0;
		else if (n == 2) bit_pic = bit_pic | 1;
		else if (n == 4) bit_pic = bit_pic | 2;
		else if (n == 8) bit_pic = bit_pic | 3;
		fputc(bit_pic, newpic);
		//printf("\nbit with n = %d\n", bit_pic);

	while (!feof(origpic)) {


		if (spoint < dspoint) {
			
			bit_text = infotxt[spoint]; // затем записываем все infotxt
			//printf("\nвзяли infotxt[spoint] = %c\n", infotxt[spoint]);
			spoint++;
		}
		else {
			bit_text = fgetc(text);
			//printf("\nвзяли bit_text = fgetc(text) = %c\n", bit_text);
			dbit_text = bit_text;
		}
		

		for (int i = 0; i < (8 / n); i++) { 

			if (feof(text)) break;

			//printf("\n ------ \n");
			dbit_text = bit_text;
			//print_byte(dbit_text, 2);

			//printf("\n сдвинули влево - \n");
			dbit_text = (dbit_text << (n * i));
			//print_byte(dbit_text, 2);

			//printf("\n 3 (one) - ");
			char one = 255 << (8 - n);
			//print_byte(one, 2);

			//printf("\n выделили на первое место n бит - \n");
			dbit_text = dbit_text & one;
			//print_byte(dbit_text, 2);

			//printf("\n убрали их направо - \n");
			dbit_text = dbit_text >> (8 - n);
			//print_byte(dbit_text, 2);

			//printf("\n 6 - ");
			bit_pic = fgetc(origpic);
			//print_byte(bit_pic, 4);

			//printf("\n 7 (one) - ");
			one = 255 << n;
			//print_byte(one, 2);

			//printf("\n убрали с картинки последние n бит - \n");
			bit_pic = bit_pic & one;
			//print_byte(bit_pic, 4);

			//printf("\n записали туда биты текста и записали это в файл: - \n");
			dbit_text = dbit_text | bit_pic;
			//print_byte(dbit_text, 2);

			fputc(dbit_text, newpic);

			sp++; // это потом убрать

		}


		if (feof(text)) { // если текст закончился, то копируем все оставшиемя байты со старой картинки в новую

			while (!feof(origpic)) {
				bit_text = fgetc(origpic);
				fputc(bit_text, newpic);

				sn++; // это потом убрать
			}
		}
	}
	fclose(origpic);
	fclose(newpic);
	fclose(text);

	printf("\n\nПеределано %d байт, не тронуто %d байт\n", sp, sn);

	return 0;
}


void decode() {

	FILE* pic;
	FILE* newtext;

	// открываем картинку
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


	// считываем сдвиг
	int sdvig = getoffset(pic);
	printf("\nsdvig = %d\n", sdvig);
	unsigned char ch = 0;
	fseek(pic, sdvig, 0);

	// считываем степень упаковки

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
	
	// считываем кол-во цифр в размере файла (в соответсвии с infotxt
	printf("\n1\n");
	int sizeoftxtlen = read_int(n, pic, 1);
	
	// считываем размер
	int sizeoftxt = 0;
	int buf = 0;
	for (double i = 1; i <= sizeoftxtlen; i++) {
		buf = read_int(n, pic, 1);
		sizeoftxt += buf * pow((double)10, (double)(sizeoftxtlen - i));
	}

	printf("\nsizeoftxtlen = %d, sizeoftxt = %d\n", sizeoftxtlen, sizeoftxt);

	// считываем кол-во букв в формате
	int formattxtlen = read_int(n, pic, 1);
	char* formattxt;
	formattxt = (char*)malloc(sizeof(char)*(formattxtlen + 1)); // +1 для \0
	// считываем формат
	for (int i = 0; i < formattxtlen; i++) {
		formattxt[i] = read_int(n, pic, 2);
		if (i == (formattxtlen - 1)) {
			i++;
			formattxt[i] = '\0'; // в конце ставим \0
		}
	}
	printf("\nformattxt = %s\n", formattxt);







	// !! сюда дописать создание нового имени и создания в соотвествии с ним текстовго файла
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

	// считываем текст
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


	printf("\nВырите:\n1 - Поместить файл в картинку\n2 - Извлечь файл из картинки\n");
	int choice = 0;
	scanf_s("%d", &choice);
	if (choice == 1) code();
	else if (choice == 2) decode();
	else printf("\nНеверный выбор\n");

	return 0;
}

int read_int(int n, FILE* pic, short type) { // выводит число из байта, type это если тип 1 - получить нужно int, если 2 - то char
	unsigned char number = 0;
	unsigned char bit_from_pic = 0;
	for (int i = 0; i < (8 / n); i++) { //  в соответствии со степенью уп считываем один байт
		bit_from_pic = fgetc(pic);
		bit_from_pic = bit_from_pic << (8 - n); // оставляем только последние 2 бита
		bit_from_pic = bit_from_pic >> (8 - n);
		number = bit_from_pic | number; // записываем их в число
		if (i == ((8 / n) - 1)) break;	// если это последняя итерация то смещать не нужно
		number = number << n; // убираем
	}
	//printf("\nnumber in char = %c\n", number);
	if (type == 2) return(number); // если нужен char, то дальше не нужно ниче делать
	int intnumber = 0;
	// переведем sizeoftxtlen из char в int, сделаем это через зад (не будет работать если в sizeoftxt больше 9 цифр)
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

		//реверс
	int tmp;
	for (int i = 0; i < 8 / 2; ++i)  // тк делаем до половины
	{
		tmp = mass[i];      //сохранение левого элемента
		mass[i] = mass[8 - 1 - i]; //запись правого на место левого
		mass[8 - 1 - i] = tmp;  //запись левого на место правого
	}


	//printf("mass in func after reverse:\t");
	//print_array(mass);

	return(mass);
}