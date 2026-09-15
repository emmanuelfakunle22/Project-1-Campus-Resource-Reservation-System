// define functions
#ifndef CANCELLATIONSTACK_H
#define CANCELLATIONSTACK_H

#include "Reservation.h"
#include <stack>

class CancellationStack
{
private:
    std::stack<Reservation> cancelledReservations;

public:

};

#endif