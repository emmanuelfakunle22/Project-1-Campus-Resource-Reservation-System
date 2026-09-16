#include <iostream>
#include <limits>
#include <string>
#include "ReservationManager.h"

// ---------------------------------------------------------------
// Helper: safely read an integer from std::cin, re-prompting on
// invalid (non-numeric) input. Prevents the program from crashing
// or looping forever on bad input.
// ---------------------------------------------------------------
int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a whole number.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

void printMenu() {
    std::cout << "\n===== Campus Resource Reservation System =====\n";
    std::cout << "1. View Resources\n";
    std::cout << "2. View Resource Availability\n";
    std::cout << "3. Create Reservation\n";
    std::cout << "4. Cancel Reservation\n";
    std::cout << "5. View Active Reservations\n";
    std::cout << "6. View Waiting Lists\n";
    std::cout << "7. Undo Last Cancellation\n";
    std::cout << "8. View Cancellation History\n";
    std::cout << "9. Exit\n";
}

int main() {
    ReservationManager manager;

    // Load starting data. Paths are relative to the working directory
    // the program is run from (see README for build/run instructions).
    manager.loadResources("data/resources.txt");
    manager.loadReservations("data/reservations.txt");

    bool running = true;
    while (running) {
        printMenu();
        int choice = readInt("Enter Choice: ");

        switch (choice) {
            case 1: {
                std::cout << "\n--- All Resources ---\n";
                manager.displayResources();
                break;
            }
            case 2: {
                std::cout << "\n--- Resource Availability ---\n";
                manager.displayResourceAvailability();
                break;
            }
            case 3: {
                std::cout << "\n--- Create Reservation ---\n";
                int studentID = readInt("Student ID: ");
                std::string studentName = readLine("Student Name: ");
                std::string resourceID = readLine("Resource ID: ");
                std::string date = readLine("Reservation Date (MM/DD/YYYY): ");

                // Basic validation of the request before handing off
                // to the manager.
                if (studentID <= 0) {
                    std::cout << "ERROR: Student ID must be a positive number.\n";
                    break;
                }
                if (studentName.empty() || resourceID.empty() || date.empty()) {
                    std::cout << "ERROR: Student name, resource ID, and date are all required.\n";
                    break;
                }

                CreateResult result = manager.createReservation(studentID, studentName, resourceID, date);
                switch (result) {
                    case CreateResult::INVALID_RESOURCE:
                        std::cout << "ERROR: No resource found with ID '" << resourceID << "'.\n";
                        break;
                    case CreateResult::INVALID_DATE:
                        std::cout << "ERROR: '" << date << "' is not a valid date. "
                                  << "Please use MM/DD/YYYY format (e.g. 09/30/2026).\n";
                        break;
                    case CreateResult::DUPLICATE_REQUEST:
                        std::cout << "ERROR: Student #" << studentID
                                  << " already has an active reservation for resource '"
                                  << resourceID << "'.\n";
                        break;
                    case CreateResult::ADDED_TO_WAITLIST:
                        std::cout << "Resource '" << resourceID << "' is currently unavailable.\n";
                        std::cout << studentName << " has been added to the waiting list for this resource.\n";
                        break;
                    case CreateResult::SUCCESS:
                        // Success message is already printed inside createReservation().
                        break;
                }
                break;
            }
            case 4: {
                std::cout << "\n--- Cancel Reservation ---\n";
                int reservationID = readInt("Reservation ID: ");
                if (!manager.cancelReservation(reservationID)) {
                    std::cout << "ERROR: No active reservation found with ID " << reservationID << ".\n";
                }
                break;
            }
            case 5: {
                std::cout << "\n--- Active Reservations ---\n";
                manager.displayActiveReservations();
                break;
            }
            case 6: {
                std::cout << "\n--- Waiting Lists ---\n";
                manager.displayWaitingLists();
                break;
            }
            case 7: {
                std::cout << "\n--- Undo Last Cancellation ---\n";
                manager.undoCancellation();
                break;
            }
            case 8: {
                std::cout << "\n--- Cancellation History ---\n";
                manager.displayCancellationHistory();
                break;
            }
            case 9: {
                std::cout << "Goodbye!\n";
                running = false;
                break;
            }
            default: {
                std::cout << "Invalid choice. Please enter a number between 1 and 9.\n";
                break;
            }
        }
    }

    return 0;
}
