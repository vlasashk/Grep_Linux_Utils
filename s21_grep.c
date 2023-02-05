#include "s21_grep.h"

int main(int argc, char *argv[]) {
  char *pattern[ONE_PAGE] = {NULL};
  if (argc < 3) {
    fprintf(stderr, "Too few arguments\n");
  } else {
    GrepOptions grep_options;
    InitializeStruct(&grep_options);
    if (ArgsChecker(&grep_options, argc, argv, pattern) == 1) {
      if (grep_options.odd == 0 && grep_options.array_counter == 1) {
        NoFileWith_V(&grep_options, argv, pattern);
      } else {
        FileProcessing(&grep_options, argv, pattern);
      }
    }
  }
  for (int i = 0; pattern[i]; i++) {
    free(pattern[i]);
  }
  return 0;
}

void FileProcessing(GrepOptions *options, char *argv[], char *pattern[]) {
  int i = 0;
  if (options->odd == 0) {
    pattern[options->index_pat] =
        malloc(sizeof(char) * (strlen(argv[options->files_index[i]]) + 1));
    strcpy(pattern[options->index_pat], argv[options->files_index[i]]);
    i++;
    options->index_pat++;
    pattern[options->index_pat] = NULL;
  }
  for (; i < options->array_counter; i++) {
    char *file_name = argv[options->files_index[i]];
    if (access(file_name, F_OK) != 0) {
      if (options->s == 0) {
        fprintf(stderr, "s21_grep: %s: No such file or directory\n", file_name);
      }
    } else {
      if (CheckIfBinary(file_name) == 0) {
        fprintf(stderr, "s21_grep: %s: binary file matches\n", file_name);
      } else {
        char buffer[ONE_PAGE];
        options->lines_ok = 0;
        options->lines_num = 0;
        FILE *file = fopen(file_name, "r");
        while (fgets(buffer, ONE_PAGE, file)) {
          if (buffer[strlen(buffer) - 1] != '\n') {
            int len = strlen(buffer);
            buffer[len] = '\n';
            buffer[len + 1] = '\0';
          }
          options->lines_num++;
          if (GrepProcessing(options, buffer, pattern, file_name) == 1) {
            break;
          }
        }
        if (options->c == 1) {
          HandleFlag_C(options, file_name);
        }
        fclose(file);
      }
    }
  }
}

void NoFileWith_V(GrepOptions *options, char *argv[], char *pattern[]) {
  char buffer[ONE_PAGE];
  int bytes_size;
  pattern[options->index_pat] =
      malloc(sizeof(char) * (strlen(argv[options->files_index[0]]) + 1));
  strcpy(pattern[options->index_pat], argv[options->files_index[0]]);
  options->index_pat++;
  pattern[options->index_pat] = NULL;
  bytes_size = read(STDIN_FILENO, buffer, ONE_PAGE);
  buffer[bytes_size] = '\0';
  char *file_name = "(standard input)";
  while (bytes_size > 0) {
    options->lines_num++;
    if (GrepProcessing(options, buffer, pattern, file_name) == 1) {
      break;
    }
    bytes_size = read(STDIN_FILENO, buffer, ONE_PAGE);
    buffer[bytes_size] = '\0';
  }
  if (options->c == 1) {
    HandleFlag_C(options, file_name);
  }
}

int GrepProcessing(GrepOptions *options, char *line, char *pattern[],
                   char *file_name) {
  char *temp_vars[ONE_PAGE] = {NULL};
  int temp_id[ONE_PAGE] = {'\0'};
  int stop = 0;
  options->matched_size = 0;
  int status = RegularExHandle(options, line, pattern, temp_vars, temp_id);
  int search_info = (status == !(options->v));
  if (status == -1) {
    stop = 1;
  } else {
    if (options->l == 1) {
      if (search_info == 1) {
        printf("%s\n", file_name);
        stop = 1;
      }
    } else {
      if (search_info == 1) {
        options->lines_ok++;
        if (options->c == 0) {
          if (HandleFlag_O(options, temp_vars, file_name, temp_id) == 0) {
            HandleHeader(options, file_name);
            HandleFlag_N(options);
            printf("%s", line);
          }
        }
      }
    }
  }
  for (int i = 0; temp_vars[i]; i++) {
    free(temp_vars[i]);
  }
  return stop;
}

