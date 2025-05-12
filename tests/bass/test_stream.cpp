#include <gtest/gtest.h>

#include "../entities/config.h"
#include "../backends/bass/BassGlobal.h"
#include "../backends/bass/BassStream.h"

#include <chrono>
#include <thread>
#include <math.h>
#include <string>

const std::string filePath = "./track.mp3";
const std::string fileUrl = "https://files.freemusicarchive.org/storage-freemusicarchive-org/tracks/gWykWiyFIQwOsAHlAKIDpFoP0bFz3sH38oqfytFF.mp3";
const std::string liveUrl = "https://s2.mexside.net/8014/stream";

class BassStreamTest : public ::testing::Test {
protected:
    BassGlobal bass;

    void SetUp() override {
        auto device = bass.getDefaultDevice();
        bass.init(Config(device, 44100, true));
    }

    void TearDown() override {
        bass.clean();
    }
};

TEST_F(BassStreamTest, LoadLocalFileTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    EXPECT_EQ(stream.play(), true);
    EXPECT_EQ(stream.seek(1), true);
    EXPECT_EQ(stream.pause(), true);
    EXPECT_EQ(stream.stop(), true);

    EXPECT_THROW(BassStream("nonexistant.mp3", BassStream::Type::LOCAL_FILE), std::runtime_error);
}

TEST_F(BassStreamTest, LoadRemoteTest) {
    BassStream stream(fileUrl, BassStream::Type::REMOTE_FILE);
    stream.setVolume(0);

    EXPECT_EQ(stream.play(), true);

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    EXPECT_EQ(stream.seek(1), true);
    EXPECT_EQ(stream.pause(), true);
    EXPECT_EQ(stream.stop(), true);
}

TEST_F(BassStreamTest, LoadLiveTest) {
    BassStream stream(liveUrl, BassStream::Type::LIVE);
    stream.setVolume(0);

    EXPECT_EQ(stream.play(), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    
    EXPECT_EQ(stream.seek(-1), false);
    EXPECT_EQ(stream.pause(), true);
    EXPECT_EQ(stream.stop(), true);
}

TEST_F(BassStreamTest, CleanupTest) {
    BassStream* stream = new BassStream(filePath, BassStream::Type::LOCAL_FILE);
    EXPECT_EQ(stream->play(), true);

    HSTREAM raw = stream->getRawStream();

    delete stream;

    // Handle error - stream has been removed
    BASS_ChannelPlay(raw, 0);
    EXPECT_EQ(BASS_ErrorGetCode(), BASS_ERROR_HANDLE);
}

TEST_F(BassStreamTest, IsSupportedTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    EXPECT_EQ(stream.isSupported(BassStream::Functionality::LOAD_MEMORY), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::LOAD_FILE), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::LOAD_URL), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::LOAD_LIVE), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::READ), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::PAUSE), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::STOP), true);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::WRITE), false);
    EXPECT_EQ(stream.isSupported(BassStream::Functionality::SET_POSITION), true);

    BassStream stream2(fileUrl, BassStream::Type::REMOTE_FILE);
    EXPECT_EQ(stream2.isSupported(BassStream::Functionality::WRITE), false);
    EXPECT_EQ(stream2.isSupported(BassStream::Functionality::SET_POSITION), true);

    BassStream stream3(liveUrl, BassStream::Type::LIVE);
    EXPECT_EQ(stream3.isSupported(BassStream::Functionality::WRITE), false);
    EXPECT_EQ(stream3.isSupported(BassStream::Functionality::SET_POSITION), false);

    BassStream stream4(44100, 2);
    EXPECT_EQ(stream4.isSupported(BassStream::Functionality::WRITE), true);
    EXPECT_EQ(stream4.isSupported(BassStream::Functionality::SET_POSITION), true);
}

TEST_F(BassStreamTest, StreamRWTest) {
    //NOTE: Setting playback buffer size to 1 second
    BASS_SetConfig(
        BASS_CONFIG_BUFFER,
        1000
    );

    int sr = 44100; // 1 second
    BassStream stream(sr, 1);
    
    std::vector<float> in(sr);
    float freq = 500.f;

    // Generating 500hz sin wave 
    for (int i = 0; i < sr; i++) {
        in.at(i) = 0.5f * sin(2 * M_PI * freq * (float)i / sr);
    }

    stream.write(in);
    auto out = stream.read(sr);

    for (int i = 0; i < sr - 2; i++) {
        ASSERT_NEAR(out.at(i), in.at(i), 0.001); 
    }

    // stream.play();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(BassStreamTest, StatesTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    EXPECT_EQ(stream.play(), true);
    EXPECT_EQ(stream.getState(), IStream::State::PLAYING);

    EXPECT_EQ(stream.pause(), true);
    EXPECT_EQ(stream.getState(), IStream::State::PAUSED);

    EXPECT_EQ(stream.stop(), true);
    EXPECT_EQ(stream.getState(), IStream::State::STOPPED);
}

TEST_F(BassStreamTest, PositionTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    EXPECT_EQ(stream.setPosition(50), true);
    EXPECT_EQ(stream.getPosition(), 50);

    EXPECT_EQ(stream.setPosition(-50), false);
    EXPECT_EQ(stream.setPosition(1000), false);
}

TEST_F(BassStreamTest, LengthTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    int len = stream.getLength();
    EXPECT_EQ(len, 239);
}

TEST_F(BassStreamTest, VolumeTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    EXPECT_EQ(stream.setVolume(0), true);
    EXPECT_EQ(stream.getVolume(), 0);

    EXPECT_EQ(stream.setVolume(0.5), true);
    EXPECT_EQ(stream.getVolume(), 0.5);

    // Amplification
    EXPECT_EQ(stream.setVolume(10), true);
    EXPECT_EQ(stream.getVolume(), 10);
}

TEST_F(BassStreamTest, FinishedTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);
    stream.setVolume(0);

    float len = stream.getLength();
    EXPECT_EQ(stream.setPosition(len - 0.2), true);
    EXPECT_EQ(stream.play(), true);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(stream.getState(), IStream::State::FINISHED);
}

TEST_F(BassStreamTest, StreamInfoTest) {
    BassStream stream(filePath, BassStream::Type::LOCAL_FILE);

    EXPECT_EQ(stream.getSampleRate(), 48000);
    EXPECT_EQ(stream.getBitrate(), 256);
}
