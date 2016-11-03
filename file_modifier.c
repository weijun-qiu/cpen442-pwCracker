#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *fp = fopen("42701136.program2.exe", "r+");
	if (fseek(fp, 0x484E, SEEK_SET)) {
		printf("fseek: operation failed\n");
		getchar();
		return 1;
	}
	fputc(0x01, fp);
	fclose(fp);
	
	printf("operation completed\n");
	getchar();
	return 0;
}