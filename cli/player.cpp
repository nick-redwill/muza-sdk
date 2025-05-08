#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>

#include "../backends/bass/BassGlobal.h"
#include "../backends/bass/BassStream.h"
#include "../entities/config.h"

#include "controls.h"

void printUsage(const char* prog) {
    std::cout << "Usage:\n"
            << prog 
	        << " <file> [--freq N] [--mono] [--url]\n";
}


std::unordered_map<std::string, std::string> parseArgs(int argc, char** argv, std::string& filename) {
    std::unordered_map<std::string, std::string> args;
    if (argc < 2) return args;

    filename = argv[1];
    for (int i = 2; i < argc; ++i) {
        std::string key = argv[i];
        std::string val = "";

        if (i < argc - 1)
            val = argv[i + 1];

        if (key.find("--") == 0 && val.find("--") != 0) {
            args[key] = val;
            ++i;
        }
        else if (key.find("--") == 0) {
            args[key] = "";
        }
    }
    return args;
}

int main(int argc, char** argv) {
    Controls::init();

    std::string path;
    auto args = parseArgs(argc, argv, path);

    if (path.empty()) {
        printUsage(argv[0]);
        Controls::close();
        return 1;
    }

    int sampleRate = std::stoi(args.count("--freq") ? args["--freq"] : "44100");
    bool stereo = args.count("--mono");
    bool url = args.count("--url");

    BassGlobal bass;
    Device device = bass.getDefaultDevice();

    Config config(device, sampleRate, stereo);
    if (!bass.init(config)) {
        std::cerr << "Failed to initialize BASS.\n";
        Controls::close();
        return 1;
        }
        
    BassStream stream;
    try {
        if (url) stream.loadFromUrl(path);
    	else stream.loadFromFile(path);

    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to load file: " << path << "\n"
		  << "Error: " << e.what() << std::endl;
        bass.clean();
        Controls::close();
    	return 1;
    }

    stream.play();
    
    std::cout << "Playing: " << path << "\n";
    std::cout << "Sample rate: " << stream.getSampleRate()
              << ", Bitrate: " << stream.getBitrate() << "\n";

    
    char c;
    while (!stream.isFinished()) {
        std::cout << stream.getPosition() << "/" 
                  << stream.getLength() << " "
                  << stream.isPlaying() << " "
                  << stream.isStopped() << " "
                  << stream.isFinished() << std::endl;

        c = Controls::getch();
        if (c == 27) break;

        if (c == ' ') stream.isPlaying() ? stream.pause() : stream.play();
        //TODO: Change this keys to arrows
        if (c == '+') stream.seek(5);
        if (c == '-') stream.seek(-5);
    }

    std::cout << "Closing..." << std::endl;
    
    bass.clean();
    Controls::close();
    return 0;
}

