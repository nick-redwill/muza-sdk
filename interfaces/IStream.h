#ifndef IAUDIO_H
#define IAUDIO_H

#include <stdint.h>
#include <string>

class IStream {
public:
   enum class State {
       STOPPED, PLAYING, PAUSED, FINISHED
   };
   enum class Type {
       NONE = 0, EMPTY, 
       LOCAL_FILE, REMOTE_FILE, 
       LIVE, 
       DECODE
   };

   virtual void loadEmpty(uint32_t sampleRate, uint8_t channels) = 0;
   virtual void loadFromFile(const std::string& path) = 0;
   virtual void loadFromUrl(const std::string& url) = 0;

   virtual IStream::State getState() = 0;
   virtual bool isStopped() = 0;
   virtual bool isPlaying() = 0;
   virtual bool isFinished() = 0;
   
   virtual bool play() = 0;
   virtual bool stop() = 0;
   virtual bool pause() = 0;

   virtual bool seek(float sec) = 0;
   virtual bool setPosition(float sec) = 0;
   virtual float getPosition() = 0;
   
   virtual bool setVolume(float volume) = 0;
   virtual float getVolume() = 0;

   virtual float getLength() = 0;
   virtual uint32_t getSampleRate() = 0;
   virtual uint8_t getChannelsCount() = 0;
   virtual uint32_t getBitrate() = 0;

protected:
   Type _type = Type::NONE;
};
#endif
