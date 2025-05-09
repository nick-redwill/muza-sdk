#include <gtest/gtest.h>

#include "../entities/config.h"
#include "../backends/bass/BassGlobal.h"
#include "../backends/bass/BassStream.h"

#include <chrono>
#include <thread>
#include <math.h>

class BassStreamTest : public ::testing::Test {
protected:
    BassGlobal bass;
    BassStream stream;

    void SetUp() override {
        auto device = bass.getDefaultDevice();
        bass.init(Config(device, 44100, true));
    }

    void TearDown() override {
        bass.clean();
    }
};
    

TEST_F(BassStreamTest, LoadLocalFileTest) {
    stream.loadFromFile("./track.mp3");

    EXPECT_THROW(stream.loadFromFile("nonexistant.mp3"), std::runtime_error);
}

TEST_F(BassStreamTest, NoStreamTest) {
    EXPECT_THROW(stream.play(), std::runtime_error);
}

TEST_F(BassStreamTest, StreamRWTest) {
    //NOTE: Setting playback buffer size to 1 second
    BASS_SetConfig(
        BASS_CONFIG_BUFFER,
        1000
    );

    int sr = 44100; // 1 second
    stream.load(sr, 1);
    
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

    stream.play();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(BassStreamTest, StatesTest) {
    stream.loadFromFile("./track.mp3");

    EXPECT_EQ(stream.play(), true);
    EXPECT_EQ(stream.getState(), IStream::State::PLAYING);

    EXPECT_EQ(stream.pause(), true);
    EXPECT_EQ(stream.getState(), IStream::State::PAUSED);

    EXPECT_EQ(stream.stop(), true);
    EXPECT_EQ(stream.getState(), IStream::State::STOPPED);
}

TEST_F(BassStreamTest, PositionTest) {
    stream.loadFromFile("./track.mp3");

    EXPECT_EQ(stream.setPosition(50), true);
    EXPECT_EQ(stream.getPosition(), 50);

    EXPECT_EQ(stream.setPosition(-50), false);
    EXPECT_EQ(stream.setPosition(1000), false);
}

TEST_F(BassStreamTest, LengthTest) {
    stream.loadFromFile("./track.mp3");

    int len = stream.getLength();
    EXPECT_EQ(len, 239);
}

TEST_F(BassStreamTest, VolumeTest) {
    stream.loadFromFile("./track.mp3");

    EXPECT_EQ(stream.setVolume(0), true);
    EXPECT_EQ(stream.getVolume(), 0);

    EXPECT_EQ(stream.setVolume(0.5), true);
    EXPECT_EQ(stream.getVolume(), 0.5);

    // Amplification
    EXPECT_EQ(stream.setVolume(10), true);
    EXPECT_EQ(stream.getVolume(), 10);
}

TEST_F(BassStreamTest, FinishedTest) {
    stream.loadFromFile("./track.mp3");
    stream.setVolume(0);

    float len = stream.getLength();
    EXPECT_EQ(stream.setPosition(len - 0.2), true);
    EXPECT_EQ(stream.play(), true);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(stream.getState(), IStream::State::FINISHED);
}

TEST_F(BassStreamTest, StreamInfoTest) {
    stream.loadFromFile("./track.mp3");

    EXPECT_EQ(stream.getSampleRate(), 48000);
    EXPECT_EQ(stream.getBitrate(), 256);
}
