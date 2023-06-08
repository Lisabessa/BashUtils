#include "realized_cat.h"

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "en_US.UTF-8");
  int error = 0;
  if (argc > 1) {
    struct OPT options = {0, 0, 0, 0, 0, 0, 0, 0};
    int counter_options = 0;

    error = parse_options(argc, argv, &options, &counter_options);

    if (error == 0) {
      char **filenames = malloc((argc - counter_options - 1) * sizeof(char *));
      int counter_files = argc - counter_options - 1;

      parse_filenames(argc, argv, filenames, counter_options);

      for (int i = 0; i < counter_files; i++) {
        error = display_file(filenames[i], options);
      }
      free(filenames);
    }

  } else {
    fprintf(stderr, "usage: cat [-beEstnTv] [file ...]");
    error = 1;
  }

  return error;
}

void parse_filenames(int argc, char *argv[], char **filenames,
                     int counter_options) {
  for (int i = counter_options + 1; i < argc; i++) {
    filenames[i - counter_options - 1] = argv[i];
  }
}

int parse_options(int argc, char *argv[], struct OPT *options,
                  int *counter_options) {
  const char *short_options = "+beEstnTv";
  const struct option long_options[] = {{"number-nonblank", 0, &options->b, 1},
                                        {"number", 0, &options->n, 1},
                                        {"squeeze-blank", 0, &options->s, 1},
                                        {NULL, 0, NULL, 0}};
  int new_option = 0, error = 0;

  while ((new_option = getopt_long(argc, argv, short_options, long_options,
                                   NULL)) != -1 &&
         error == 0) {
    switch (new_option) {
      case 'b':
        options->b = 1;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 'E':
        options->E = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'T':
        options->T = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case '?':
        fprintf(stderr,
                "cat: illegal option\nusage: cat [-benstuv] [file ...]");
        error = 1;
        break;
    }
    *counter_options += 1;
  }
  if (options->b == 1 && options->n == 1) options->n = 0;

  return error;
}

void option_n(struct OPT res_options, int current_c, int *number_line,
              int *start) {
  if (res_options.n && (current_c == '\n' || *start)) {
    if (*start) {
      *start = 0;
    }
    printf("%6d\t", *number_line);
    *number_line += 1;
  }
}

void option_b(struct OPT res_options, int current_c, int future_c,
              int *number_line, int *start) {
  if (res_options.b && future_c != '\n' && (current_c == '\n' || *start)) {
    if (*start) {
      *start = 0;
    }
    printf("%6d\t", *number_line);
    *number_line += 1;
  }
}

void option_e(struct OPT res_options, int future_c) {
  if ((res_options.E || res_options.e) && future_c == '\n') {
    printf("%c", '$');
  }
}

void option_v(struct OPT res_options, int *future_c) {
  if (res_options.v && *future_c != '\n' && *future_c != '\t') {
    if (*future_c >= 32) {
      if (*future_c == 127) {
        printf("^");
        *future_c = '?';
      } else if (*future_c > 127 && isprint(*future_c) == 0) {
        printf("M-^");
        *future_c -= 64;
      }
    }
    if (iscntrl(*future_c) != 0 && *future_c < ' ') {
      printf("^");
      *future_c += 64;
    }
  }
}

void option_t(struct OPT res_options, int future_c) {
  if ((res_options.T || res_options.t) && future_c == '\t') {
    printf("%s", "^I");
  }
}

void option_s(struct OPT res_options, int *future_c, int *current_c, FILE *fp,
              int *number_line, int *start) {
  if (res_options.s && *future_c == '\n' && *current_c == '\n') {
    while (*future_c == '\n') {
      *current_c = *future_c;
      *future_c = fgetc(fp);
    }
    option_e(res_options, *current_c);
    printf("%c", *current_c);
    option_n(res_options, *current_c, number_line, start);
  }
}

int display_file(char *filename, struct OPT res_options) {
  int error = 0;
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory", filename);
    error = 1;
  } else {
    int current_c = ' ';
    int future_c = ' ';
    int number_line = 1;
    int start = 1;

    while ((future_c = fgetc(fp)) != EOF) {
      option_n(res_options, current_c, &number_line, &start);
      option_s(res_options, &future_c, &current_c, fp, &number_line, &start);
      option_b(res_options, current_c, future_c, &number_line, &start);
      option_e(res_options, future_c);
      option_v(res_options, &future_c);
      option_t(res_options, future_c);

      current_c = future_c;

      if (!((res_options.T || res_options.t) && future_c == '\t')) {
        printf("%c", current_c);
      }
    }
    fclose(fp);
  }
  return error;
}