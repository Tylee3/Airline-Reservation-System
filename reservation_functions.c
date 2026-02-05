#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reservation.h"
#include "seatmap.h"

Reservation reservations[MAX_RESERVATIONS];
int reservation_count = 0;
int next_id = 1;

/**
 * Opens the GDBM database
 * Creates it if it doesn't exist
 */
static GDBM_FILE open_db()
{
    GDBM_FILE db = gdbm_open(DB_FILE, 0, GDBM_WRCREAT, 0644, 0);
    if (!db)
    {
        printf("Error: Could not open database file '%s'.\n", DB_FILE);
    }
    return db;
}

/**
 * Closes the GDBM database
 */
static void close_db(GDBM_FILE db)
{
    if (db)
    {
        gdbm_close(db);
    }
}

/**
 * Generates a database key for a reservation record
 */
static void make_key(char *buffer, int id)
{
    sprintf(buffer, "R:%d", id);
}

/**
 * Saves all reservation records to the GDBM database
 * Each record is stored under key as a binary struct
 */
void save_reservations()
{
    GDBM_FILE db = open_db();
    if (!db)
        return;

    for (int i = 0; i < reservation_count; i++)
    {
        char key[32];
        make_key(key, reservations[i].id);

        datum k = {key, (int)strlen(key)};
        datum v = {(char *)&reservations[i], sizeof(Reservation)};

        gdbm_store(db, k, v, GDBM_REPLACE);
    }

    close_db(db);
}

/**
 * Loads all reservations from the GDBM database into memory
 */
void load_reservations()
{
    reservation_count = 0;
    next_id = 1;

    GDBM_FILE db = open_db();
    if (!db)
        return;

    datum key = gdbm_firstkey(db);
    while (key.dptr != NULL)
    {
        datum value = gdbm_fetch(db, key);

        if (value.dptr != NULL && value.dsize == sizeof(Reservation))
        {
            if (reservation_count < MAX_RESERVATIONS)
            {
                memcpy(&reservations[reservation_count], value.dptr, sizeof(Reservation));

                if (reservations[reservation_count].id >= next_id)
                    next_id = reservations[reservation_count].id + 1;

                reservation_count++;
            }
            free(value.dptr);
        }

        datum nextkey = gdbm_nextkey(db, key);
        free(key.dptr);
        key = nextkey;
    }

    close_db(db);
}

/**
 * Adds a new seat reservation
 * Automatically creates or updates seat map for that flight
 */
void add_reservation()
{
    if (reservation_count >= MAX_RESERVATIONS)
    {
        printf("Reservation list full!\n");
        return;
    }

    Reservation r;
    r.id = next_id++;

    printf("Passenger Name: ");
    fgets(r.passenger_name, MAX_STRING, stdin);
    r.passenger_name[strcspn(r.passenger_name, "\n")] = 0;

    printf("Flight Number: ");
    fgets(r.flight_number, MAX_STRING, stdin);
    r.flight_number[strcspn(r.flight_number, "\n")] = 0;

    printf("Flight Date (MM/DD/YYYY): ");
    fgets(r.flight_date, MAX_STRING, stdin);
    r.flight_date[strcspn(r.flight_date, "\n")] = 0;

    printf("Seat Number: ");
    fgets(r.seat_number, sizeof(r.seat_number), stdin);
    r.seat_number[strcspn(r.seat_number, "\n")] = 0;

    strcpy(r.status, "Confirmed");

    GDBM_FILE db = open_db();
    if (!db)
        return;

    char key[32];
    make_key(key, r.id);

    datum k = {key, (int)strlen(key)};
    datum v = {(char *)&r, sizeof(Reservation)};

    if (gdbm_store(db, k, v, GDBM_REPLACE) == 0)
        printf("Reservation added successfully! (ID: %d)\n", r.id);
    else
        printf("Error saving reservation.\n");

    close_db(db);
    reserve_seat(r.flight_number, r.seat_number);
    load_reservations();
}

/**
 * Displays all reservations in a formatted table
 */
void view_reservations()
{
    load_reservations();

    if (reservation_count == 0)
    {
        printf("No reservations found.\n");
        return;
    }

    printf("\n%-4s %-25s %-12s %-12s %-8s %-12s\n",
           "ID", "Passenger", "Flight", "Date", "Seat", "Status");
    printf("%-4s %-25s %-12s %-12s %-8s %-12s\n",
           "----", "-------------------------", "------------", "------------", "--------", "------------");

    for (int i = 0; i < reservation_count; i++)
    {
        printf("%-4d %-25.25s %-12.12s %-12.12s %-8.8s %-12.12s\n",
               reservations[i].id,
               reservations[i].passenger_name,
               reservations[i].flight_number,
               reservations[i].flight_date,
               reservations[i].seat_number,
               reservations[i].status);
    }
}

