/**
 * Seat Map Header File
 * Defines seat map structure and related functions
 */

#ifndef SEATMAP_H
#define SEATMAP_H
#include <gdbm.h>
#include "reservation.h"
#define SEATMAP_DB "seatmap_data.gdbm"

/**
 * SeatMap structure - represents seating layout for a flight
 * O for open, X for taken
 */
typedef struct
{
    char flight_number[MAX_STRING]; // Flight identifier
    int rows;                       // Number of seat rows
    int cols;                       // Seats per row
    char seats[MAX_ROWS][MAX_COLS]; // Seat grid
} SeatMap;

// Function declarations
void view_seatmap();                                     // Display seat layout
void reserve_seat(const char *flight, const char *seat); // Automatically creates map and mark the seat taken
void release_seat(const char *flight, const char *seat); // Automatically frees the seat if deleted or changed

#endif
