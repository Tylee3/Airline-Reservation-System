#include <stdio.h>
#include <stdlib.h>
#include "reservation.h"
#include "seatmap.h"

/**
 * Main function - Entry point of the Seat Reservation System
 * Loads existing data and presents menu-driven interface to user
 * Handles user input and calls appropriate functions
 */
int main()
{
    int choice;

    load_reservations(); // Load existing data at startup

    while (1)
    {
        printf("\n=== Seat Reservation System ===\n");
        printf("1. Add Reservation\n");
        printf("2. View Reservations\n");
        printf("3. Update Reservation\n");
        printf("4. Delete Reservation\n");
        printf("5. Search Reservations\n");
        printf("6. View Seat Map\n");
        printf("0. Exit\n");
        printf("Choice: ");

        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice)
        {
        case 1:
            add_reservation();
            break;
        case 2:
            view_reservations();
            break;
        case 3:
            update_reservation();
            break;
        case 4:
            delete_reservation();
            break;
        case 5:
            search_reservations();
            break;
        case 6:
            view_seatmap();
            break;
        case 0:
            printf("Goodbye!\n");
            exit(0);
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}
