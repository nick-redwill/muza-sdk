#include <iostream>
#include "bass/BassGlobal.h"
#include "bass/BassStream.h"

#include <bass.h>

int main(int argc, char ** argv) {
    BassGlobal bass;
    auto devs = bass.getDevices();

    for (auto dev : devs) {
        std::cout << dev.id << " " << dev.name << std::endl;
    }
    
    int id;
    std::cout << "Enter device: ";
    std::cin >> id;

    int sampleRate, stereo;

    std::cout << "Enter sample rate: ";
    std::cin >> sampleRate;

    std::cout << "Is stereo (1/0): ";
    std::cin >> stereo;

    Config config(devs[id], sampleRate, (bool)stereo);

    if (!bass.init(config)) {
        return 1;
    }
   
    BassStream stream;
    stream.loadFromFile(argv[1]);
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
