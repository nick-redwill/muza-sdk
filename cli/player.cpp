#include <iostream>
#include <string>
#include <unordered_map>

#include "../bass/BassGlobal.h"
#include "../bass/BassStream.h"
#include "../entities/config.h"

void printUsage(const char* prog) {
    std::cout << "Usage:\n"
              << prog 
	      << " <file> [--freq N] [--mono]\n";
}

std::unordered_map<std::string, std::string> parseArgs(int argc, char** argv, std::string& filename) {
    std::unordered_map<std::string, std::string> args;
    if (argc < 2) return args;

    filename = argv[1];
    for (int i = 2; i < argc - 1; ++i) {
        std::string key = argv[i];
        std::string val = argv[i + 1];
        if (key.find("--") == 0) {
            args[key] = val;
            ++i;
        }
    }
    return args;
}

int main(int argc, char** argv) {
    std::string filepath;
    auto args = parseArgs(argc, argv, filepath);

    if (filepath.empty()) {
        printUsage(argv[0]);
        return 1;
    }

    int sampleRate = std::stoi(args.count("--freq") ? args["--freq"] : "44100");
    bool stereo = args.count("--mono") ? false : true;

    BassGlobal bass;
    Device device = bass.getDefaultDevice();

    Config config(device, sampleRate, stereo);
    if (!bass.init(config)) {
        std::cerr << "Failed to initialize BASS.\n";
        return 1;
    }

    BassStream stream;
    try {
    	stream.loadFromFile(filepath);     
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to load file: " << filepath << "\n"
		  << "Error: " << e.what() << std::endl;
        bass.clean();
	return 1;
    }

    stream.play();

    std::cout << "Playing: " << filepath << "\n";
    std::cout << "Sample rate: " << stream.getSampleRate()
              << ", Bitrate: " << stream.getBitrate() << "\n";

    std::cin.get();

    std::cout << "Closing..." << std::endl;

    bass.clean();
    return 0;
}

