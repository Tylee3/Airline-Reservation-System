#Airline Reservation System

## Problem Statement
A C-based seat reservation system that allows users to manage airline reservations and automatically maintain seat maps for each flight. The system eliminates manual tracking errors by synchronizing reservations and seat availability using persistent storage through GDBM. Users can add, view, update, delete, and search reservations while ensuring that seat assignments are valid and synchronized in real time.

## Design and Architecture Details
1. Add new reservations with automatic ID assignment
2. View all reservations in a formatted table display
3. Update existing reservations (passenger, flight, date, or seat)
4. Delete reservations by ID
5. Search reservations by passenger name, flight number, or date
6. Automatically generate seat maps for new flights
7. Automatically mark and release seats as reservations are made or deleted
8. Persistent data storage using GDBM database files
9. Built-in validation for seat numbers, dates, and input entries

## Pros
2. All reservations and seat layouts are stored in GDBM
3. Separation of concerns between reservation logic and seat map logic
4. Easy to use and navigate through all features
5. Reservations and seat map data are consistent across the system
6. Basic error handling prevents crashes caused by blank fields, invalid seat labels, or non-existent IDs
7. Users can easily find reservations by passenger, flight, or date using partial text matches
8. New seat maps are generated automatically for each flight, reducing setup time and user effort.

Data Consistency – Each operation ensures database integrity by automatically saving changes after every update or deletion.
## Cons
1. Single task operation
2. All flights use 30 rows by 6 columns layout
3. Does not validate out of range input from user
4. Very user reliant, errors are handled, but mistakes in input aren't
5. GDBM stores data unecrypted
6. Terminal interface only
