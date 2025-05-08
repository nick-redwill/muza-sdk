#ifndef BASS_STREAM_H
#define BASS_STREAM_H

#include "../../interfaces/IStream.h"

#include <bass.h>

class BassStream : public IStream {
public:
   BassStream();
   BassStream(uint32_t sampleRate, uint8_t channels);
   BassStream(HSTREAM stream, Type type);
   ~BassStream();

   virtual void loadEmpty(uint32_t sampleRate, uint8_t channels) override;
   virtual void loadFromFile(const std::string& path) override;
   virtual void loadFromUrl(const std::string& url) override;

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

   HSTREAM getRawStream();

private:
   void cleanup();

   inline void raiseOnNoStream();
   std::string errorStringify(int code);

   static void CALLBACK finishedCallback(HSYNC handle, DWORD channel, DWORD data, void *user);

private:
   HSTREAM _stream;
   IStream::State _state = IStream::State::STOPPED;
};
#endif
