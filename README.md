# MUZA-SDK

**MUZA** is a universal audio processing SDK designed to provide a unified interface for working with various audio libraries. It achieves this by defining abstract base classes and implementing polymorphic, library-specific derived classes. This architecture abstracts away the implementation details of different backends, enabling seamless switching between libraries—both at compile time and at runtime (supporting hot-swapping).

## 🚀 Features
* Unified interface for multiple audio libraries
* Backend switching without changing client code
* Support for runtime hot-swapping
* Modular architecture for easy extension
* Unit-tested and C++11-compatible

## 📦 Building & Usage

```
git clone https://github.com/nick-redwill/muza-sdk.git
cd muza-sdk
mkdir build && cd build
cmake ..
make
```

### Run the CLI Tool
```
./muza_cli path/to/audiofile.mp3 --freq 44100 --stereo
./muza_cli https://url.to/audiofile.mp3 --stereo --url
```

### Run Unit Tests
```
ctest
```

> **Note**: You must provide the required backend binaries (e.g., BASS) in the lib/ directory. These are not distributed with this repository.

## ⚠️ Important Note
**MUZA-SDK** uses proprietary libraries such as BASS and potentially FMOD or Superpowered. Therefore, their source code is not included in this repository. Users are responsible for obtaining and complying with the licenses of any third-party libraries used.

## ✅ Currently Supported Backends

| Backend      | Status                 |
| ------------ | ---------------------- |
| [BASS](https://www.un4seen.com/bass.html)         | ✅ Supported           |
| [FMOD](https://www.fmod.com/core)         | 🚧 Planned             |
| [Superpowered](https://superpowered.com/audio-overview) | 🚧 Planned             |
| [JUCE](https://juce.com/)         | ❓ Under Consideration |


## 📋 Roadmap / TODO
- [x] Define interfaces for global management and audio streams  
- [x] Implement interfaces using BASS
- [x] Provide simple command-line tool  
- [x] Add GoogleTest unit tests
- [x] Add capability checking (**isSupported()**)
- [ ] Provide additional backend implementations
- [ ] Add comprehensive documentation and usage examples 
- [ ] Improve error handling and resilience 
- [ ] Implement SFX Module
- [ ] Implement Metadata Module
- [ ] Implement FFT Module
- [ ] Implement Recording Module
- [ ] Integrate logging system

## ❓ FAQ
### Why not include backend sources?
Due to licensing restrictions, third-party libraries like BASS and FMOD cannot be redistributed. You must obtain them separately.

### Can I use MUZA-SDK in a commercial project?
Yes, but **MUZA-SDK** is licensed under the **GPL-3.0 License**, meaning you must open-source your product if you distribute it. Additionally, be sure to review and comply with the licensing terms of any backends you use.


### Is there documentation or examples?
Not yet—but they are planned and will be added soon.

### What C++ standard is MUZA-SDK based on?
**MUZA-SDK** is fully compatible with **C++11**.
