#include "realized_grep.h"

int main(int argc, char *argv[]) {
  int error = 0;
  if (argc > 1) {
    struct OPT options = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0};
    int counter_not_files = 0;
    count_not_files(argc, argv, &counter_not_files);
    options.templates = malloc(1 * sizeof(char *));
    error = parse_options(argc, argv, &options, &counter_not_files);

    if (error == 0) {
      int counter_files = argc - counter_not_files - 1;
      char **filenames = malloc((counter_files) * sizeof(char *));
      int is_manyf = 0;
      if (counter_files > 1) {
        is_manyf = 1;
      }
      parse_filenames(argc, argv, filenames, counter_not_files);
      int all_fail = 1;
      int result = 0;
      for (int i = 0; i < counter_files; i++) {
        result = search(filenames[i], is_manyf, options);
        if (result == 0) {
          all_fail = 0;
        } else if (result == 2) {
          error = 1;
        }
      }
      if (all_fail) error = 1;
      free(filenames);
    }

    if (options.templates != NULL) {
      for (int i = 0; i < options.templates_num; i++) {
        free(options.templates[i]);
      }
      free(options.templates);
    }
  } else {
    fprintf(stderr, "usage: grep [-eivclnhsof] [pattern] [file ...]");
    error = 1;
  }
  return error;
}

int search(char *filename, int is_manyf, struct OPT options) {
  int result = 0;
  char *tmp_line = NULL;
  size_t len = 0;
  ssize_t read;
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    result = 2;
    if (options.s == 0) {
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    }
  } else {
    int number_line = 0;
    int find_lines = 0;
    while ((read = getline(&tmp_line, &len, fp)) != -1) {
      number_line += 1;
      if ((options.v == 0 && match(tmp_line, options) == 0) ||
          (options.v && match(tmp_line, options) != 0)) {
        find_lines += 1;
        if (options.l == 0 && options.c == 0) {
          many_files(filename, is_manyf, options);
          option_n(options, number_line);
          output_matching_line(options, tmp_line);
        }
      }
    }
    if (options.c && options.l == 0) {
      many_files(filename, is_manyf, options);
      printf("%d\n", find_lines);
    } else if (options.c && options.l) {
      many_files(filename, is_manyf, options);
      if (find_lines > 0)
        printf("%d\n", 1);
      else
        printf("%d\n", 0);
    }
    if (options.l && find_lines > 0) {
      printf("%s\n", filename);
    }
    if (find_lines == 0) result = 1;
    if (tmp_line) free(tmp_line);

    fclose(fp);
  }
  return result;
}

void output_matching_line(struct OPT res_options, char *string) {
  if (res_options.o == 0 || (res_options.o && res_options.v)) {
    printf("%s", string);
    if (string[strlen(string) - 1] != '\n') printf("\n");
  } else if (res_options.o && res_options.v == 0) {
    int status = 0;
    for (int i = 0; i < res_options.templates_num; i++) {
      regex_t regex;
      if (generate_regex(&regex, res_options, i) == 0) {
        char *ps = string;
        regmatch_t pmatch[100];
        int eflag = 0;
        while ((status = regexec(&regex, ps, 1, pmatch, eflag)) == 0) {
          for (long long i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
            printf("%c", ps[i]);
          }
          printf("\n");
          ps += pmatch[0].rm_eo;
          eflag = REG_NOTBOL;
        }
      }
      regfree(&regex);
    }
  }
}

void option_n(struct OPT res_options, int number_line) {
  if (res_options.n) {
    printf("%d:", number_line);
  }
}

void many_files(char *filename, int is_manyf, struct OPT options) {
  if (is_manyf && options.h == 0) {
    printf("%s:", filename);
  }
}

int generate_regex(regex_t *regex, struct OPT res_options, int id_template) {
  int status = 1;
  if (res_options.i) {
    status = regcomp(regex, res_options.templates[id_template], REG_ICASE);
  } else {
    status = regcomp(regex, res_options.templates[id_template], REG_EXTENDED);
  }
  return status;
}

int match(char *string, struct OPT res_options) {
  int status = 1;
  for (int i = 0; i < res_options.templates_num; i++) {
    regex_t regex;
    if (generate_regex(&regex, res_options, i) == 0) {
      if (regexec(&regex, string, 0, NULL, 0) == 0) {
        status = 0;
      }
    }
    regfree(&regex);
  }
  return status;
}

int read_template_from_file(char *filename, struct OPT *options) {
  int error = 0;
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    error = 1;
    if (options->s == 0) {
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    }
  } else {
    char *tmp_line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&tmp_line, &len, fp)) != -1) {
      process_regex(options, tmp_line);
    }
    if (tmp_line) free(tmp_line);
    fclose(fp);
  }
  return error;
}

void count_not_files(int argc, char *argv[], int *counter_not_files) {
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] == '-') {
      *counter_not_files += 1;
      if ((argv[i][1] == 'e' || argv[i][1] == 'f') &&
          (int)strlen(argv[i]) == 2) {
        *counter_not_files += 1;
      }
    }
  }
}

void process_regex(struct OPT *options, char *patterns) {
  options->templates = (char **)realloc(
      options->templates, (options->templates_num + 1) * sizeof(char *));
  options->templates[options->templates_num] =
      malloc((strlen(patterns) + 1) * sizeof(char));
  int j = 0;
  int length = (int)strlen(patterns);
  for (int i = 0; i < length; i++) {
    if (i < length - 1) {
      if (patterns[i] == '\\' && patterns[i + 1] == '|') {
        options->templates[options->templates_num][j] = '\0';
        j = 0;
        options->templates_num += 1;
        options->templates = (char **)realloc(
            options->templates, (options->templates_num + 1) * sizeof(char *));
        options->templates[options->templates_num] =
            malloc((strlen(patterns) + 1) * sizeof(char));
        i += 1;
      } else {
        options->templates[options->templates_num][j] = patterns[i];
        j += 1;
      }
    } else {
      if (patterns[i] == '\n')
        options->templates[options->templates_num][j] = '\0';
      else {
        options->templates[options->templates_num][j++] = patterns[i];
        options->templates[options->templates_num][j] = '\0';
      }
    }
  }
  options->templates_num += 1;
}

void parse_filenames(int argc, char *argv[], char **filenames,
                     int counter_not_files) {
  for (int i = counter_not_files + 1; i < argc; i++) {
    filenames[i - counter_not_files - 1] = argv[i];
  }
}

int parse_options(int argc, char *argv[], struct OPT *options,
                  int *counter_not_files) {
  const char *short_options = "e:ivclnhsof:";
  const struct option long_options[] = {{0, 0, 0, 0}};
  int new_option = 0, error = 0;
  while ((new_option = getopt_long(argc, argv, short_options, long_options,
                                   NULL)) != -1 &&
         error == 0) {
    switch (new_option) {
      case 'e':
        options->e = 1;
        process_regex(options, optarg);
        break;
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
      case 'f':
        options->f = 1;
        if (read_template_from_file(optarg, options) != 0) error = 1;
        break;
      case '?':
        fprintf(stderr,
                "grep: illegal option\nusage: grep [-eivclnhsof] "
                "template [file_name]");
        error = 1;
        break;
    }
  }
  if (options->templates_num == 0) {
    process_regex(options, argv[*counter_not_files + 1]);
    *counter_not_files += 1;
  }
  if (options->l == 1 || options->c == 1) options->n = 0;
  return error;
}