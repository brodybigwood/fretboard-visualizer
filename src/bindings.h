#pragma once

#ifdef __cplusplus
extern "C" {
    #endif
    extern float songTime;

    void updateNotes(float* newNotes, int count);
    void recieveJSON(const char*);

    #ifdef __cplusplus
}
#endif
