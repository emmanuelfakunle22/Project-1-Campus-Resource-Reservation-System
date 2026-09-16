# Project-1-Campus-Resource-Reservation-System
This project is from class CSCE 2110 Foundations of Data Structures. The team members are Emmanuel Fakunle (Team Leader), Andrew Camargo, &amp; Jacob Beard.


You'll see a menu:

```
===== Campus Resource Reservation System =====
1. View Resources
2. View Resource Availability
3. Create Reservation
4. Cancel Reservation
5. View Active Reservations
6. View Waiting Lists
7. Undo Last Cancellation
8. View Cancellation History
9. Exit
Enter Choice:
```

## Error handling checklist

- [x] Invalid/out-of-range menu selections
- [x] File-open failures (missing/misnamed data files)
- [x] Invalid resource IDs on reservation creation
- [x] Invalid reservation IDs on cancellation
- [x] Empty cancellation-history stack (undo with nothing to undo)
- [x] Empty waiting-list queues (display, or removing from an empty list)

See `COMPLEXITY_ANALYSIS.md` for the Big-O discussion.
