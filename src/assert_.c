#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "assert_.h"


void assert_(bool val, char *str) {
    if (!val) {
        printf("Assertion failed.\nError message: %s\n", str);
        assert(val);
    }
}