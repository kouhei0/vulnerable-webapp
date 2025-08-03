#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "types.h"

int
save(const struct record *record)
{
	int fd;
	struct header header = {0};

	if ((fd = open("db.bin", O_RDWR)) < 0) {
		printf("Error: open db.bin. %s\n", strerror(errno));
		return -1;
	}

	if (read(fd, &header, sizeof(header)) < 0) {
		printf("Error: read db.bin. %s\n", strerror(errno));
		return -1;
	}

	int idx, block;
	char usedRecord[MAX_RECORDS + 1] = {0};
	for (idx = 0; idx < MAX_RECORDS && header.index[idx]; idx++)
		usedRecord[header.index[idx]] = 1;
	for (block = 1; usedRecord[block]; block++);
	if (idx >= MAX_RECORDS || block > MAX_RECORDS) {
		block = header.index[0];
		for (idx = 0; idx < MAX_RECORDS - 1; idx++)
			header.index[idx] = header.index[idx + 1];
	}
	header.index[idx] = block;

	if (lseek(fd, block * sizeof(struct record), SEEK_SET) < 0) {
		printf("Error: lseek db.bin. %s\n", strerror(errno));
		return -1;
	}

	if (write(fd, record, sizeof(*record)) < 0) {
		printf("Error: lseek db.bin. %s\n", strerror(errno));
		return -1;
	}

	if (lseek(fd, 0, SEEK_SET) < 0) {
		printf("Error: lseek db.bin. %s\n", strerror(errno));
		return -1;
	}

	if (write(fd, &header, sizeof(header)) < 0) {
		printf("Error: lseek db.bin. %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int
parse(struct record *record)
{
	int i = 0;
	int ch;
	char key[8];
	char value[32];
	char buf[32];
	while (1) {
		ch = getchar();
		if (ch == '%') {
			char tmp[3];
			tmp[0] = getchar();
			tmp[1] = getchar();
			tmp[2] = '\0';
			buf[i++] = strtol(tmp, NULL, 16);
		} else if (ch == '=') {
			buf[i] = '\0';
			strcpy(key, buf);
			i = 0;
		} else if (ch == '&' || ch == EOF) {
			buf[i] = '\0';
			strcpy(value, buf);
			i = 0;

			if (!strcmp("name", key))
				strcpy(record->name, value);
			else if (!strcmp("body", key))
				strcpy(record->body, value);
		} else {
			buf[i++] = ch;
		}
		if (ch == EOF)
			break;
	}

	if (!strlen(record->name) || !strlen(record->body)) {
		printf("Error: 入力してください\n");
		return -1;
	}

	time_t t;
	time(&t);
	record->timestamp = t;

	return 0;
}

int
main()
{
	setvbuf(stdout, NULL, _IONBF, 0);

	printf("Content-type: text/html; charset=utf-8\n");
	printf("\n");
	printf(
		"<!DOCTYPE html>\n"
		"<html>\n"
		"  <body>\n"
	);

	char *method = getenv("REQUEST_METHOD");
	if (method == NULL || strcmp(method, "POST")) {
		printf("Error: invalid method: %s\n", method);
		return 0;
	}

	struct record record = {0};
	if (parse(&record))
		return 0;
	
	if (save(&record))
		return 0;

	printf(
		"投稿しました<br>"
		"<a href=\"./view.cgi\">戻る</a>\n"
	);

	printf(
		"  </body>\n"
		"</html>\n"
	);
}