#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

struct Event {
    int type;
    float note;
    int velocity;
    float time;
    int id;
};

struct Instrument {
    std::string name;
    std::vector<Event> events;
};

struct Song {
    std::string name;
    std::vector<Instrument> instruments;
};

inline Event parseEvent(const json& j, float offset) {
    return Event{
        j.value("type", 0),
        j.value("note", 0.0f),
        j.value("velocity", 0),
        j.value("time", 0.0f) - offset,
        j.value("id", -1)
    };
}

inline Instrument parseInstrument(const json& j, float offset) {
    Instrument inst;
    inst.name = j.value("name", "");
    if (j.contains("events") && j["events"].is_array()) {
        for (const auto& ev : j["events"]) {
            inst.events.push_back(parseEvent(ev, offset));
        }
    }
    return inst;
}

inline Song parseSong(const json& j) {
    Song song;
    song.name = j.value("name", "");
    float offset = j.value("offset", 0.0f);

    if (j.contains("instruments") && j["instruments"].is_array()) {
        for (const auto& inst : j["instruments"]) {
            song.instruments.push_back(parseInstrument(inst, offset));
        }
    }
    return song;
}
