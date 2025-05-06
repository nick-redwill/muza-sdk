#include <bass.h>

#include "BassGlobal.h"

bool BassGlobal::init() {
    //FIXME: Use actual parameters, instead of hardcoded values
    return BASS_Init(-1, 44100, 0, 0, 0);
}

bool BassGlobal::clean() {
    return BASS_Free();
}
