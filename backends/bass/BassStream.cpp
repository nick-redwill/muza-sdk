#include "BassStream.h"

#include <algorithm>
#include <iostream>


BassStream::BassStream(uint32_t sampleRate, uint8_t channels) {
    load(sampleRate, channels);
    _type = Type::MEMORY;
}

BassStream::BassStream(HSTREAM stream, Type type) : _stream(stream) {
    _type = type;
}

BassStream::BassStream(const std::string& source, Type type) {
    if (type == Type::MEMORY)
        throw std::runtime_error("Type cannot be MEMORY in this constructor");
    
    if (type == Type::LOCAL_FILE)
        loadFromFile(source);
    else if (type == Type::REMOTE_FILE)
        loadFromUrl(source);
    else if (type == Type::LIVE)
        loadLiveUrl(source);

    _type = type;
}

void BassStream::cleanup() {
    int ret = BASS_StreamFree(_stream);
    if (ret == 0)
        throw std::runtime_error("Unable to cleanup: " + errorStringify(BASS_ErrorGetCode()));
}

BassStream::~BassStream() {
    this->cleanup();
}

bool BassStream::isSupported(Functionality func) {
    // BASS supports all these functions no matter the conditions
    if (func == Functionality::LOAD_MEMORY || 
        func == Functionality::LOAD_FILE ||
        func == Functionality::LOAD_URL || 
        func == Functionality::LOAD_LIVE ||
        func == Functionality::STOP ||
        func == Functionality::PAUSE || 
        func == Functionality::READ
    )
    return true;
    
    // You can't seek a live stream
    if (func == Functionality::SET_POSITION && _type != Type::LIVE)
        return true;

    //TODO: I'm not sure is it possible to write into a file/remote stream
    if (func == Functionality::WRITE && _type == Type::MEMORY)
        return true;

    return false;
}


void BassStream::load(uint32_t sampleRate, uint8_t channels) {
    //TODO: Implement all additionals parameters
    _stream = BASS_StreamCreate(sampleRate, channels, BASS_SAMPLE_FLOAT, STREAMPROC_PUSH, 0);
    if (!_stream)
        throw std::runtime_error("Unable to load empty stream: " + errorStringify(BASS_ErrorGetCode()));

    this->_type = Type::MEMORY;
}

void BassStream::loadFromFile(const std::string& path) {
    //TODO: Implement all additionals parameters
    _stream = BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0);
    if (!_stream)
        throw std::runtime_error("Unable to load local file: " + errorStringify(BASS_ErrorGetCode()));

    BASS_ChannelSetSync(_stream, BASS_SYNC_END, 0, &BassStream::finishedCallback, this);
}

void BassStream::loadFromUrl(const std::string& url) {
    //TODO: Implement all additionals parameters
    _stream = BASS_StreamCreateURL(url.c_str(), 0, 0, 0, 0);
    if (!_stream)
        throw std::runtime_error("Unable to load remote file: " + errorStringify(BASS_ErrorGetCode()));

    BASS_ChannelSetSync(_stream, BASS_SYNC_END, 0, &BassStream::finishedCallback, this);
}

void BassStream::loadLiveUrl(const std::string& url) {
    _stream = BASS_StreamCreateURL(url.c_str(), 0, 0, 0, 0);
    if (!_stream)
        throw std::runtime_error("Unable to load live stream: " + errorStringify(BASS_ErrorGetCode()));

    BASS_ChannelSetSync(_stream, BASS_SYNC_END, 0, &BassStream::finishedCallback, this);
}

IStream::State BassStream::getState() {
    return _state;

    // TODO: Combine this code with returning set state
    // In case stream got stalled or stopped externally
    /*
    uint32_t state = BASS_ChannelIsActive(_stream);
    switch(state) {
        case BASS_ACTIVE_STOPPED: return IStream::State::STOPPED;
        case BASS_ACTIVE_PLAYING: return IStream::State::PLAYING;
        case BASS_ACTIVE_PAUSED: return IStream::State::PAUSED;
        default: return IStream::State::STOPPED;
    }
    */
}

bool BassStream::isPlaying() {
    return getState() == BassStream::State::PLAYING;
}

