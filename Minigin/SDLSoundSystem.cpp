#include <SDL_mixer.h>
#include <unordered_map>
#include <iostream>
#include "SDLSoundSystem.h"

std::unique_ptr<SoundSystem> ServiceLocator::soundSystemInstance{ std::make_unique<NullSoundSystem>() };

class SDLSoundSystem::Impl {
public:
    std::unordered_map<sound_id, Mix_Chunk*> loadedSounds;
    std::unordered_map<sound_id, std::string> soundPaths;

    bool initializeSDL() 
    {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
        {
            return false;
        }
        return true;
    }

    void shutdownSDL() {
        for (auto& pair : loadedSounds) 
        {
            if (pair.second) 
            {
                Mix_FreeChunk(pair.second);
            }
        }
        loadedSounds.clear();
        Mix_CloseAudio();
    }

    void registerSound(sound_id id, const std::string& filepath) 
    {
        soundPaths[id] = filepath;
    }

    Mix_Chunk* loadSound(sound_id id) 
    {
        const auto it{ loadedSounds.find(id) };

        if (it != loadedSounds.end()) 
        {
            return it->second;
        }

        const auto pathIt{ soundPaths.find(id) };

        if (pathIt == soundPaths.end()) 
        {
            return nullptr;
        }

        const auto chunk{ Mix_LoadWAV(pathIt->second.c_str()) };

        if (!chunk) 
        {
            return nullptr;
        }

        loadedSounds[id] = chunk;
        return chunk;
    }

    void playSound(sound_id id, float volume) 
    {
        const auto chunk{ loadSound(id) };

        if (chunk) 
        {
            Mix_VolumeChunk(chunk, static_cast<int>(volume * MIX_MAX_VOLUME));
            Mix_PlayChannel(-1, chunk, 0);
        }
    }

    void stopAllSounds()
    {
        Mix_HaltChannel(-1);
    }
};

SDLSoundSystem::SDLSoundSystem() : pImpl(std::make_unique<Impl>()) {}

SDLSoundSystem::~SDLSoundSystem() 
{
    shutdown();
}

void SDLSoundSystem::initialize() 
{
    if (!pImpl->initializeSDL()) 
    {
        throw std::runtime_error("failed to initialize audio");
    }

    running = true;
    audioThread = std::thread(&SDLSoundSystem::audioThreadFunction, this);
}

void SDLSoundSystem::shutdown() 
{
    if (running.exchange(false))
    {
        eventQueue.requestShutdown();
        if (audioThread.joinable())
        {
            audioThread.join();
        }
        pImpl->shutdownSDL();
    }
}

void SDLSoundSystem::stopAllSounds()
{
    pImpl->stopAllSounds();
}

void SDLSoundSystem::play(sound_id id, float volume) 
{
    if (running) 
    {
        eventQueue.push(SoundEvent(id, volume));
    }
}

void SDLSoundSystem::registerSound(sound_id id, const std::string& filepath) 
{
    pImpl->registerSound(id, filepath);
}

void SDLSoundSystem::audioThreadFunction() 
{
    SoundEvent event(0, 0);
    while (eventQueue.waitForEvent(event)) 
    {
        pImpl->playSound(event.id, event.volume);
    }
}