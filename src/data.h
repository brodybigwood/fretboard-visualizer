#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Event {
    int type;
    float note;
    int velocity;
    float time;
};

struct Instrument {
    std::string name;
    std::vector<Event> events;
};

struct Song {
    std::string name;
    std::string file;
    std::vector<Instrument> instruments;
};

inline Event parseEvent(const json& j) {
    return Event{
        j.value("type", 0),
        j.value("note", 0.0f),
        j.value("velocity", 0),
        j.value("time", 0.0f)
    };
}

inline Instrument parseInstrument(const json& j) {
    Instrument inst;
    inst.name = j.value("name", "");
    if (j.contains("events") && j["events"].is_array()) {
        for (const auto& ev : j["events"]) {
            inst.events.push_back(parseEvent(ev));
        }
    }
    return inst;
}

inline Song parseSong(const json& j) {
    Song song;
    song.name = j.value("name", "");
    song.file = j.value("file", "");
    if (j.contains("instruments") && j["instruments"].is_array()) {
        for (const auto& inst : j["instruments"]) {
            song.instruments.push_back(parseInstrument(inst));
        }
    }
    return song;
}
