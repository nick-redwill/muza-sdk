#include <iostream>
#include "bass/BassGlobal.h"
#include "bass/BassStream.h"

#include <bass.h>

int main(int argc, char ** argv) {
    BassGlobal bass;

    if (!bass.init()) {
        return 1;
    }

    BassStream stream(BASS_StreamCreateFile(false, argv[1], 0, 0, 0), BassStream::Type::LOCAL_FILE);
    stream.play();

    std::cout << stream.isPlaying() << std::endl;
    std::cout << stream.getSampleRate() << " " << stream.getBitrate() << std::endl;

    std::cin.get();
    // stream.pause();
    // stream.seek(50);
    stream.setPosition(30);

    std::cin.get();

    bass.clean();

    return 0;
}
