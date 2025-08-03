#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "types.h"

struct record_list *
load()
{
	int fd;
	struct header header = {0};
	struct record_list *head = NULL;
	
	if ((fd = open("db.bin", O_RDONLY)) < 0) {
		printf("Error: open db.bin. %s\n", strerror(errno));
		return NULL;
	}

	if (read(fd, &header, sizeof(header)) < 0) {
		printf("Error: read db.bin. %s\n", strerror(errno));
		return NULL;
	}

	int i;
	for (i = 0; i < MAX_RECORDS && header.index[i]; i++) {
		if (lseek(fd, header.index[i] * sizeof(struct record), SEEK_SET) < 0) {
			printf("Error: lseek db.bin. %s\n", strerror(errno));
			return NULL;
		}
		struct record *record;
		if ((record = malloc(sizeof(*record))) == NULL) {
			printf("Error: malloc. %s\n", strerror(errno));
			return NULL;
		}
		if (read(fd, record, sizeof(*record)) < 0) {
			printf("Error: read db.bin. %s\n", strerror(errno));
			return NULL;
		}

		struct record_list *newHead;
		if ((newHead = malloc(sizeof(*newHead))) == NULL) {
			printf("Error: malloc. %s\n", strerror(errno));
			return NULL;
		}
		newHead->next = head;
		newHead->p = record;
		head = newHead;
	}
	return head;
}

void
print(const struct record_list *p)
{
	int i;
	printf(
		"<section>\n"
		"  <header>\n"
		"    <h2>投稿一覧</h2>\n"
		"  </header>\n"
	);
	for (i = 0; i < MAX_RECORDS && p; i++, p = p->next) {
		struct record *record = p->p;
		struct tm tm;
		time_t t = record->timestamp;
		localtime_r(&t, &tm);
		printf("<article class=\"record\">\n");
		printf("  <header>\n");
		printf("    <address>%s</address>\n", record->name);
		printf("    <time>%04d-%02d-%02d %02d:%02d:%02d</time>\n",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec
		);
		printf("  </header>\n");
		printf("  <div>%s<div>\n", record->body);
		printf("</article>\n");
	}
	printf("</section>\n");
}
int
main()
{
	printf("Content-type: text/html; charset=utf-8\n");
	printf("\n");

	struct record_list *list = load();

	printf(
		"<!DOCTYPE html>\n"
		"<html lang=\"ja\">\n"
		"  <head>\n"
		"    <base href=\"/\">\n"
		"    <link href=\"global.css\" rel=\"stylesheet\">\n"
		"    <title>おトクな掲示板</title>\n"
		"  </head>\n"
		"  <body>\n"
		"    <header>\n"
		"      <h1>おトクな掲示板</h1>\n"
		"    </header>\n"
		"    <main>"
		"      <section>\n"
		"        <header>\n"
		"          <h2>投稿</h2>"
		"        </header>\n"
		"        <form action=\"cgi-bin/post.cgi\" method=\"post\">\n"
		"          <div class=\"form\">"
		"            <div>\n"
		"              <label for=\"name\">お名前</label>\n"
		"              <input type=\"text\" autocomplete=\"on\" id=\"name\" name=\"name\" placeholder=\"15 文字まで\">\n"
		"            </div>\n"
		"            <div>\n"
		"              <label for=\"body\">本文</label>\n"
		"              <input type=\"text\" autocomplete=\"on\" id=\"body\" name=\"body\" placeholder=\"31 文字まで\">\n"
		"            </div>\n"
		"            <div>\n"
		"              <input type=\"submit\">\n"
		"            </div>\n"
		"          </div>\n"
		"        </form>\n"
		"      </section>\n"
	);
	print(list);
	printf(
		"    </main>\n"
		"  </body>\n"
		"</html>"
	);
}