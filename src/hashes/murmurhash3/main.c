#include "murmurhash3.h"
#include "../bitops.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const uint32_t SEED = 12345;

int main(int argc, char *argv[]) {

    uint32_t output;
    murmurhash3_32(argv[1], strlen(argv[1]), SEED, &output);

    printf("%u", output);

    return 0;
}
