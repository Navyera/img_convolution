#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "args.h"
#include "utils.h"

void print_usage() {
    printf("Usage : mpiexec -n <n_procs> img_conv -f <filename> -w <width> -h <height> -c <color_type> -l <n_loops>\n");
}

int parse_args(int argc, char *argv[], args_t *ret, int myrank) {
    if (argc != 9)
        return -1;

    int option;
    char *end;

    char w_read = 0;
    char h_read = 0;
    char c_read = 0;
    char l_read = 0;

    while ((option = getopt(argc, argv, "w:h:c:l:")) != -1){
        switch (option){
            case 'w':
                if (w_read)
                    IF_MASTER_ERR(myrank, DUPLICATE_ERR("width"))

                w_read = 1;

                ret->width = (int )strtol(optarg, &end, 10);

                if (ret->width <= 0 || *end != '\0')
                    IF_MASTER_ERR(myrank, {printf("Error : Width must be a positive integer.\n");})

                break;

            case 'h':
                if (h_read)
                    IF_MASTER_ERR(myrank, DUPLICATE_ERR("height"))

                h_read = 1;

                ret->height = (int) strtol(optarg, &end, 10);

                if (ret->height <= 0 || *end != '\0')
                    IF_MASTER_ERR(myrank, {printf("Error : Height must be a positive integer.\n");})

                break;

            case 'c':
                if (c_read)
                    IF_MASTER_ERR(myrank, DUPLICATE_ERR("color_type"))

                c_read = 1;

                if (!strcmp("RGB", optarg))
                    ret->color = RGB;
                else if (!strcmp("GRAY", optarg))
                    ret->color = GRAYSCALE;
                else
                    IF_MASTER_ERR(myrank, {printf("Error : Color type must be one of {RGB, GRAY}.\n");})

                break;

            case 'l':
                if (l_read)
                    IF_MASTER_ERR(myrank, DUPLICATE_ERR("n_loops"))

                l_read = 1;

                ret->loops = (int) strtol(optarg, &end, 10);

                if (ret->loops <= 0 || *end != '\0')
                    IF_MASTER_ERR(myrank, {printf("Error : Number of loops must be a positive integer.\n");})

                break;

            default:
                return -1;
        }
    }

    return 0;
}
