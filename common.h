#ifndef COMMON_H
#define COMMON_H

#define BUFF_LEN 0xFF

typedef struct {
  char *args[2];            /* arg[1]=dim.width and arg[2]=dim.height */
  int verbose;              /* The -v flag */
  int quantity;               /* The -n flag */
  char *outfile;            /* Argument for -o */
  double bp_ratio;          /* Arguments for black pixels ratio*/
} arg_t;

extern arg_t arguments;

#endif

