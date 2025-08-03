#include <stdio.h>

int
main(int argc, char *argv[], char *envp[])
{
	printf("Content-type: text/html; charset=utf-8\n");
	printf("\n");

	printf("<pre>\n");
	printf("argc: %d\n", argc);
	int i;
	for (i = 0; i < argc; i++)
		printf("argv[%d]: %s\n", i, argv[i]);
	printf("\n");

	for (i = 0; envp[i]; i++)
		printf("envp[%d]: %s\n", i, envp[i]);

	printf("\n");

	printf("stdin: ");
	int ch;
	while ((ch = getchar()) != EOF)
		putchar(ch);

	printf("</pre>\n");
}