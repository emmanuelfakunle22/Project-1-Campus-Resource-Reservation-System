#include "ReservationManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>

ReservationManager::ReservationManager() : nextReservationID(301) {}

// ---------------------------------------------------------------
// Resource Management
// ---------------------------------------------------------------

// Loads resources from a pipe-delimited file:
//   ResourceID|ResourceName|ResourceType|Available|Unavailable
bool ReservationManager::loadResources(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open resource file: " << filename << "\n";
        return false;
    }

    resources.clear();
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id, name, type, statusStr;

        if (!std::getline(ss, id, '|') ||
            !std::getline(ss, name, '|') ||
            !std::getline(ss, type, '|') ||
            !std::getline(ss, statusStr, '|')) {
            std::cout << "WARNING: Skipping malformed line " << lineNumber
                      << " in " << filename << "\n";
            continue;
        }

        bool available = (statusStr == "Available");
        resources.push_back(Resource(id, name, type, available));
    }

    file.close();
    std::cout << "Loaded " << resources.size() << " resources from " << filename << ".\n";
    return true;
}

void ReservationManager::displayResources() const {
    if (resources.empty()) {
        std::cout << "  No resources loaded.\n";
        return;
    }
    std::cout << std::left
              << std::setw(8)  << "ID"
              << std::setw(20) << "Name"
              << std::setw(22) << "Type"
              << "Status" << "\n";
    std::cout << std::string(60, '-') << "\n";
    for (const auto& r : resources) {
        r.display();
    }
}

void ReservationManager::displayResourceAvailability() const {
    if (resources.empty()) {
        std::cout << "  No resources loaded.\n";
        return;
    }
    int availableCount = 0;
    for (const auto& r : resources) {
        if (r.isAvailable()) availableCount++;
    }
    std::cout << "Resource Availability Summary\n";
    std::cout << std::string(35, '-') << "\n";
    std::cout << "Total resources:     " << resources.size() << "\n";
    std::cout << "Available:           " << availableCount << "\n";
    std::cout << "Unavailable:         " << (resources.size() - availableCount) << "\n\n";

    std::cout << std::left
              << std::setw(8)  << "ID"
              << std::setw(20) << "Name"
              << "Status" << "\n";
    std::cout << std::string(45, '-') << "\n";
    for (const auto& r : resources) {
        std::cout << std::left << std::setw(8) << r.getID()
                   << std::setw(20) << r.getName()
                   << (r.isAvailable() ? "Available" : "Unavailable") << "\n";
    }
}

bool ReservationManager::resourceExists(const std::string& resourceID) const {
    for (const auto& r : resources) {
        if (r.getID() == resourceID) return true;
    }
    return false;
}

Resource* ReservationManager::findResource(const std::string& resourceID) {
    for (auto& r : resources) {
        if (r.getID() == resourceID) return &r;
    }
    return nullptr;
}

// ---------------------------------------------------------------
// Reservation Management
// ---------------------------------------------------------------

// Loads pre-existing reservation records from a pipe-delimited file:
//   ReservationID|StudentID|StudentName|ResourceID|Date
// This is used to preload sample/historical data for testing. It does
// not alter resource availability flags, which are treated as the
// authoritative current state coming from resources.txt.
bool ReservationManager::loadReservations(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open reservation file: " << filename << "\n";
        return false;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    int highestID = nextReservationID - 1;

    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string idStr, studIDStr, name, resourceID, date;

        if (!std::getline(ss, idStr, '|') ||
            !std::getline(ss, studIDStr, '|') ||
            !std::getline(ss, name, '|') ||
            !std::getline(ss, resourceID, '|') ||
            !std::getline(ss, date, '|')) {
            std::cout << "WARNING: Skipping malformed line " << lineNumber
                      << " in " << filename << "\n";
            continue;
        }

        try {
            int reservationID = std::stoi(idStr);
            int studentID = std::stoi(studIDStr);

            Reservation r(reservationID, studentID, name, resourceID, date);
            activeReservations.insert(r);
            loadedCount++;

            if (reservationID > highestID) highestID = reservationID;
        } catch (const std::exception&) {
            std::cout << "WARNING: Skipping line " << lineNumber
                      << " with invalid numeric field.\n";
        }
    }

    file.close();
    nextReservationID = highestID + 1;
    std::cout << "Loaded " << loadedCount << " reservations from " << filename << ".\n";
    return true;
}

// Validates a date string is in MM/DD/YYYY format and represents a real
// calendar date (correct days-per-month, including leap years).
bool ReservationManager::isValidDate(const std::string& date) {
    // Expected shape: MM/DD/YYYY -> exactly 10 characters.
    if (date.size() != 10 || date[2] != '/' || date[5] != '/') {
        return false;
    }

    std::string monthStr = date.substr(0, 2);
    std::string dayStr = date.substr(3, 2);
    std::string yearStr = date.substr(6, 4);

    for (char c : monthStr) if (!isdigit(static_cast<unsigned char>(c))) return false;
    for (char c : dayStr)   if (!isdigit(static_cast<unsigned char>(c))) return false;
    for (char c : yearStr)  if (!isdigit(static_cast<unsigned char>(c))) return false;

    int month = std::stoi(monthStr);
    int day = std::stoi(dayStr);
    int year = std::stoi(yearStr);

    if (month < 1 || month > 12) return false;
    if (year < 1900 || year > 2999) return false;

    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];

    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && isLeapYear) maxDay = 29;

    if (day < 1 || day > maxDay) return false;

    return true;
}

