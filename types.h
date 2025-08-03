#pragma once 

#define MAX_RECORDS 31
#include <time.h>

struct header {
	char index[MAX_RECORDS];
	char unused[33];
} __attribute__((__packed__));

struct record {
	time_t timestamp;
	char unused[8];
	char name[16];
	char body[32];
} __attribute__((__packed__));

struct record_list {
	struct record *p;
	struct record_list *next;
};