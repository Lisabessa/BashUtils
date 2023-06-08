#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct OPT {
  int e;  // pattern
  int i;  // ignore uppercase vs. lowercase
  int v;  // invert match
  int c;  // output count of matching lines only
  int l;  // output matching files only
  int n;  // precede each matching line with a line number
  int h;  // output matching lines without preceding them by file names
  int s;  // suppress error messages about nonexistent or unreadable files
  int f;  // take regexes from a file
  int o;  //  output the matched parts of a matching line
  char **templates;
  int templates_num;
};

int search(char *filename, int is_manyf, struct OPT options);

int match(char *string, struct OPT res_options);

void many_files(char *filename, int is_manyf, struct OPT options);

void option_n(struct OPT res_options, int number_line);

int generate_regex(regex_t *regex, struct OPT res_options, int id_template);

void output_matching_line(struct OPT res_options, char *string);

void parse_filenames(int argc, char *argv[], char **filenames,
                     int counter_not_files);

int parse_options(int argc, char *argv[], struct OPT *options,
                  int *counter_not_files);

void count_not_files(int argc, char *argv[], int *counter_not_files);

void process_regex(struct OPT *options, char *patterns);

int read_template_from_file(char *filename, struct OPT *options);

int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex);