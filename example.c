#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define HOPE_IMPLEMENTATION
#include "hope.h"

//void test_0(char **argv);
void test_1(char **argv);

int main(int argc, char *argv[]) {
    (void) argc;
    test_1(argv);
    return 0;
}

void test_1(char **argv){
    hope_t hope = hope_init(argv[0], "Hope Example: A simple program to give an example on the capabilities of Hope");
    hope_set_t help_set = hope_init_set("Help");
    hope_add_param(&help_set, hope_init_param("-h", "Print this help message", HOPE_TYPE_SWITCH, 1));
    
    hope_set_t main_set = hope_init_set("Default");
    hope_add_param(&main_set, hope_init_param("-v", "Print the version of HOPE", HOPE_TYPE_SWITCH, HOPE_ARGC_OPT));
    hope_add_param(&main_set, hope_init_param("-i", "An integer", HOPE_TYPE_INTEGER, HOPE_ARGC_OPT));
    hope_add_param(&main_set, hope_init_param("-d", "A double", HOPE_TYPE_DOUBLE, HOPE_ARGC_OPT));
    hope_add_param(&main_set, hope_init_param("-s", "A string", HOPE_TYPE_STRING, HOPE_ARGC_OPTMORE));
    //hope_add_param(&main_set, hope_init_param(NULL, "Other arguments", HOPE_TYPE_STRING, HOPE_ARGC_OPTMORE));
    
    hope_add_set(&hope, help_set);
    hope_add_set(&hope, main_set);
    
    if(hope_parse_argv(&hope, argv)){
        hope_print_help(&hope, stdout);
        exit(1);
    }

    if(!strcmp(hope.used_set_name, "Help")){
        if(hope_get_single_switch(&hope, "-h")){
            hope_print_help(&hope, stdout);
        }
    } else {
        bool print_version = hope_get_single_switch(&hope, "-v");
        long int passed_int = hope_get_single_integer(&hope, "-i");
        double passed_double = hope_get_single_double(&hope, "-d");
        const char *passed_string = hope_get_single_string(&hope, "-s");

        if(print_version){
            printf("HOPE Library Version: %s\n", HOPE_VERSION);
            return;
        }
        printf("Passed Integer: %lu\nPassed Double: %f\nPassed String: %s\n", 
                passed_int, 
                passed_double,
                passed_string);
        }
    hope_free(&hope);
}