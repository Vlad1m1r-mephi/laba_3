#ifndef NUMBER_IO_H
#define NUMBER_IO_H

#include <stddef.h>

size_t read_ints_from_stdin(int **out_data);
void   print_int_array(const int *data, size_t n);

int load_previous_rows(const char *filename,
                       int **prev_original, size_t *prev_original_n,
                       int **prev_sorted,   size_t *prev_sorted_n);

int save_rows(const char *filename,
              const int *original, size_t original_n,
              const int *sorted,   size_t sorted_n);

#endif /* NUMBER_IO_H */
