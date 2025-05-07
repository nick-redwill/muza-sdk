#ifndef BASS_STREAM_H
#define BASS_STREAM_H

#include "../interfaces/IStream.h"

#include <bass.h>

class BassStream : public IStream {
public:
   BassStream();
   BassStream(HSTREAM stream, Type type);
   ~BassStream();

   virtual void loadFromFile(const std::string& path) override;
   virtual void loadFromUrl(const std::string& url) override;

   virtual IStream::State getState() override;
   virtual bool isPlaying() override;
   virtual bool isStopped() override;

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

   virtual void* getRawStream() override;

private:
   HSTREAM _stream;

   void cleanup();
};
#endif