// Checks whether 'studentID' already has an active reservation for
// 'resourceID', to prevent the same student double-booking the same
// resource. O(n) traversal of the active-reservations linked list.
bool ReservationManager::hasDuplicateReservation(int studentID, const std::string& resourceID) const {
    Reservation dummy;
    return activeReservations.find(
        [studentID, resourceID](const Reservation& r) {
            return r.getStudentID() == studentID && r.getResourceID() == resourceID;
        },
        dummy);
}

CreateResult ReservationManager::createReservation(int studentID, const std::string& studentName,
                                                     const std::string& resourceID,
                                                     const std::string& date) {
    Resource* resource = findResource(resourceID);
    if (resource == nullptr) {
        return CreateResult::INVALID_RESOURCE;
    }

    if (!isValidDate(date)) {
        return CreateResult::INVALID_DATE;
    }

    if (hasDuplicateReservation(studentID, resourceID)) {
        return CreateResult::DUPLICATE_REQUEST;
    }

    if (!resource->isAvailable()) {
        // Resource is busy: place the student on its FIFO waiting list.
        waitingList.addRequest(resourceID, studentID, studentName);
        return CreateResult::ADDED_TO_WAITLIST;
    }

    // Resource is free: create the reservation and mark it unavailable.
    int newID = nextReservationID++;
    Reservation r(newID, studentID, studentName, resourceID, date);
    activeReservations.insert(r);
    resource->setAvailable(false);

    std::cout << "Reservation Created Successfully. (Reservation ID: " << newID << ")\n";
    return CreateResult::SUCCESS;
}

bool ReservationManager::cancelReservation(int reservationID) {
    Reservation removed;
    bool found = activeReservations.remove(
        [reservationID](const Reservation& r) { return r.getReservationID() == reservationID; },
        removed);

    if (!found) {
        return false;
    }

    // Record on the cancellation history stack (undo support).
    cancellationHistory.recordCancellation(removed);

    // Free up the resource.
    Resource* resource = findResource(removed.getResourceID());
    if (resource != nullptr) {
        resource->setAvailable(true);
    }

    std::cout << "Reservation Cancelled.\nAdded to cancellation history.\n";

    // Automatically assign the now-free resource to the next waiting
    // student, if any (FIFO waiting list processing).
    if (resource != nullptr && !waitingList.isEmpty(removed.getResourceID())) {
        WaitingRequest next;
        if (waitingList.removeNext(removed.getResourceID(), next)) {
            int newID = nextReservationID++;
            Reservation autoReservation(newID, next.getStudentID(), next.getStudentName(),
                                         next.getResourceID(), removed.getDate());
            activeReservations.insert(autoReservation);
            resource->setAvailable(false);

            std::cout << "Resource " << removed.getResourceID()
                      << " automatically assigned to next student in waiting list: "
                      << next.getStudentName() << " (Student #" << next.getStudentID()
                      << "), Reservation ID: " << newID << "\n";
        }
    }

    return true;
}

bool ReservationManager::reservationExists(int reservationID) const {
    Reservation dummy;
    return activeReservations.find(
        [reservationID](const Reservation& r) { return r.getReservationID() == reservationID; },
        dummy);
}

void ReservationManager::displayActiveReservations() const {
    std::cout << "Active Reservations (" << activeReservations.size() << " total):\n";
    activeReservations.display();
}

// ---------------------------------------------------------------
// Waiting List Management
// ---------------------------------------------------------------

void ReservationManager::displayWaitingLists() const {
    // WaitingList's internal map is not const-iterable through this
    // wrapper without a const overload, so we cast away constness here
    // for the purpose of a read-only display call.
    const_cast<WaitingList&>(waitingList).displayAll();
}

// ---------------------------------------------------------------
// Cancellation History (Undo)
// ---------------------------------------------------------------

bool ReservationManager::undoCancellation() {
    Reservation restored;
    if (!cancellationHistory.undoLastCancellation(restored)) {
        std::cout << "No cancellations to undo.\n";
        return false;
    }

    // Put the reservation back into the active list.
    activeReservations.insert(restored);

    // Mark the resource unavailable again, if it exists.
    Resource* resource = findResource(restored.getResourceID());
    if (resource != nullptr) {
        resource->setAvailable(false);
    }

    std::cout << "Reservation Restored Successfully.\n";
    restored.display();
    return true;
}

void ReservationManager::displayCancellationHistory() const {
    std::cout << "Cancellation History (most recent first):\n";
    cancellationHistory.display();
}
