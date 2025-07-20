#include <stdbool.h>
#include <argp.h>

struct command_options {
    char * path;
    int width;
    int height;
    int size_set;
} command_options;

static int parse_opt(int key, char *arg, struct argp_state *state) {
    switch(key) {
        case 'd': { 
            char *sep = strchr(arg, ':');
            *sep = '\0';
            command_options.width = atoi(arg);
            command_options.height = atoi(sep + 1);
            command_options.size_set = true;
        }
        break;
        case ARGP_KEY_ARG: {
            if (command_options.path != NULL) {
                argp_failure(state, 1, 0, "too many files");
            } else {
                command_options.path = arg;
            }
        }
        break;
        case ARGP_KEY_END: {
            if (command_options.path == NULL) {
                argp_failure(state, 1, 0, "no file specified");
            }
        }
    }
    return 0;
}

struct argp_option options[] = {
    { "dim", 'd', "width:height", 0, "Specify canvas size"},
    { "size", 's', "width:height", OPTION_ALIAS},
    { 0 }
};

int process_args(int argc, char ** argv) {
    struct argp argp = { options, parse_opt, "filename" };

    if(argp_parse(&argp, argc, argv, 0, 0, 0)) {
        return -1;
    }
}