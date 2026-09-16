#ifndef CANCELLATIONHISTORY_H
#define CANCELLATIONHISTORY_H

#include "Reservation.h"
#include <stack>

class CancellationHistory {
private:
    std::stack<Reservation> history;

public:
    void recordCancellation(const Reservation& reservation);
    bool undoLastCancellation(Reservation& restored);
    void display() const;
};

#endif
