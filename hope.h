/* File: hope.h
 * Author: Janick Eicher
 * Date: 21.06.2022
 * Description: Single-Header-Library for argument parsing in C
 * License: MIT
 * Version: 1.0.0
 * 
 * To use this library, define HOPE_IMPLEMENTATION before including this file.
 * This will make this file compile.
 */


#ifndef HOPE_H_
#define HOPE_H_

// Header
#ifndef INCLUDE_HOPE_H_
#define INCLUDE_HOPE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HOPEDEF
#ifdef HOPE_STATIC
#define HOPEDEF static
#else
#define HOPEDEF extern
#endif
#endif

#define HOPE_VERSION "0.1.3"


/* individual types of arguments
 * all arguments will always be put into an array for streamlining
 */
enum hope_argtype_e {
    HOPE_TYPE_SWITCH = 0,
    HOPE_TYPE_INTEGER = 1, // these are long integers
    HOPE_TYPE_DOUBLE = 2,
    HOPE_TYPE_STRING = 3,
};

HOPEDEF const char *hope_argtype_str(enum hope_argtype_e argtype);

// Argument count special values
// Expect none: This is unused
#define HOPE_ARGC_NONE       0
// Expect one or more
#define HOPE_ARGC_MORE      -1
// Expect zero or more
#define HOPE_ARGC_OPTMORE   -2
// Expect zero or one
#define HOPE_ARGC_OPT       -3

/* Parameter struct
 * nargs: number of arguments
 *        (0 for no arguments)
 *        (-1 for 1 or more arguments)
 *        (-2 for 0 or more arguments)
 *        (-3 for an optional argument)
 * name: prefix for the parameter
 *      (NULL for no prefix, called collector, there can only be one of these)
 * help: help message
 * 
 */
typedef struct {
    const char *name;
    const char *help;
    enum hope_argtype_e type;
    int nargs; 
} hope_param_t;

/* Temporary struct for storing arguments parsed
 * First arg is always the prefix (NULL if no prefix)
 */
typedef struct {
    union {
        long int *integers;
        double *doubles;
        const char **strings;
        bool _switch;
    } value;
    const char *name;
    size_t count;
    enum hope_argtype_e type;
} hope_result_t;

/* Main data structure, will contain the parameters and
 * further information about the arguments parsed
 */ 
typedef struct {
    const char *prog_name;
    const char *prog_desc;
    hope_param_t *params;
    hope_param_t *collector;
    hope_result_t *results;
    size_t nparams;
    size_t nresults;
} hope_t;


//
// hope_param_t functions
//

// Initialize the hope_param_t data structure
HOPEDEF hope_param_t hope_param_init(const char *name, const char *help, enum hope_argtype_e type, int nargs);

//
// hope_t functions
// 

// Initialize the hope data structure
HOPEDEF hope_t hope_init(const char *prog_name, const char *prog_desc);
// Free the params in the hope data structure
HOPEDEF void hope_free(hope_t *hope);
// Generate and write the help message to stdout
HOPEDEF void hope_print_help(hope_t *hope); 
// Add a new parameter to the hope data structure
HOPEDEF int hope_add_param(hope_t *hope, hope_param_t param);
// Parse the command line arguments
HOPEDEF int hope_parse(hope_t *hope, char *argv[]);

// All these getter functions return -1 on error, and print an error message to stderr
// Dest pointers will also be set to NULL on error

// Get the value of a switch parameter (Function format is same as others to keep uniformity,
// despite the fact that only one value is returned for switches)
HOPEDEF int hope_get_switch(hope_t *hope, const char *name, bool *dest);
// Get the values of an integer parameter
HOPEDEF int hope_get_integer(hope_t *hope, const char *name, long int **dest);
// Get the values of a double parameter
HOPEDEF int hope_get_double(hope_t *hope, const char *name, double **dest);
// Get the values of a string parameter
HOPEDEF int hope_get_string(hope_t *hope, const char *name, const char ***dest);

// All these getter functions will terminate the program with an assertion if an error occurs,
// so use them carefully.

// Get a single switch or return false if it wasn't set.
HOPEDEF bool hope_get_single_switch(hope_t *hope, const char *name);

// Get a single integer or return a default value if none were passed.
HOPEDEF long int hope_get_single_integer(hope_t *hope, const char *name);

// Get a single double or return a default value if none were passed.
HOPEDEF double hope_get_single_double(hope_t *hope, const char *name);

// Get a single string or return a default value if none were passed.
HOPEDEF const char *hope_get_single_string(hope_t *hope, const char *name);

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_HOPE_H_
// End of Header

//#define HOPE_IMPLEMENTATION
#ifdef HOPE_IMPLEMENTATION

#include <stdio.h>
#include <assert.h>
#include <string.h>

// Get the string representation of an argument type
HOPEDEF const char *hope_argtype_str(enum hope_argtype_e argtype) {
    switch (argtype) {
        case HOPE_TYPE_SWITCH:
            return "switch";
        case HOPE_TYPE_INTEGER:
            return "integer";
        case HOPE_TYPE_DOUBLE:
            return "double";
        case HOPE_TYPE_STRING:
            return "string";
        default:
            return "unknown";
    }
}

// generic format string

#define HOPE_SUCCESS_CODE 0x00
#define HOPE_FMT_DEFAULT "hope: %s; %s"

// Error codes for system failures
#define HOPE_ERR_CODE 0x10
#define HOPE_ERR_ALLOC_FAILED_CODE 0x11
#define HOPE_ERR_ALLOC_FAILED_MSG "Allocation of memory failed"
#define HOPE_ERR_INVALID_STRUCT_CODE 0x12
#define HOPE_ERR_INVALID_STRUCT_MSG "Invalid hope structure passed"

void hope_err_alloc(const char *msg) {
    fprintf(stderr, HOPE_FMT_DEFAULT "\n", msg, HOPE_ERR_ALLOC_FAILED_MSG);
}

void hope_err_invalid_struct(const char *msg) {
    fprintf(stderr, HOPE_FMT_DEFAULT "\n", msg, HOPE_ERR_INVALID_STRUCT_MSG);
}

// Error codes and messages for adding parameters
#define HOPE_PARAMADD_ERR_CODE 0x20
#define HOPE_PARAMADD_ERR_GENERIC_MSG "Error adding parameter"
#define HOPE_PARAMADD_ERR_HASCOLLECTOR_CODE 0x21
#define HOPE_PARAMADD_ERR_HASCOLLECTOR_MSG "Collector already exists"
#define HOPE_PARAMADD_ERR_DUPLICATE_CODE 0x22
#define HOPE_PARAMADD_ERR_DUPLICATE_MSG "Duplicate parameter name"

void hope_paramadd_err_hascollector(){
    fprintf(stderr, HOPE_FMT_DEFAULT "\n", 
            HOPE_PARAMADD_ERR_GENERIC_MSG, 
            HOPE_PARAMADD_ERR_HASCOLLECTOR_MSG);
}

void hope_paramadd_err_duplicate(const char *name) {
    fprintf(stderr, HOPE_FMT_DEFAULT ": %s\n", 
            HOPE_PARAMADD_ERR_GENERIC_MSG, 
            HOPE_PARAMADD_ERR_DUPLICATE_MSG,
            name);
}

// Error codes and messages for parsing arguments
#define HOPE_PARSE_ERR_CODE 0x30
#define HOPE_PARSE_ERR_GENERIC_MSG "Error parsing arguments"
#define HOPE_PARSE_ERR_PARAM_MISCOUNT_CODE 0x31
#define HOPE_PARSE_ERR_PARAM_MISCOUNT_MSG "Invalid amount of parameters provided"
#define HOPE_PARSE_ERR_PARAM_UNPARSABLE_CODE 0x32
#define HOPE_PARSE_ERR_PARAM_UNPARSABLE_MSG "Parameter could not be parsed"
#define HOPE_PARSE_ERR_PARAM_ARG_MISCOUNT_CODE 0x33
#define HOPE_PARSE_ERR_PARAM_ARG_MISCOUNT_MSG "Invalid amount of arguments passed for parameter"

void hope_parse_err_param_miscount() {
    fprintf(stderr, HOPE_FMT_DEFAULT "\n",
            HOPE_PARSE_ERR_GENERIC_MSG,
            HOPE_PARSE_ERR_PARAM_MISCOUNT_MSG);
}

void hope_parse_err_param_unparsable(const char *name) {
    fprintf(stderr, HOPE_FMT_DEFAULT ": %s\n",
            HOPE_PARSE_ERR_GENERIC_MSG,
            HOPE_PARSE_ERR_PARAM_UNPARSABLE_MSG,
            name);
}

void hope_parse_err_param_arg_miscount(const char *name) {
    fprintf(stderr, HOPE_FMT_DEFAULT " %s\n",
                HOPE_PARSE_ERR_GENERIC_MSG,
                HOPE_PARSE_ERR_PARAM_ARG_MISCOUNT_MSG,
                name);
}


// Error messages for types
#define HOPE_TYPED_ERR_SWITCH_MSG "Argument is not a switch, but a"
#define HOPE_TYPED_ERR_INTEGER_MSG "Argument is not an integer, but a"
#define HOPE_TYPED_ERR_DOUBLE_MSG "Argument is not a double, but a"
#define HOPE_TYPED_ERR_STRING_MSG "Argument is not a string, but a"


// Error messages and codes for getter
#define HOPE_GET_ERR_CODE 0x40
#define HOPE_GET_ERR_GENERIC_MSG "Failed to get parameter"
#define HOPE_GET_ERR_NOEXIST_CODE 0x41
#define HOPE_GET_ERR_NOEXIST_MSG "Parameter does not exist"
#define HOPE_GET_ERR_TYPE_MISMATCH_CODE 0x42
#define HOPE_GET_ERR_TYPE_MISMATCH_MSG "Type mismatch for parameter"

void hope_get_err_noexist(const char *name){
    fprintf(stderr, HOPE_FMT_DEFAULT ": %s\n",
            HOPE_GET_ERR_GENERIC_MSG,
            HOPE_GET_ERR_NOEXIST_MSG,
            name);
}

void hope_get_err_type(const char *name, enum hope_argtype_e type) {
    fprintf(stderr, HOPE_FMT_DEFAULT " %s: %s %s\n",
            HOPE_GET_ERR_GENERIC_MSG,
            HOPE_GET_ERR_TYPE_MISMATCH_MSG,
            name,
            HOPE_TYPED_ERR_SWITCH_MSG,
            hope_argtype_str(type));
}

//
// hope_param_t functions
//

// Initialize the hope data structure
HOPEDEF hope_param_t hope_param_init(const char *name, const char *help, enum hope_argtype_e type, int nargs){
    hope_param_t param = {
        .name = name,
        .help = help,
        .type = type,
        .nargs = nargs,
    };
    return param;
}

// Free the hope_param_t data structure
//HOPEDEF void hope_param_free(hope_param_t *param){
//    free((hope_param_t*) param);
//}

//
// hope_t functions
// 

// Initialize the hope data structure
HOPEDEF hope_t hope_init(const char *prog_name, const char *prog_desc){
    assert(prog_name && "prog_name cannot be NULL");
    hope_t hope = {
        .prog_name = prog_name,
        .prog_desc = prog_desc,
        .params = NULL,
        .collector = NULL,
        .results = NULL,
        .nparams = 0,
        .nresults = 0,
    };
    return hope;
}
// Free the params in the hope data structure
HOPEDEF void hope_free(hope_t *hope){
    if (hope->params) free(hope->params);
    if (hope->collector) free(hope->collector);
    if (hope->results){
        for (size_t i = 0; i < hope->nresults; i++){
            if(hope->results[i].type != HOPE_TYPE_SWITCH)
                free(hope->results[i].value.strings);
        }
        free(hope->results);
    }
    hope->nparams = 0;
}

// Generate and write the help message to stdout
HOPEDEF void hope_print_help(hope_t *hope){
    const char *FMT_SWITCH_REQ = "%s ";
    const char *FMT_SWITCH_OPT = "(%s) ";
    const char *FMT_PARAM_REQ = "%s [%s] ";
    const char *FMT_PARAM_OPT = "(%s [%s])? ";
    const char *FMT_PARAM_OPTMORE = "(%s [%s]*) ";
    const char *FMT_PARAM_MORE = "%s [%s]+ ";
    const char *FMT_PARAM_REQ_SPECIFIC = "%s [%s]{%u} ";
    
    if(hope->prog_desc)
        fprintf(stdout, "%s\n", hope->prog_desc);
    fprintf(stdout, "Usage: %s ", hope->prog_name);

    char *fmt_string = NULL;
    for(size_t i = 0; i < hope->nparams; i++){
        hope_param_t *cur_param = hope->params + i;
        if(cur_param->type == HOPE_TYPE_SWITCH){
            fmt_string = (char*)(cur_param->nargs == HOPE_ARGC_OPT ? FMT_SWITCH_OPT : FMT_SWITCH_REQ);
            printf(fmt_string, cur_param->name);
        } else {
            switch(cur_param->nargs){
                case HOPE_ARGC_MORE:
                    fmt_string = (char*)FMT_PARAM_MORE;
                    break;
                case HOPE_ARGC_OPTMORE:
                    fmt_string = (char*)FMT_PARAM_OPTMORE;
                    break;
                case HOPE_ARGC_OPT:
                    fmt_string = (char*)FMT_PARAM_OPT;
                    break;
                default:
                    assert(cur_param->nargs > 0);
                    fmt_string = (char*)(cur_param->nargs > 1 ? FMT_PARAM_REQ_SPECIFIC : FMT_PARAM_REQ);
                    break;
            }
            if(cur_param-> nargs > 1)
                printf(fmt_string, cur_param->name, hope_argtype_str(cur_param->type), cur_param->nargs);  
            else
                printf(fmt_string, cur_param->name, hope_argtype_str(cur_param->type));

        }
    }
    fprintf(stdout, "\n");
    for(size_t i = 0; i < hope->nparams; i++){
        hope_param_t *cur_param = hope->params + i;
        if(cur_param->help)
            fprintf(stdout, "  %s: %s\n", cur_param->name, cur_param->help);
    }
}
// Add a new parameter to the hope data structure
HOPEDEF int hope_add_param(hope_t *hope, hope_param_t param){
    if(param.name == NULL){
        if(hope->collector != NULL){
            hope_paramadd_err_hascollector();
            return HOPE_PARAMADD_ERR_HASCOLLECTOR_CODE;
        }
        hope->collector = (hope_param_t*) malloc(sizeof(hope_param_t));
        if(!hope->collector){
            hope_err_alloc(HOPE_PARAMADD_ERR_GENERIC_MSG);
            return HOPE_ERR_ALLOC_FAILED_CODE;
        }
        *hope->collector = param;
    } else {
        // search for a parameter with the same name
        for(size_t i = 0; i < hope->nparams; i++){
            hope_param_t *cur_param = hope->params + i;
            if(strcmp(cur_param->name, param.name) == 0){
                hope_paramadd_err_duplicate(param.name);
                return HOPE_PARAMADD_ERR_DUPLICATE_CODE;
            }
        }
        hope->params = (hope_param_t*) realloc(hope->params, (hope->nparams + 1) * sizeof(hope_param_t));
        if(!hope->params){
            hope_err_alloc(HOPE_PARAMADD_ERR_GENERIC_MSG);
            return HOPE_ERR_ALLOC_FAILED_CODE;
        }
        hope->params[hope->nparams] = param;
        hope->nparams++;
    }
    return HOPE_SUCCESS_CODE;
}

// Search for the parameter with the given name
hope_param_t *hope_search_param(hope_t *hope, const char *name){
    for(size_t i = 0; i < hope->nparams; i++){
        hope_param_t *cur_param = hope->params + i;
        if(name == NULL){
            if(cur_param->name == NULL)
                return cur_param;
        } else if(strcmp(cur_param->name, name) == 0){
            return cur_param;
        }
    }
    return NULL;
}

// Search for the result with the given name
hope_result_t *hope_search_result(hope_t *hope, const char *name){
    for(size_t i = 0; i < hope->nresults; i++){
        hope_result_t *cur_result = hope->results + i;
        if(name == NULL){
            if(cur_result->name == NULL){
                return cur_result;
            }
        } else if(strcmp(cur_result->name, name) == 0){
            return cur_result;
        }
    }
    return NULL;
}

// parse an integer, allocate memory for the new item and push it to the result
int hope_parse_integer_into_result(const char *str, hope_result_t *result){
    char *endptr;
    long int next_val = strtol(str, &endptr, 10);
    if(endptr == str){
        hope_parse_err_param_unparsable(result->name);
        return HOPE_PARSE_ERR_PARAM_UNPARSABLE_CODE;
    }
    result->value.integers = (long int*) realloc(result->value.integers, (result->count + 1) * sizeof(long int));
    if(!result->value.integers){
        hope_err_alloc(HOPE_PARSE_ERR_GENERIC_MSG);
        return HOPE_ERR_ALLOC_FAILED_CODE;
    }
    result->value.integers[result->count] = next_val;
    result->count++;
    return HOPE_SUCCESS_CODE;
}

// parse a double, allocate memory for the new item and push it to the result
int hope_parse_double_into_result(const char *str, hope_result_t *result){
    char *endptr;
    double next_val = strtod(str, &endptr);
    if(endptr == str){
        hope_parse_err_param_unparsable(result->name);
        return HOPE_PARSE_ERR_PARAM_UNPARSABLE_CODE;
    }
    result->value.doubles = (double*) realloc(result->value.doubles, (result->count + 1) * sizeof(double));
    if(!result->value.doubles){
        hope_err_alloc(HOPE_PARSE_ERR_GENERIC_MSG);
        return HOPE_ERR_ALLOC_FAILED_CODE;
    }
    result->value.doubles[result->count] = next_val;
    result->count++;
    return HOPE_SUCCESS_CODE;
}

// allocate memory for the new string and push it to the result
int hope_parse_string_into_result(const char *str, hope_result_t *result){
    result->value.strings = (const char**) realloc(result->value.strings, (result->count + 1) * sizeof(char*));
    if(!result->value.strings){
        hope_err_alloc(HOPE_PARSE_ERR_GENERIC_MSG);
        return HOPE_ERR_ALLOC_FAILED_CODE;
    }
    result->value.strings[result->count] = str;
    result->count++;
    return HOPE_SUCCESS_CODE;
}

// evaluate the required parsing method based on the parameter type, then parse and push to the result
int hope_parse_into_result(const char *str, hope_param_t *param, hope_result_t *result){
    switch(param->type){
        case HOPE_TYPE_INTEGER:
            return hope_parse_integer_into_result(str, result);
        case HOPE_TYPE_DOUBLE: {
            return hope_parse_double_into_result(str, result);
        }
        case HOPE_TYPE_STRING:{
            return hope_parse_string_into_result(str, result);
        }
        default:
            printf("Unknown type %d\n", param->type);
            assert(0 && "Unreachable"); // TODO: error msg for invalid type
    }
}

int hope_push_parsed_result(hope_t *hope, hope_result_t result){
    hope->results = (hope_result_t*) realloc(hope->results, (hope->nresults + 1) * sizeof(hope_result_t));
    if(!hope->results){ 
        hope_err_alloc(HOPE_PARSE_ERR_GENERIC_MSG);
        return HOPE_ERR_ALLOC_FAILED_CODE;
    }   
    hope->results[hope->nresults] = result;
    hope->nresults++;
    return HOPE_SUCCESS_CODE;
}

// Parse the command line arguments and store the results in the hope data structure
HOPEDEF int hope_parse(hope_t *hope, char *argv[]){
    // look for the collector first
    hope_result_t result = {0};
    int parse_code = 0;
    hope_result_t collector_result = {0};
    hope_param_t *param = NULL;

    if(argv[0] == NULL){
        if(hope->nparams > 0) {
            // check if there are any required parameters
            for(size_t i = 0; i < hope->nparams; i++){
                hope_param_t *cur_param = hope->params + i;
                if(cur_param->nargs >= HOPE_ARGC_MORE && cur_param){
                    hope_parse_err_param_miscount();
                    return HOPE_PARSE_ERR_PARAM_MISCOUNT_CODE;
                }
            }
        } else if(hope->collector){
            if(hope->collector->nargs >= HOPE_ARGC_MORE && hope->collector->nargs != 0){ 
                hope_parse_err_param_miscount();
                return HOPE_PARSE_ERR_PARAM_MISCOUNT_CODE;
            }
        } 
    }
 
    for(size_t i = 0; argv[i] != NULL; i++){
        if(strcmp(argv[i], "--") == 0)
            continue; // skip the -- separator
        param = hope_search_param(hope, argv[i]);
        if(param){
            result.name = param->name;
            result.type = param->type;
            if(param->type == HOPE_TYPE_SWITCH){
                result.value._switch = 1;
            } else if(param->nargs == HOPE_ARGC_NONE){
                continue;
            } else {
                while(argv[i+1] != NULL){
                    if(hope_search_param(hope, argv[i+1]) || strcmp(argv[i+1], "--") == 0)
                        break;

                    parse_code = hope_parse_into_result(argv[i+1], param, &result);
                    if(parse_code != HOPE_SUCCESS_CODE)
                        return parse_code;
                    i++;
                    if(param->nargs == HOPE_ARGC_OPT || param->nargs == (int)result.count)
                        break;
                }
            }
            hope_push_parsed_result(hope, result);
            result = (hope_result_t){0};
        } else {
            if (hope->collector){
                if((hope->collector->nargs == HOPE_ARGC_OPT && collector_result.count != 0) ||
                    hope->collector->nargs == (int)collector_result.count){
                    break;
                }
                parse_code = hope_parse_into_result(argv[i], hope->collector, &collector_result);
                if(parse_code != HOPE_SUCCESS_CODE)
                    return parse_code;
            } else {
                hope_parse_err_param_miscount();
                return HOPE_PARSE_ERR_PARAM_MISCOUNT_CODE;
            }
        }
        if(argv[i] == NULL) 
            break;
    }
    // add empty entries for optional params, or error out if not enough arguments were provided earlier
    for(size_t i = 0; i < hope->nparams; i++){
        param = hope->params + i;
        hope_result_t *param_result = hope_search_result(hope, param->name);
        if(param->nargs == HOPE_ARGC_MORE || param->nargs > HOPE_ARGC_NONE){
            if(param_result == NULL){
                hope_parse_err_param_arg_miscount(param->name);
                return HOPE_PARSE_ERR_PARAM_ARG_MISCOUNT_CODE;
            }
        } else if((param->nargs == HOPE_ARGC_OPTMORE || 
                   param->nargs == HOPE_ARGC_OPT || 
                   param->nargs == HOPE_ARGC_NONE) && 
                  param_result == NULL){
            result = (hope_result_t){
                .name = param->name, 
                .type = param->type,
                .count = 0,
                .value = {0}
            };
            hope_push_parsed_result(hope, result);
        }
    }
    if(hope->collector){
        collector_result.type = hope->collector->type;
        if ((hope->collector->nargs == HOPE_ARGC_MORE && collector_result.count <= 0) ||
            (hope->collector->nargs > HOPE_ARGC_NONE && hope->collector->nargs != (int)collector_result.count)){
            hope_parse_err_param_arg_miscount(hope->collector->name);
            return HOPE_PARSE_ERR_PARAM_ARG_MISCOUNT_CODE;
        }
        hope_push_parsed_result(hope, collector_result);
    }
    return HOPE_SUCCESS_CODE;
}

HOPEDEF int hope_get_switch(hope_t *hope, const char *name, bool *dest){
    hope_result_t *result = hope_search_result(hope, name);
    if(!result){
        hope_get_err_noexist(name);
		dest = NULL;
		return -1;
    }
    if(result->type != HOPE_TYPE_SWITCH){
        hope_get_err_type(name, HOPE_TYPE_SWITCH);
		dest = NULL;
		return -1;
    }
    *dest = result->value._switch;
    return 1;
}


HOPEDEF int hope_get_integer(hope_t *hope, const char *name, long int **dest){
    hope_result_t *result = hope_search_result(hope, name);
    if(!result){
        hope_get_err_noexist(name);
        dest = NULL;
		return -1;
    }
    if(result->type != HOPE_TYPE_INTEGER){
        hope_get_err_type(name, HOPE_TYPE_INTEGER);
        dest = NULL;
		return -1;
    }
    *dest = result->value.integers;
    return result->count;
}

HOPEDEF int hope_get_double(hope_t *hope, const char *name, double **dest){
    hope_result_t *result = hope_search_result(hope, name);
    if(!result){
        hope_get_err_noexist(name);
        dest = NULL;
		return -1;
    }
    if(result->type != HOPE_TYPE_DOUBLE){
        hope_get_err_type(name, HOPE_TYPE_DOUBLE);
        dest = NULL;
		return -1;
    }
    *dest = result->value.doubles;
    return result->count;
}

HOPEDEF int hope_get_string(hope_t *hope, const char *name, const char ***dest){
    hope_result_t *result = hope_search_result(hope, name);
    if(!result){
        hope_get_err_noexist(name);
        dest = NULL;
		return -1;
    }
    if(result->type != HOPE_TYPE_STRING){
        hope_get_err_type(name, HOPE_TYPE_STRING);
        dest = NULL;
		return -1;
    }
    *dest = result->value.strings;
    return result->count;
}

// Get a single switch or return false if it wasn't set.
HOPEDEF bool hope_get_single_switch(hope_t *hope, const char *name){
    hope_result_t *result = hope_search_result(hope, name);
    assert(result && "Queried parameter could not be found.");
    assert(result->type == HOPE_TYPE_SWITCH && "Queried parameter is not of switch type");
    assert(result->count < 2 && "Queried parameter contained more than 1 value.");
    return result->value._switch;
}

// Get a single integer or return a default value if none were passed.
HOPEDEF long int hope_get_single_integer(hope_t *hope, const char *name){
    hope_result_t *result = hope_search_result(hope, name);
    assert(result && "Queried parameter could not be found.");
    assert(result->type == HOPE_TYPE_INTEGER && "Queried parameter is not of integer type");
    assert(result->count < 2 && "Queried parameter contained more than 1 value.");
    return result->count ? result->value.integers[0] : 0;
}

// Get a single double or return a default value if none were passed.
HOPEDEF double hope_get_single_double(hope_t *hope, const char *name){
    hope_result_t *result = hope_search_result(hope, name);
    assert(result && "Queried parameter could not be found.");
    assert(result->type == HOPE_TYPE_DOUBLE && "Queried parameter is not of double type");
    assert(result->count < 2 && "Queried parameter contained more than 1 value.");
    return result->count ? result->value.doubles[0] : 0.0;
}

// Get a single string or return a default value if none were passed.
HOPEDEF const char *hope_get_single_string(hope_t *hope, const char *name){
    hope_result_t *result = hope_search_result(hope, name);
    assert(result && "Queried parameter could not be found.");
    assert(result->type == HOPE_TYPE_STRING && "Queried parameter is not of string type");
    assert(result->count < 2 && "Queried parameter contained more than 1 value.");
    return result->count ? result->value.strings[0] : NULL;
}
#endif // HOPE_IMPLEMENTATION
#endif // HOPE_H_