bool BassStream::isStopped() {
    return getState() == BassStream::State::STOPPED;
}

bool BassStream::isFinished() {
    return _state == State::FINISHED;
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
    //FIXME: This is horrible, fix asap
    float pos = BASS_ChannelBytes2Seconds(_stream, BASS_ChannelGetPosition(_stream, BASS_POS_BYTE));
    pos += sec;
    pos = std::min(std::max(pos, 0.0f), (float)this->getLength());
    
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
    //FIXME: Actually implement (if possible)
    return 0;
}

uint32_t BassStream::getBitrate() { 
    float value;
    BASS_ChannelGetAttribute(_stream, BASS_ATTRIB_BITRATE, &value);
    return (uint32_t)value; 
}

HSTREAM BassStream::getRawStream() {
    return _stream;
}


void BassStream::write(std::vector<float> raw) {
    int res = BASS_StreamPutData(_stream, raw.data(), raw.size() * sizeof(float));
    
    if (res == -1)
        throw std::runtime_error(errorStringify(BASS_ErrorGetCode()));
}

std::vector<float> BassStream::read(uint32_t size) {
    std::vector<float> buffer(size, 0);
    int res = BASS_ChannelGetData(_stream, buffer.data(), size * sizeof(float) | BASS_DATA_FLOAT);

    if (res == -1)
        throw std::runtime_error(errorStringify(BASS_ErrorGetCode()));

    return buffer;
}

std::string BassStream::errorStringify(int code) {
    //TODO: Make this errors more verbose
    switch (code) {
        case 0:  return "BASS_OK";
        case 1:  return "BASS_ERROR_MEM";
        case 2:  return "BASS_ERROR_FILEOPEN";
        case 3:  return "BASS_ERROR_DRIVER";
        case 4:  return "BASS_ERROR_BUFLOST";
        case 5:  return "BASS_ERROR_HANDLE";
        case 6:  return "BASS_ERROR_FORMAT";
        case 7:  return "BASS_ERROR_POSITION";
        case 8:  return "BASS_ERROR_INIT";
        case 9:  return "BASS_ERROR_START";
        case 10: return "BASS_ERROR_SSL";
        case 11: return "BASS_ERROR_REINIT";
        case 14: return "BASS_ERROR_ALREADY";
        case 17: return "BASS_ERROR_NOTAUDIO";
        case 18: return "BASS_ERROR_NOCHAN";
        case 19: return "BASS_ERROR_ILLTYPE";
        case 20: return "BASS_ERROR_ILLPARAM";
        case 21: return "BASS_ERROR_NO3D";
        case 22: return "BASS_ERROR_NOEAX";
        case 23: return "BASS_ERROR_DEVICE";
        case 24: return "BASS_ERROR_NOPLAY";
        case 25: return "BASS_ERROR_FREQ";
        case 27: return "BASS_ERROR_NOTFILE";
        case 29: return "BASS_ERROR_NOHW";
        case 31: return "BASS_ERROR_EMPTY";
        case 32: return "BASS_ERROR_NONET";
        case 33: return "BASS_ERROR_CREATE";
        case 34: return "BASS_ERROR_NOFX";
        case 37: return "BASS_ERROR_NOTAVAIL";
        case 38: return "BASS_ERROR_DECODE";
        case 39: return "BASS_ERROR_DX";
        case 40: return "BASS_ERROR_TIMEOUT";
        case 41: return "BASS_ERROR_FILEFORM";
        case 42: return "BASS_ERROR_SPEAKER";
        case 43: return "BASS_ERROR_VERSION";
        case 44: return "BASS_ERROR_CODEC";
        case 45: return "BASS_ERROR_ENDED";
        case 46: return "BASS_ERROR_BUSY";
        case 47: return "BASS_ERROR_UNSTREAMABLE";
        case 48: return "BASS_ERROR_PROTOCOL";
        case 49: return "BASS_ERROR_DENIED";
        case -1: return "BASS_ERROR_UNKNOWN";
        default: return "Unknown error code";
    }
}

void CALLBACK BassStream::finishedCallback(HSYNC handle, DWORD channel, DWORD data, void *user) {
    BassStream* instance = static_cast<BassStream*>(user);
    instance->_state = State::FINISHED;
 }
