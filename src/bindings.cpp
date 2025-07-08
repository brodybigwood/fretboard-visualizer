#include <vector>
#include <emscripten.h>
#include "global.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "data.h"
using json = nlohmann::json;

extern "C" {

    float songTime = 0.0f;

    EMSCRIPTEN_KEEPALIVE
    float* getTimePtr() {
        return &songTime;
    }

    EMSCRIPTEN_KEEPALIVE
    void updateNotes(float* newNotes, int count) {
        if (count > 0) std::cout << newNotes[0] << std::endl;
        std::vector<float> nums(newNotes, newNotes + count);
        notes = nums;
    }

    EMSCRIPTEN_KEEPALIVE
    void recieveJSON(const char* jsonStr) {
        try {
            json parsed = json::parse(jsonStr);
            song = parseSong(parsed);
        } catch (const json::parse_error& e) {
            std::cout<<"error "<<e.what()<<std::endl;
        }
    }

}