int RegularExHandle(GrepOptions *options, char *line, char *pattern[],
                    char *temp_vars[], int temp_id[]) {
  int found = 0;
  for (int i = 0; pattern[i]; i++) {
    regex_t regex;
    if (options->i == 1) {
      if (regcomp(&regex, pattern[i], REG_NEWLINE | REG_ICASE | REG_EXTENDED) !=
          0) {
        found = -1;
        fprintf(stderr, "s21_grep: Regular expression compilation error\n");
        regfree(&regex);
        break;
      }
    } else {
      if (regcomp(&regex, pattern[i], REG_NEWLINE | REG_EXTENDED) != 0) {
        found = -1;
        fprintf(stderr, "s21_grep: Regular expression compilation error\n");
        regfree(&regex);
        break;
      }
    }
    regmatch_t pmatch;
    if (options->o && !options->v) {
      char *buff = line;
      int bufer_id = 0;
      while (!regexec(&regex, buff, 1, &pmatch, 0)) {
        int from = (int)pmatch.rm_so;
        int to = (int)pmatch.rm_eo;
        temp_vars[options->matched_size] =
            malloc(sizeof(char) * (to - from + 1));
        GetSubString(buff, temp_vars[options->matched_size], from, to - 1);
        temp_id[options->matched_size] = bufer_id + from;
        bufer_id += to;
        options->matched_size++;
        buff += pmatch.rm_eo;
        found = 1;
      }
    } else if (regexec(&regex, line, 1, &pmatch, 0) == 0) {
      found = 1;
      if (options->o != 1 || options->v != 0) {
        regfree(&regex);
        break;
      }
    }
    regfree(&regex);
  }
  return found;
}

int CheckIfBinary(char *filename) {
  FILE *f = fopen(filename, "r");
  int flag = 1;
  char c;
  while ((c = fgetc(f)) != EOF) {
    if (c == 0) {
      flag = 0;
      break;
    }
  }
  fclose(f);
  return flag;
}

void HandleHeader(GrepOptions *options, char *file_name) {
  if (options->array_counter > 1 && options->h == 0) {
    if (options->odd != 0) {
      printf("%s:", file_name);
    } else if (options->array_counter > 2) {
      printf("%s:", file_name);
    }
  }
}
void HandleFlag_C(GrepOptions *options, char *file_name) {
  if (options->lines_ok != 0 || options->l != 1) {
    HandleHeader(options, file_name);
    printf("%d\n", options->lines_ok);
  }
}

void HandleFlag_N(GrepOptions *options) {
  if (options->n == 1) {
    printf("%d:", options->lines_num);
  }
}

int HandleFlag_O(GrepOptions *options, char *temp_vars[], char *file_name,
                 int temp_id[]) {
  int flag = 0;
  if (options->o == 1) {
    flag = 1;
    BubbleSortChars(temp_vars, temp_id, options->matched_size);
    for (int i = 0; temp_vars[i]; i++) {
      HandleHeader(options, file_name);
      HandleFlag_N(options);
      printf("%s\n", temp_vars[i]);
    }
  }
  return flag;
}

int HandleOption_F(GrepOptions *options, char *file_name, char *pattern[]) {
  int flag = 1;
  if (access(file_name, F_OK) != 0) {
    flag = 0;
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", file_name);
  } else {
    char buffer[ONE_PAGE];
    FILE *file = fopen(file_name, "r");
    while (fgets(buffer, ONE_PAGE, file)) {
      if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
      }
      pattern[options->index_pat] = malloc(sizeof(char) * (strlen(buffer) + 1));
      strcpy(pattern[options->index_pat], buffer);
      options->index_pat++;
    }
    fclose(file);
  }
  return flag;
}

