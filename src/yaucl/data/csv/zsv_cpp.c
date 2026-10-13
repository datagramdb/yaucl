//
// Created by gyankos on 01/01/26.
//


#include <zsv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <yaucl/data/csv/csv.h>

struct local_zsv {
    // void* zsv_opts_data;
    FILE* f;
    void* parser;
};

unsigned char open_csv_parser(const char* filename, struct local_zsv *zsv, char delimiter) {
    if (!zsv) {
        return EXIT_FAILURE;
    }
    zsv->f =  fopen(filename, "rb");
    if (!zsv->f) {
        return EXIT_FAILURE;
    }
    // zsv->zsv_opts_data = (void*)malloc(sizeof(struct zsv_opts));
    // if (!zsv->zsv_opts_data) {
        // return EXIT_FAILURE;
    // }
    // memset(zsv->zsv_opts_data, 0, sizeof(struct zsv_opts));
    struct zsv_opts opts = {0};
    opts.delimiter = delimiter;
    opts.stream = zsv->f;
    zsv->parser = (void*)zsv_new(&opts);
    if (!((struct zsv_opts *)zsv->parser)) {
        fprintf(stderr, "Could not allocate parser!\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

unsigned char has_csv_next(struct local_zsv *zsv) {
    if (!zsv)
        return EXIT_FAILURE;
    size_t cell_count = zsv_cell_count(zsv->parser);
    return (zsv_next_row(zsv->parser) == zsv_status_row) ? EXIT_SUCCESS : EXIT_FAILURE;
}

size_t csv_cells_in_current_row(struct local_zsv *zsv) {
    if (!zsv)
        return 0;
    return zsv_cell_count(zsv->parser);
}

size_t close_csv_parser(struct local_zsv *zsv) {
    if (!zsv)
        return EXIT_FAILURE;
    /**
      * Clean up
      */
    enum zsv_status val = zsv_delete(zsv->parser);
    if ((val != zsv_status_ok) && (val != zsv_status_done))
        return EXIT_FAILURE;
    if (fclose(zsv->f) == EOF)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

size_t ith_csv_cell(struct local_zsv *zsv, size_t idx, unsigned char **str,  size_t* len,  char* quoted,
  unsigned char* overwritten) {
    if (!zsv) {
        *str = NULL;
        *len = 0;
        *quoted = 0;
        *overwritten = 0;
        return EXIT_FAILURE;
    }
    struct zsv_cell c = zsv_get_cell(zsv->parser, idx);
    *str = c.str;
    *len = c.len;
    *overwritten = c.overwritten;
    *quoted = c.quoted;
    return EXIT_SUCCESS;
}


