#include "BassStream.h"

#include <algorithm>
#include <iostream>

BassStream::BassStream() {
    _stream = BASS_StreamCreate(44100, 2, 0, 0, 0);
    this->_type = IStream::Type::EMPTY;
}

BassStream::BassStream(HSTREAM stream, BassStream::Type type) : _stream(stream) {
    this->_type = type;
}

void BassStream::cleanup() {
    BASS_StreamFree(_stream);
}

BassStream::~BassStream() {
    this->cleanup();
}

void BassStream::loadFromFile(const std::string& path) {
    this->cleanup();

    //TODO: Implement all additionals parameters
    _stream = BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0);
    if (!_stream)
        throw std::runtime_error("Unable to load file"); //TODO: Throw more specific error based on BASS ErrorCode

    this->_type = IStream::Type::LOCAL_FILE;
}

void BassStream::loadFromUrl(const std::string& url) {
    this->cleanup();

    //TODO: Implement all additionals parameters
    _stream = BASS_StreamCreateURL(url.c_str(), 0, 0, 0, 0);
    if (!_stream)
        throw std::runtime_error("Unable to load remote file"); //TODO: Throw more specific error based on BASS ErrorCode
    
    this->_type = IStream::Type::REMOTE_FILE;
}

IStream::State BassStream::getState() {
    return _state;
}

bool BassStream::isPlaying() {
    return _state == BassStream::State::PLAYING;
}

bool BassStream::isStopped() {
    return _state == BassStream::State::STOPPED;
}

bool BassStream::play() {
    bool ret = BASS_ChannelPlay(_stream, false);
    if (ret) _state = State::PLAYING;
    return ret;
}

bool BassStream::pause() {
    bool ret = BASS_ChannelPause(_stream);
    if (ret) _state = State::PAUSED;
    return ret;
}

bool BassStream::stop() {
    bool ret = BASS_ChannelStop(_stream);
    if (ret) _state = State::STOPPED;
    return ret;
}

bool BassStream::seek(float sec) {
    float pos = BASS_ChannelBytes2Seconds(_stream, BASS_ChannelGetPosition(_stream, BASS_POS_BYTE));
    pos += sec;
    pos = std::min(std::max(pos, 0.0f), (float)this->getLength());
    
    std::cout << pos << "\n";
    return BASS_ChannelSetPosition(_stream, BASS_ChannelSeconds2Bytes(_stream, pos), BASS_POS_BYTE);
}

bool BassStream::setPosition(float sec) {
    return BASS_ChannelSetPosition(_stream, BASS_ChannelSeconds2Bytes(_stream, sec), BASS_POS_BYTE);
}

float BassStream::getPosition() { 
    return BASS_ChannelBytes2Seconds(
       _stream, BASS_ChannelGetPosition(_stream, BASS_POS_BYTE)
    ); 
}


bool BassStream::setVolume(float volume) {
    return BASS_ChannelSetAttribute(_stream, BASS_ATTRIB_VOL, volume);
}

float BassStream::getVolume() {
    float volume = 0;
    BASS_ChannelGetAttribute(_stream, BASS_ATTRIB_VOL, &volume);
    return volume;
}

float BassStream::getLength() { 
    return BASS_ChannelBytes2Seconds(
       _stream, BASS_ChannelGetLength(_stream, BASS_POS_BYTE)
    ); 
}

uint32_t BassStream::getSampleRate() { 
    float value;
    BASS_ChannelGetAttribute(_stream, BASS_ATTRIB_FREQ, &value);
    return (uint32_t)value;
}

uint8_t BassStream::getChannelsCount() { 
    return 0;
}

uint32_t BassStream::getBitrate() { 
    float value;
    BASS_ChannelGetAttribute(_stream, BASS_ATTRIB_BITRATE, &value);
    return (uint32_t)value; 
}

void* BassStream::getRawStream() {
    return (void*)_stream;
}
