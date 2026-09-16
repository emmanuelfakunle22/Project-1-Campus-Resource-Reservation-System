#include "Resource.h"
#include <iostream>
#include <iomanip>

Resource::Resource()
    : resourceID(""), resourceName(""), resourceType(""), available(false) {}

Resource::Resource(const std::string& id, const std::string& name,
                    const std::string& type, bool isAvailable)
    : resourceID(id), resourceName(name), resourceType(type), available(isAvailable) {}

std::string Resource::getID() const { return resourceID; }
std::string Resource::getName() const { return resourceName; }
std::string Resource::getType() const { return resourceType; }
bool Resource::isAvailable() const { return available; }

void Resource::setAvailable(bool isAvailable) { available = isAvailable; }

void Resource::display() const {
    std::cout << std::left
              << std::setw(8)  << resourceID
              << std::setw(20) << resourceName
              << std::setw(22) << resourceType
              << (available ? "Available" : "Unavailable")
              << "\n";
}
