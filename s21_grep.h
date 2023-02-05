#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ONE_PAGE 4096
#define ARGV_ARR 128

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int odd;

  int index_pat;
  int lines_num;
  int lines_ok;
  int matched_size;

  int array_counter;
  unsigned files_index[ARGV_ARR];
} GrepOptions;

void InitializeStruct(GrepOptions *init_opt);
int ArgsChecker(GrepOptions *options, int argc, char *argv[], char *pattern[]);
int OptionParser(GrepOptions *options, char *argv);
int OddOptionsParser(int argc, char *argv[], char *pattern[], int *counter,
                     int *i);
void FileProcessing(GrepOptions *options, char *argv[], char *pattern[]);
int CheckIfBinary(char *filename);
int RegularExHandle(GrepOptions *options, char *line, char *pattern[],
                    char *temp_vars[], int temp_id[]);
int GrepProcessing(GrepOptions *options, char *line, char *pattern[],
                   char *file_name);
void HandleFlag_C(GrepOptions *options, char *file_name);
void HandleHeader(GrepOptions *options, char *file_name);
void HandleFlag_N(GrepOptions *options);
regex_t HandleFlag_I(GrepOptions *options, char *pattern, int *found);
int GetSubString(const char *source, char *target, int from, int to);
int HandleFlag_O(GrepOptions *options, char *temp_vars[], char *file_name,
                 int temp_id[]);
void BubbleSortChars(char *temp_vars[], int *temp_id, int len);
void NoFileWith_V(GrepOptions *options, char *argv[], char *pattern[]);

#endif // SRC_GREP_S21_GREP_H_
