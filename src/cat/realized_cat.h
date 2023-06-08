#include <ctype.h>
#include <getopt.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct OPT {
  int E;  // display end-of-line characters as $
  int b;  // numbers only non-empty lines
  int e;  // display end-of-line characters as $ + v
  int n;  // number all output lines
  int s;  // squeeze multiple adjacent blank lines
  int t;  // display tabs as ^I + v
  int T;  // display tabs as ^I
  int v;  // show nonprinting use ^ and M- notation
};

int parse_options(int argc, char *argv[], struct OPT *options,
                  int *counter_options);

void parse_filenames(int argc, char *argv[], char **filenames,
                     int counter_options);

void option_b(struct OPT res_options, int current_c, int future_c,
              int *number_line, int *start);

void option_e(struct OPT res_options, int future_c);

void option_n(struct OPT res_options, int current_c, int *number_line,
              int *start);

void option_s(struct OPT res_options, int *future_c, int *current_c, FILE *fp,
              int *number_line, int *start);

void option_v(struct OPT res_options, int *future_c);

void option_t(struct OPT res_options, int future_c);

int display_file(char *filename, struct OPT res_options);

int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex);