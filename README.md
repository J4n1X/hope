# HOPE Library
HOPE stands for **H**eader **O**nly **P**arameter **E**valuator and it is written in pure C.

It's intended use is to make command-line argument parsing as easy as possible.

## Usage
To use this library in your project, simply download the header file "hope.h". Then, to properly include it, do this:

    #define HOPE_IMPLEMENTATION
    #include "hope.h"

---
## Documentation

### Creating a new parser structure

To create a new parser, use:

    hope_t hope_init(const char *prog_name, const char *prog_desc)

Where prog_name is the name of the executable and prog_desc is an optional brief description of the program. Note that you should always call the free function after using the parser. This frees the internal pointers to memory. (If you have allocated memory for the hope_t itself, then you must free it separately)

    void hope_free(hope_t *hope)

In order to add a parameter to the parser, you must first create a parameter set, you do this with:

    hope_set_t hope_init_set(const char *name)

To this set, you can now add parameters. For this, use:
    hope_param_t hope_init_param(const char *name, const char *help, enum hope_argtype_e type, int nargs)

The parameter "help" is optional here.

To select a parameter type, the following enumerators are used:

  - `HOPE_TYPE_SWITCH` - Accepts no arguments, but switches a boolean
  - `HOPE_TYPE_INTEGER` - Accepts 64-bit integer numbers
  - `HOPE_TYPE_DOUBLE` - Accepts double floating point numbers
  - `HOPE_TYPE_STRING` - Accepts strings

To set the quantity of arguments a parameter accepts, either pass a positive number, or use these special values:

  - `HOPE_ARGC_NONE` - Technically unused, forbids you from passing any arguments to the parameter
  - `HOPE_ARGC_MORE` - Accepts one(1) or more arguments (You can end the passing of arguments to the parameter with "--")
  - `HOPE_ARGC_OPTMORE` Accepts zero(0) or more arguments (You can end the passing of arguments to the parameter with "--")
  - `HOPE_ARGC_OPT` Accepts zero(0) or one(1) arguments

Having created a parameter structure, you can then add it to the set by using:

    int hope_add_param(hope_set_t *set, hope_param_t param)

And this set can then be added to the parser with:
    int hope_add_set(hope_t *hope, hope_set_t set)

### Parsing

To parse a list of arguments, use either of these two functions:

    int hope_parse(hope_t *hope, char *args[])
    int hope_parse_argv(hope_t *hope, char *argv[])

In order to find out which parameter set was parsed, you can access the `used_set_name` field in the `hope_t` structure.

### Getting Parameter values

There exist two sets of functions to get values.

The first set of functions can be used to get arguments if more than one was passed or if you want to handle any errors. These getter functions return -1 on error, set dest to NULL and print an error message.

    int hope_get_switch(hope_t *hope, const char *name, bool *dest);
    int hope_get_integer(hope_t *hope, const char *name, long int **dest);
    int hope_get_double(hope_t *hope, const char *name, double **dest);
    hope_get_string(hope_t *hope, const char *name, const char ***dest);

The second set of functions can be used to get single or optional values. These getter functions will terminate the program with an assertion if an error occurs, so use them carefully. If no argument was passed to an optional parameter, then a default value is returned.


Get a single switch or return false if it wasn't set.
    
    bool hope_get_single_switch(hope_t *hope, const char *name);
Get a single integer or return a default value if none were passed.
    
    long int hope_get_single_integer(hope_t *hope, const char *name);
Get a single double or return a default value if none were passed.
    
    double hope_get_single_double(hope_t *hope, const char *name);
Get a single string or return a default value if none were passed.
    
    const char *hope_get_single_string(hope_t *hope, const char *name);

# Other

At any time, you can generate and print a help message to stdout by using:

    void hope_print_help(hope_t *hope, FILE *sink)

The version of the library is stored as a string in the definition `HOPE_VERSION`
