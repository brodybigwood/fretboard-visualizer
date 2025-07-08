#include <vector>
#include <emscripten.h>
#include "global.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
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
        json parsed = json::parse(jsonStr);

        song.name = parsed.value("name", "");
        song.file = parsed.value("file", "");

        song.instruments.clear();

        if (parsed.contains("instruments") && parsed["instruments"].is_array()) {
            for (const auto& inst : parsed["instruments"]) {
                Instrument instrument;
                instrument.name = inst.value("name", "");

                if (inst.contains("events") && inst["events"].is_array()) {
                    for (const auto& ev : inst["events"]) {
                        Event event;
                        event.type = ev.value("type", 0);
                        event.note = ev.value("note", 0.0f);
                        event.velocity = ev.value("velocity", 0);
                        event.time = ev.value("time", 0.0f);
                        instrument.events.push_back(event);
                    }
                }
                song.instruments.push_back(instrument);
            }
        }
    }

}
