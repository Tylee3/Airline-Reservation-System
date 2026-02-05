#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "seatmap.h"

/**
 * Opens the GDBM seat map database
 */
static GDBM_FILE open_seatmap_db()
{
    GDBM_FILE db = gdbm_open(SEATMAP_DB, 0, GDBM_WRCREAT, 0644, 0);
    if (!db)
    {
        printf("Error: Could not open seat map database '%s'.\n", SEATMAP_DB);
    }
    return db;
}

/**
 * Automatically creates a seat map for a flight
 * All seats become open when created
 */
static void create_seatmap(GDBM_FILE db, const char *flight)
{
    SeatMap sm;
    memset(&sm, 0, sizeof(sm));
    strncpy(sm.flight_number, flight, MAX_STRING - 1);
    sm.rows = 30;
    sm.cols = 6;

    for (int r = 0; r < sm.rows; r++)
        for (int c = 0; c < sm.cols; c++)
            sm.seats[r][c] = 'O';

    datum k = {(char *)flight, (int)strlen(flight)};
    datum v = {(char *)&sm, sizeof(SeatMap)};
    gdbm_store(db, k, v, GDBM_REPLACE);

    printf("Created seat map for new flight %s.\n", flight);
}

/**
 * Displays a seat map for a given flight
 * O for open, X for taken
 */
void view_seatmap()
{
    char flight[MAX_STRING];
    printf("Enter Flight Number: ");
    fgets(flight, MAX_STRING, stdin);
    flight[strcspn(flight, "\n")] = 0;

    GDBM_FILE db = open_seatmap_db();
    if (!db)
        return;

    datum k = {flight, (int)strlen(flight)};
    datum v = gdbm_fetch(db, k);

    if (!v.dptr)
    {
        printf("No seat map found for flight %s.\n", flight);
        gdbm_close(db);
        return;
    }

    SeatMap *sm = (SeatMap *)v.dptr;
    printf("\nSeat Map for Flight %s\n", sm->flight_number);
    printf("---------------------------------\n");

    for (int r = 0; r < sm->rows; r++)
    {
        printf("Row %2d: ", r + 1);
        for (int c = 0; c < sm->cols; c++)
            printf(" %c", sm->seats[r][c]);
        printf("\n");
    }

    printf("---------------------------------\n");
    printf("O = Open, X = Taken\n");

    free(v.dptr);
    gdbm_close(db);
}

/**
 * Automatically reserves a seat in a flight's seat map
 * Creates the map if it does not exist
 */
void reserve_seat(const char *flight, const char *seat)
{
    GDBM_FILE db = open_seatmap_db();
    if (!db)
        return;

    datum k = {(char *)flight, (int)strlen(flight)};
    datum v = gdbm_fetch(db, k);

    if (!v.dptr)
    {
        create_seatmap(db, flight);
        v = gdbm_fetch(db, k);
    }

    SeatMap sm;
    memcpy(&sm, v.dptr, sizeof(SeatMap));
    free(v.dptr);

    int row = atoi(seat) - 1;
    int col = -1;

    for (int i = 0; i < (int)strlen(seat); i++)
    {
        if (seat[i] >= 'A' && seat[i] <= 'Z')
        {
            col = seat[i] - 'A';
            break;
        }
    }

    if (col == -1)
    {
        col = 0;
    }

    if (row < 0 || row >= sm.rows || col < 0 || col >= sm.cols)
    {
        printf("Invalid seat %s for flight %s.\n", seat, flight);
        gdbm_close(db);
        return;
    }

    if (sm.seats[row][col] == 'X')
    {
        printf("Seat %s on flight %s is already taken!\n", seat, flight);
    }
    else
    {
        sm.seats[row][col] = 'X';
        datum new_val = {(char *)&sm, sizeof(SeatMap)};
        gdbm_store(db, k, new_val, GDBM_REPLACE);
        printf("Seat %s reserved for flight %s.\n", seat, flight);
    }

    gdbm_close(db);
}

/**
 * Automatically releases a seat in a flight's seat map
 * Called when a reservation is deleted or seat changed
 */
void release_seat(const char *flight, const char *seat)
{
    GDBM_FILE db = open_seatmap_db();
    if (!db)
        return;

    datum k = {(char *)flight, (int)strlen(flight)};
    datum v = gdbm_fetch(db, k);

    if (!v.dptr)
    {
        printf("No seat map found for flight %s.\n", flight);
        gdbm_close(db);
        return;
    }

    SeatMap sm;
    memcpy(&sm, v.dptr, sizeof(SeatMap));
    free(v.dptr);

    int row = atoi(seat) - 1;
    int col = -1;

    for (int i = 0; i < (int)strlen(seat); i++)
    {
        if (seat[i] >= 'A' && seat[i] <= 'Z')
        {
            col = seat[i] - 'A';
            break;
        }
    }

    if (col == -1)
    {
        col = 0;
    }

    if (row < 0 || row >= sm.rows || col < 0 || col >= sm.cols)
    {
        printf("Invalid seat %s for flight %s.\n", seat, flight);
        gdbm_close(db);
        return;
    }

    sm.seats[row][col] = 'O';
    datum new_val = {(char *)&sm, sizeof(SeatMap)};
    gdbm_store(db, k, new_val, GDBM_REPLACE);

    printf("Seat %s released for flight %s.\n", seat, flight);
    gdbm_close(db);
}
