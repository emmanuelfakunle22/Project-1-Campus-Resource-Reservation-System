#include "Reservation.h"
#include <iostream>
#include <iomanip>

Reservation::Reservation()
    : reservationID(0), studentID(0), studentName(""), resourceID(""), reservationDate("") {}

Reservation::Reservation(int reservationID, int studentID, const std::string& studentName,
                          const std::string& resourceID, const std::string& date)
    : reservationID(reservationID), studentID(studentID), studentName(studentName),
      resourceID(resourceID), reservationDate(date) {}

int Reservation::getReservationID() const { return reservationID; }
int Reservation::getStudentID() const { return studentID; }
std::string Reservation::getStudentName() const { return studentName; }
std::string Reservation::getResourceID() const { return resourceID; }
std::string Reservation::getDate() const { return reservationDate; }

void Reservation::display() const {
    std::cout << std::left
              << "Res#" << std::setw(6) << reservationID
              << "Student#" << std::setw(8) << studentID
              << std::setw(20) << studentName
              << "Resource:" << std::setw(8) << resourceID
              << "Date:" << reservationDate
              << "\n";
}
