#ifndef BASS_STREAM_H
#define BASS_STREAM_H

#include "../../interfaces/IStream.h"

#include <bass.h>
#include <string>

class BassStream : public IStream {
public:
    BassStream(uint32_t sampleRate, uint8_t channels);
    BassStream(const std::string& source, Type type);
    BassStream(HSTREAM stream, Type type);
    virtual ~BassStream();

    virtual bool isSupported(IStream::Functionality func) override;

    virtual IStream::State getState() override;
    virtual bool isPlaying() override;
    virtual bool isStopped() override;
    virtual bool isFinished() override;

    virtual bool play() override;
    virtual bool pause() override;
    virtual bool stop() override;

    virtual bool seek(float sec) override;
    virtual bool setPosition(float sec) override;
    virtual float getPosition() override;

    virtual bool setVolume(float volume) override;
    virtual float getVolume() override;

    virtual float getLength() override;
    virtual uint32_t getSampleRate() override;
    virtual uint8_t getChannelsCount() override;
    virtual uint32_t getBitrate() override;

    virtual void write(std::vector<float> raw) override;
    virtual std::vector<float> read(uint32_t size) override;

    HSTREAM getRawStream();

private:
    virtual void load(uint32_t sampleRate, uint8_t channels) override;
    virtual void loadFromFile(const std::string& path) override;
    virtual void loadFromUrl(const std::string& url) override;
    virtual void loadLiveUrl(const std::string& url) override;

    void cleanup();

    std::string errorStringify(int code);

    static void CALLBACK finishedCallback(HSYNC handle, DWORD channel, DWORD data, void *user);

private:
    HSTREAM _stream = 0;
    IStream::State _state = IStream::State::STOPPED;
};
#endif
