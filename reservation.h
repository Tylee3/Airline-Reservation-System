/**
 * Seat Reservation System Header File
 * Contains structure definitions, constants, and function declarations
 */

#ifndef RESERVATION_H
#define RESERVATION_H
#include <gdbm.h>

// System constants
#define MAX_RESERVATIONS 100            // Maximum number of seat reservations
#define MAX_STRING 1024                 // Maximum length for string fields
#define MAX_ROWS 30                     // Maximum rows for the seat map
#define MAX_COLS 6                      // Maximum columns for the seat map
#define DB_FILE "reservation_data.gdbm" // GDBM database file name

/**
 * Reservation structure - represents a single seat reservation record
 * Contains passenger and flight information along with seat details
 */
typedef struct
{
    int id;                          // Unique reservation ID
    char passenger_name[MAX_STRING]; // Passenger name
    char flight_number[MAX_STRING];  // Flight number
    char flight_date[MAX_STRING];    // Date of flight
    char seat_number[8];             // Seat label
    char status[MAX_STRING];         // Status
} Reservation;

// Global variables
extern Reservation reservations[MAX_RESERVATIONS]; // Array to store reservations
extern int reservation_count;                      // Current number of reservations
extern int next_id;                                // Next unique ID to assign

// Reservation-related functions
void save_reservations();   // Save all reservations to GDBM
void load_reservations();   // Load reservations from GDBM
void add_reservation();     // Add a new seat reservation
void view_reservations();   // Display all reservations in table format
void update_reservation();  // Update existing reservation by ID
void delete_reservation();  // Delete reservation by ID
void search_reservations(); // Search reservations by passenger or flight

#endif