int GetSubString(const char *source, char *target, int from, int to) {
  int length = 0;
  int i = 0, j = 0;
  while (source[i++] != '\0')
    length++;
  for (i = from, j = 0; i <= to; i++, j++) {
    target[j] = source[i];
  }
  target[j] = '\0';

  return 0;
}

void BubbleSortChars(char *temp_vars[], int *temp_id, int len) {
  int position;
  char *temp_string;
  for (int pass = 0; pass < len - 1; ++pass) {
    for (int n = 0; n < len - 1; ++n) {
      if (temp_id[n] > temp_id[n + 1]) {
        position = temp_id[n];
        temp_id[n] = temp_id[n + 1];
        temp_id[n + 1] = position;

        temp_string = temp_vars[n];
        temp_vars[n] = temp_vars[n + 1];
        temp_vars[n + 1] = temp_string;
      }
    }
  }
}

int OptionParser(GrepOptions *options, char *argv) {
  int flag = 1;
  for (char *c = argv; *c; c++) {
    if (flag == 1) {
      if (*c == 'e' || *c == 'f') {
        options->odd = 1;
        if (*c == 'e') {
          options->e = 1;
        } else {
          options->f = 1;
        }
        break;
      }
      switch (*c) {
      case 'i':
        options->i = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'c':
        options->c = 1;
        break;
      case 'l':
        options->l = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 'h':
        options->h = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 'o':
        options->o = 1;
        break;
      default:
        fprintf(stderr, "s21_cat: invalid option --'%c'\n", *c);
        flag = 0;
        break;
      }
    } else {
      break;
    }
  }
  return flag;
}

int ArgsChecker(GrepOptions *options, int argc, char *argv[], char *pattern[]) {
  int flag = 1;
  char buffer[1024] = {'\0'};
  for (int i = 1; i != argc; i++) {
    if (*argv[i] == '-') {
      argv[i]++;
      if ((flag = OptionParser(options, argv[i])) == 0) {
        break;
      }
      if ((options->e + options->f) != 0) {
        while ((*(argv[i]) != 'e') && (*(argv[i]) != 'f')) {
          ++argv[i];
        }
        if (*(++argv[i]) == '\0') {
          if (++i > (argc - 1)) {
            fprintf(stderr, "s21_grep: option requires an argument -- '%c'\n",
                    *(argv[i - 1] - 1));
            flag = 0;
            break;
          } else {
            if (options->e == 1) {
              pattern[options->index_pat] =
                  malloc(sizeof(char) * (strlen(argv[i]) + 1));
              strcpy(pattern[options->index_pat], argv[i]);
              options->index_pat++;
            } else {
              if ((flag = HandleOption_F(options, argv[i], pattern)) == 0) {
                break;
              }
            }
          }
        } else {
          int buff_i = 0;
          for (char *c = argv[i]; *c; c++) {
            buffer[buff_i++] = *c;
          }
          buffer[buff_i] = '\0';
          if (options->e == 1) {
            pattern[options->index_pat] = malloc(sizeof(char) * (buff_i + 1));
            strcpy(pattern[options->index_pat], buffer);
            options->index_pat++;
          } else {
            if ((flag = HandleOption_F(options, buffer, pattern)) == 0) {
              break;
            }
          }
        }
        options->e = 0;
        options->f = 0;
      }
    } else {
      options->files_index[options->array_counter] = i;
      options->array_counter++;
    }
  }
  pattern[options->index_pat] = NULL;
  return flag;
}

void InitializeStruct(GrepOptions *init_opt) {
  init_opt->e = 0;
  init_opt->i = 0;
  init_opt->v = 0;
  init_opt->c = 0;
  init_opt->l = 0;
  init_opt->n = 0;
  init_opt->h = 0;
  init_opt->s = 0;
  init_opt->f = 0;
  init_opt->o = 0;
  init_opt->odd = 0;

  init_opt->index_pat = 0;
  init_opt->lines_num = 0;
  init_opt->lines_ok = 0;
  init_opt->matched_size = 0;

  init_opt->array_counter = 0;
  for (int i = 0; i < ARGV_ARR; i++) {
    init_opt->files_index[i] = '\0';
  }
}