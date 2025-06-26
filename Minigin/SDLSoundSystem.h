#pragma once
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

using sound_id = unsigned short;

struct SoundEvent 
{
    sound_id id;
    float volume;

    SoundEvent(sound_id soundId, float vol) : id(soundId), volume(vol) {}
};

class SoundSystem 
{
public:
    virtual ~SoundSystem() = default;
    virtual void play(sound_id id, float volume) = 0;
    virtual void initialize() = 0;
    virtual void stopAllSounds() = 0;
    virtual void shutdown() = 0;
    virtual void registerSound(sound_id id, const std::string& filepath) = 0;
};

class SoundEventQueue 
{
public:
    void push(const SoundEvent& event) 
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (shutdown) return;
        events.push(event);
        condition.notify_one();
    }

    bool waitForEvent(SoundEvent& event)
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] { return !events.empty() || shutdown; });

        if (shutdown && events.empty()) 
        {
            return false;
        }

        event = events.front();
        events.pop();
        return true;
    }

    void requestShutdown() 
    {
        std::lock_guard<std::mutex> lock(mutex);
        shutdown = true;
        condition.notify_all();
    }

private:
    std::queue<SoundEvent> events;
    mutable std::mutex mutex;
    std::condition_variable condition;
    bool shutdown{};

};

class SDLSoundSystem final : public SoundSystem {
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

    SoundEventQueue eventQueue;
    std::thread audioThread;
    std::atomic<bool> running{ false };

    void audioThreadFunction();

public:
    SDLSoundSystem();
    ~SDLSoundSystem();

    void play(sound_id id, float volume) override;
    void initialize() override;
    void shutdown() override;
    void stopAllSounds() override;
    void registerSound(sound_id id, const std::string& filepath) override;
};

class LoggingSoundSystem final : public SoundSystem
{
private:
    std::unique_ptr<SoundSystem> realSoundSystem;

public:
    LoggingSoundSystem(std::unique_ptr<SoundSystem> soundSystem)
        : realSoundSystem(std::move(soundSystem)) 
    {
    }

    void play(sound_id id, float volume) override 
    {
        std::cout << "playing sound " << id << " at volume " << volume << std::endl;
        realSoundSystem->play(id, volume);
    }

    void initialize() override 
    {
        std::cout << "initializing sound system with logging" << std::endl;
        realSoundSystem->initialize();
    }

    void shutdown() override 
    {
        std::cout << "shutting down sound system" << std::endl;
        realSoundSystem->shutdown();
    }

    void stopAllSounds() override
    {
        std::cout << "stopping all sounds" << std::endl;
        realSoundSystem->stopAllSounds();
    }

    void registerSound(sound_id id, const std::string& filepath) override {
        std::cout << "registering sound " << id << ": " << filepath << std::endl;
        realSoundSystem->registerSound(id, filepath);
    }
};

class NullSoundSystem final : public SoundSystem
{
public:
    void play(sound_id, float) override 
    {
    }

    void initialize() override 
    {
    }

    void shutdown() override 
    {
    }

    void stopAllSounds() override
    {
    }

    void registerSound(sound_id, const std::string&) override 
    {
    }
};

class ServiceLocator final 
{
private:
    static std::unique_ptr<SoundSystem> soundSystemInstance;

public:
    static SoundSystem& getSoundSystem() 
    {
        return *soundSystemInstance;
    }

    static void registerSoundSystem(std::unique_ptr<SoundSystem> soundSystem) 
    {
        soundSystemInstance = soundSystem ? std::move(soundSystem) : std::make_unique<NullSoundSystem>();
    }

    static void initialize() {
        if (!soundSystemInstance) 
        {
            soundSystemInstance = std::make_unique<NullSoundSystem>();
        }
    }

    static void shutdown() {
        if (soundSystemInstance) 
        {
            soundSystemInstance->shutdown();
        }
    }
};
