#pragma once
#define Version "1.1.0"

//loas-tools.h
void usage();
void cli_parser(char* argopts[], int optsum);

void Loas2FakeWave(const char* source, struct _opts* option);
void FakeWave2Loas(const char* source, struct _opts* option);

struct _opts {
	 bool exact;
	 bool overwrite;
	 uint8_t bitdepth;

};
