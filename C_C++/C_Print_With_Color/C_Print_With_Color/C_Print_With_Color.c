#include <stdio.h>

//��ɫ�궨��
#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"


int main()
{
	printf(RED"��ɫ****************************\n"NONE);
	printf(LIGHT_RED"����ɫ**************************\n"NONE);
	printf(GREEN"��ɫ****************************\n"NONE);
	printf(LIGHT_GREEN"����ɫ****************************\n"NONE);

	printf(BLUE"��ɫ******************************\n"NONE);
	printf(LIGHT_BLUE"����ɫ****************************\n"NONE);

	printf(DARY_GRAY"��ɫ******************************\n"NONE);
	printf(CYAN"��ɫ*****************************\n"NONE);
	printf(LIGHT_CYAN"����ɫ****************************\n"NONE);

	printf(PURPLE"��ɫ*****************************\n"NONE);
	printf(LIGHT_PURPLE"����ɫ****************************\n"NONE);

	printf(BROWN"��ɫ*****************************\n"NONE);
	printf(YELLOW"��ɫ****************************\n"NONE);
	printf(LIGHT_GRAY"����ɫ*****************************\n"NONE);
	printf(WHITE"��ɫ****************************\n"NONE);

	return 0;
}