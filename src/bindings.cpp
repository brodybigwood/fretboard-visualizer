#include <vector>
#include <emscripten.h>
#include "global.h"
#include <iostream>

extern "C" {

    EMSCRIPTEN_KEEPALIVE
    void updateNotes(float* newNotes, int count) {
        if (count > 0) std::cout << newNotes[0] << std::endl;
        std::vector<float> nums(newNotes, newNotes + count);
        notes = nums;
    }

}
