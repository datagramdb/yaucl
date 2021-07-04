/*
 * evaluateHashes.c
 * This file is part of fuzzyStringMatching
 *
 * Copyright (C) 2018 - Giacomo Bergami
 *
 * fuzzyStringMatching is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * fuzzyStringMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fuzzyStringMatching. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by Giacomo Bergami on 16/08/16.
//

#include <yaucl/hashing/evaluateHashes.h>

unsigned int doHash(char *line, int *pos) {
    unsigned int prime = 17;
    unsigned int result = 3;
    int i = 0;
    int c = 0;
    char* tok;
    for (tok = strtok(line, ",");
         tok && *tok;
         tok = strtok(NULL, ",\n")) {
        if (i++ == pos[c]) {
            result = result * prime + hashCode(tok);
            c++;
        }
    }
    return result;
}

void evaluateHashes(char *file, int *pos) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(file, "r");
    if (fp == NULL)
        exit(1);
    int first = 1;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (first) {
            first = 0;
            printf("Hashes\n");
        } else
            printf("%u\n", doHash(line, pos));
    }

    fclose(fp);
    if (line)
        free(line);
}


unsigned int hashCode(char* string) {
    unsigned int len = strlen(string), h = 0;
    if (h == 0 && len > 0) {
        for (int i = 0; i < len; i++) {
            h = 31 * h + (unsigned int)string[i];
        }
    }
    return h;
}

unsigned int hashCodeArray(char** arrayToHash) {
    unsigned int prime = 17;
    unsigned int result = 3;
    int i = 0;
    char* check = arrayToHash[i++];
    while (check!=NULL) {
        result = result * prime + hashCode(check);
        check = arrayToHash[i++];
    }
    return result;
}
