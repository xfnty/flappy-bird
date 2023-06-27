#include <raylib.h>
#include <raymath.h>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <system/logging.h>


int main(int argc, char const *argv[])
{
    assert(logging_init());
    logging_shutdown();
    return 0;
}