/**
 * Updates an existing reservation by ID
 * Automatically updates seat map if seat or flight changes
 */
void update_reservation()
{
    load_reservations();
    if (reservation_count == 0)
    {
        printf("No reservations to update.\n");
        return;
    }

    int id;
    printf("Enter reservation ID: ");
    scanf("%d", &id);
    getchar();

    int index = -1;
    for (int i = 0; i < reservation_count; i++)
    {
        if (reservations[i].id == id)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        printf("Reservation with ID %d not found.\n", id);
        return;
    }

    Reservation *r = &reservations[index];
    char old_flight[MAX_STRING];
    char old_seat[8];
    strcpy(old_flight, r->flight_number);
    strcpy(old_seat, r->seat_number);

    char temp[MAX_STRING];

    printf("Current Passenger: %s\nNew Passenger: ", r->passenger_name);
    fgets(temp, MAX_STRING, stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(r->passenger_name, temp);
    }

    printf("Current Seat: %s\nNew Seat: ", r->seat_number);
    fgets(temp, MAX_STRING, stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(r->seat_number, temp);
    }

    printf("Current Flight: %s\nNew Flight: ", r->flight_number);
    fgets(temp, MAX_STRING, stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(r->flight_number, temp);
    }

    GDBM_FILE db = open_db();
    if (!db)
        return;

    char key[32];
    make_key(key, r->id);
    datum k = {key, (int)strlen(key)};
    datum v = {(char *)r, sizeof(Reservation)};

    if (gdbm_store(db, k, v, GDBM_REPLACE) == 0)
        printf("Reservation updated successfully!\n");
    else
        printf("Error updating reservation.\n");

    close_db(db);

    if (strcmp(old_flight, r->flight_number) != 0 || strcmp(old_seat, r->seat_number) != 0)
    {
        release_seat(old_flight, old_seat);
        reserve_seat(r->flight_number, r->seat_number);
    }

    load_reservations();
}

/**
 * Deletes a reservation by ID
 * Automatically frees the seat in the seat map
 */
void delete_reservation()
{
    load_reservations();
    if (reservation_count == 0)
    {
        printf("No reservations to delete.\n");
        return;
    }

    int id;
    printf("Enter reservation ID: ");
    scanf("%d", &id);
    getchar();

    for (int i = 0; i < reservation_count; i++)
    {
        if (reservations[i].id == id)
        {
            release_seat(reservations[i].flight_number, reservations[i].seat_number);
            break;
        }
    }

    char key[32];
    make_key(key, id);
    datum k = {key, (int)strlen(key)};

    GDBM_FILE db = open_db();
    if (!db)
        return;

    if (gdbm_delete(db, k) == 0)
        printf("Reservation deleted successfully!\n");
    else
        printf("Reservation with ID %d not found.\n", id);

    close_db(db);
    load_reservations();
}

/**
 * Searches for reservations by passenger name, flight, or date
 */
void search_reservations()
{
    load_reservations();
    if (reservation_count == 0)
    {
        printf("No reservations found.\n");
        return;
    }

    char search_term[MAX_STRING];
    printf("Enter search term (passenger/flight/date): ");
    fgets(search_term, MAX_STRING, stdin);
    search_term[strcspn(search_term, "\n")] = 0;

    int found = 0;

    printf("\n%-4s %-25s %-12s %-12s %-8s %-12s\n",
           "ID", "Passenger", "Flight", "Date", "Seat", "Status");
    printf("%-4s %-25s %-12s %-12s %-8s %-12s\n",
           "----", "-------------------------", "------------", "------------", "--------", "------------");

    for (int i = 0; i < reservation_count; i++)
    {
        if (strstr(reservations[i].passenger_name, search_term) ||
            strstr(reservations[i].flight_number, search_term) ||
            strstr(reservations[i].flight_date, search_term))
        {
            printf("%-4d %-25.25s %-12.12s %-12.12s %-8.8s %-12.12s\n",
                   reservations[i].id,
                   reservations[i].passenger_name,
                   reservations[i].flight_number,
                   reservations[i].flight_date,
                   reservations[i].seat_number,
                   reservations[i].status);
            found++;
        }
    }

    if (found == 0)
        printf("No reservations found matching '%s'.\n", search_term);
    else
        printf("\nFound %d reservation(s).\n", found);
}
