#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include <string>
#include <iostream>
#include "SDLSoundSystem.h"

namespace dae
{
    class GameObject;
}

struct EventData
{
    std::string eventType;
    dae::GameObject* gameObject{ nullptr };
	int intValue{ 0 };
    std::string stringValue;

    EventData(const std::string& type) : eventType(type) {}
    EventData(const std::string& type, dae::GameObject* obj) : eventType(type), gameObject(obj) {}
    EventData(const std::string& type, dae::GameObject* obj, int value) : eventType(type), gameObject(obj), intValue(value) {}
};

class Observer
{
public:
    virtual ~Observer() = default;
    virtual void OnNotify(const EventData& event) = 0;
};