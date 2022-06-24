#include <stdio.h>
#include <stdbool.h>
#define HOPE_IMPLEMENTATION
#include "hope.h"

void test_0(char **argv);
void test_1(char **argv);



int main(int argc, char *argv[]) {
    (void) argc;
    test_1(argv);
    return 0;
}

void test_0(char **argv){
    hope_t hope = hope_init(argv[0], "A simple program");
    hope_add_param(&hope, hope_param_init("-h", "Print this help message", HOPE_TYPE_SWITCH, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-v", "Print the version of HOPE", HOPE_TYPE_SWITCH, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-i", "An integer", HOPE_TYPE_INTEGER, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-d", "A double", HOPE_TYPE_DOUBLE, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-s", "A string", HOPE_TYPE_STRING, HOPE_ARGC_OPTMORE));
    hope_add_param(&hope, hope_param_init(NULL, "Other arguments", HOPE_TYPE_STRING, HOPE_ARGC_OPTMORE));
    if(hope_parse(&hope, argv + 1)){
        hope_print_help(&hope);
        exit(1);
    }

    bool help = false;
    bool version = false;
    long int *integers = NULL;
    double *doubles = NULL;
    const char **strings = NULL;
    const char **collected = NULL;
    hope_get_switch(&hope, "-h", &help);
    hope_get_switch(&hope, "-v", &version);
    size_t int_count = hope_get_integer(&hope, "-i", &integers);
    size_t double_count = hope_get_double(&hope, "-d", &doubles);
    size_t string_count = hope_get_string(&hope, "-s", &strings);
    size_t collector_count = hope_get_string(&hope, NULL, &collected);
    if(help) {
        hope_print_help(&hope);
        return;
    }
    if(version){
        printf("HOPE version %s\n", HOPE_VERSION);
        return;
    }
    if(string_count > 0){
        for(size_t i = 0; i < string_count; i++)
            printf("String %zu: %s\n", i, strings[i]);
    }
    if(int_count > 0){
        for(size_t i = 0; i < int_count; i++)
            printf("Integer %zu: %ld\n", i, integers[i]);
    }
    if(double_count > 0){
        for(size_t i = 0; i < double_count; i++)
            printf("Double %zu: %f\n", i, doubles[i]);
    }
    if(collector_count > 0){
        for(size_t i = 0; i < collector_count; i++)
            printf("Collected %zu: %s\n", i, collected[i]);
    }
    
    hope_free(&hope);
}

void test_1(char **argv){
    hope_t hope = hope_init(argv[0], "A simple program");
    hope_add_param(&hope, hope_param_init("-h", "Print this help message", HOPE_TYPE_SWITCH, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-v", "Print the version of HOPE", HOPE_TYPE_SWITCH, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-i", "An integer", HOPE_TYPE_INTEGER, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-d", "A double", HOPE_TYPE_DOUBLE, HOPE_ARGC_OPT));
    hope_add_param(&hope, hope_param_init("-s", "A string", HOPE_TYPE_STRING, HOPE_ARGC_OPT));
    if(hope_parse(&hope, argv + 1)){
        hope_print_help(&hope);
        exit(1);
    }

    bool print_help = hope_get_single_switch(&hope, "-h");
    bool print_version = hope_get_single_switch(&hope, "-v");
    long int passed_int = hope_get_single_integer(&hope, "-i");
    double passed_double = hope_get_single_double(&hope, "-d");
    const char *passed_string = hope_get_single_string(&hope, "-s");

    if(print_help){
        hope_print_help(&hope);
        return;
    }
    if(print_version){
        printf("HOPE Library Version: %s\n", HOPE_VERSION);
        return;
    }
    printf("Passed Integer: %lu\nPassed Double: %f\nPassed String: %s\n", 
            passed_int, 
            passed_double,
            passed_string);
    hope_free(&hope);
}