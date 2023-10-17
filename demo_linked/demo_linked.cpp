#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#pragma comment(lib, "user32.lib")

int readChoice();
void clear();
int leapYear(int year);
int daysInMonth(int month, int year);
int isNumeric(const char* str);
int isValidText(const char* str);
int isValidDate(const char* date);
void printCentered(const char* text);
void saveAddresses(struct Address* head);
void editAddress(struct Address* head, struct Address* address);
void deleteAddress(struct Address** head, struct Address** tail, struct Address* address);
void displaySingleAddress(struct Address* address);
void displayAddress(struct Address* head);
void loadAddresses(struct Address** head, struct Address** tail);
void addAddress(struct Address** head, struct Address** tail);
void updateIDs(struct Address* head);
void bubbleSortByName(struct Address** head);
void bubbleSortByBirthday(struct Address** head);
int compareBirthdays(const char* date1, const char* date2);
void swapAddresses(struct Address** a, struct Address** b);

// Struktur für die Adressdaten
struct Address {
    int ID;
    char first_name[50];
    char last_name[50];
    char street[100];
    char city[50];
    char zip[10];
    char birthday[15];
    struct Address* next;
    struct Address* prev;
};

struct Address* head = NULL;
struct Address* tail = NULL;

int mainMenu() {
    int choice;
    loadAddresses(&head, &tail);
    int sortOption;

    while (1) {
        system("cls");

        printCentered("1. Neue Adresse hinzufuegen\n");
        printCentered("2. Adressliste anzeigen\n");
        printCentered("Esc Beenden\n");
        printCentered("Auswahl: ");

        choice = _getch();

        if (choice == 27) { // Prüfung auf ESC-Taste (ASCII-Code 27)
            break; // Beende die Schleife und das Programm
        }

        choice -= '0';

        switch (choice) {
        case 1:
            clear();
            addAddress(&head, &tail);
            break;
        case 2:
            clear();            
            saveAddresses(head); // Adressen in der sortierten Reihenfolge speichern
            displayAddress(head);
            break;
        default:
            printf("Ungueltige Auswahl. Bitte erneut versuchen.\n");
        }
    }

    return 1;
}

int main() {
    return mainMenu();
}

int readChoice() {
    while (1) {
        char choice[10];
        if (fgets(choice, sizeof(choice), stdin) != NULL) {
            int num;
            if (sscanf(choice, "%d", &num) == 1) {
                return num;
            }
            else {
                printf("Ungueltige Eingabe. Bitte geben Sie eine Nummer ein: ");
            }
        }
    }
}

void clear() {
#ifdef _WIN64
    system("cls");
#endif
}

int leapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    switch (month) {
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        return leapYear(year) ? 29 : 28;
    default:
        return 31;
    }
}

int isNumeric(const char* str) {
    while (*str) {
        if (*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

int isValidText(const char* str) {
    while (*str) {
        if (!(((*str >= 'A') && (*str <= 'Z')) || ((*str >= 'a') && (*str <= 'z')) || (*str == ' '))) {
            return 0; // Ungültiges Zeichen gefunden
        }
        str++;
    }
    return 1; // Alle Zeichen sind gültig
}

int isValidDate(const char* date) {
    char day[3], month[3], year[5];
    memset(day, 0, sizeof(day));
    memset(month, 0, sizeof(month));
    memset(year, 0, sizeof(year));

    if (sscanf(date, "%2[^.].%2[^.].%4s", day, month, year) != 3) {
        printf("Ungültiges Format.\n");
        return -1;
    }

    if (!isNumeric(day) || !isNumeric(month) || !isNumeric(year)) {
        if (!isNumeric(day))
            printf("Ungueltige Eingabe: Tag '%s' ist keine Zahl.\n", day);
        if (!isNumeric(month))
            printf("Ungueltige Eingabe: Monat '%s' ist keine Zahl.\n", month);
        if (!isNumeric(year))
            printf("Ungueltige Eingabe: Jahr '%s' ist keine Zahl.\n", year);

        return -1; // Ungültige Eingabe
    }

    int dayInt = atoi(day);
    int monthInt = atoi(month);
    int yearInt = atoi(year);

    if (monthInt < 1 || monthInt > 12) {
        printf("Ungültiger Monat: %d\n", monthInt);
        return -1;
    }

    if (dayInt < 1 || dayInt > daysInMonth(monthInt, yearInt)) {
        printf("Ungültiger Tag: %d\n", dayInt);
        return -1;
    }

    if (yearInt < 1) {
        printf("Ungültiges Jahr: %d\n", yearInt);
        return -1;
    }

    return 1;
}

void printCentered(const char* text) {
    int len = strlen(text);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    int padding = (columns - len) / 2;

    // Links auffüllen mit Leerzeichen
    for (int i = 0; i < padding; i++) {
        putchar(' ');
    }

    // Text ausgeben
    printf("%s", text);

    // Rechts auffüllen mit Leerzeichen
    for (int i = padding + len; i < columns; i++) {
        putchar(' ');
    }

    // Neue Zeile
    putchar('\n');
}

void saveAddresses(struct Address* head) {
    FILE* file = fopen("addresses.txt", "w");
    if (file == NULL) {
        printf("Fehler beim Oeffnen der Datei!\n");
        return;
    }

    struct Address* current = head;
    while (current != NULL) {
        fprintf(file, "%s,%s,%s,%s,%s,%s\n",
            current->last_name,
            current->first_name,
            current->street,
            current->city,
            current->zip,
            current->birthday);
        current = current->next;
    }

    fclose(file);
}

void deleteCharacter(int* i) {
    if (*i > 0) {
        (*i)--;
        putchar('\b');  // Backspace, um das Zeichen zu löschen
        putchar(' ');   // Leerzeichen, um das Zeichen zu überschreiben
        putchar('\b');  // Zurück zum Ausgangspunkt
    }
}

// Funktion zum Hinzufügen einer Adresse zur Liste
void addAddress(struct Address** head, struct Address** tail) {
    struct Address* new_address = (struct Address*)malloc(sizeof(struct Address));
    if (new_address == NULL) {
        printf("\nFehler: Speicher konnte nicht reserviert werden.\n");
        exit(1);
    }

    new_address->ID = 0;

    printf("--- Neue Adresse ---\n");
    char buffer[50];

    // Nachname
    do {
        printf("\nNachname: \n");
        fflush(stdin);
        int i = 0;
        while (1) {
            int ch = _getch();
            if (ch == VK_ESCAPE) {
                free(new_address);
                return;
            }
            else if (ch == '\r') {
                buffer[i] = '\0';
                break;
            }
            else if (ch == '\b') {  // Backspace
                deleteCharacter(&i);
            }
            else {
                buffer[i] = ch;
                i++;
                putchar(ch);
            }
        }
    } while (!isValidText(buffer));

    // Kopieren des Inhalts im Puffer in das Feld der Adresse
    strncpy(new_address->last_name, buffer, sizeof(new_address->last_name));

    // Vorname
    do {
        printf("\nVorname: \n");
        fflush(stdin);
        int i = 0;
        while (1) {
            int ch = _getch();
            if (ch == VK_ESCAPE) {
                free(new_address);
                return;
            }
            else if (ch == '\r') {
                buffer[i] = '\0';
                break;
            }
            else if (ch == '\b') {  // Backspace
                deleteCharacter(&i);
            }
            else {
                buffer[i] = ch;
                i++;
                putchar(ch);
            }
        }
    } while (!isValidText(buffer));
    strncpy(new_address->first_name, buffer, sizeof(new_address->first_name));

    // Straße
    printf("\nStrasse: \n");
    fflush(stdin);
    int i = 0;
    while (1) {
        int ch = _getch();
        if (ch == VK_ESCAPE) {
            free(new_address);
            return;
        }
        else if (ch == '\r') {
            buffer[i] = '\0';
            break;
        }
        else if (ch == '\b') {  // Backspace
            deleteCharacter(&i);
        }
        else {
            buffer[i] = ch;
            i++;
            putchar(ch);
        }
    }
    strncpy(new_address->street, buffer, sizeof(new_address->street));

    // Ort
    do {
        printf("\nOrt: \n");
        fflush(stdin);
        int i = 0;
        while (1) {
            int ch = _getch();
            if (ch == VK_ESCAPE) {
                free(new_address);
                return;
            }
            else if (ch == '\r') {
                buffer[i] = '\0';
                break;
            }
            else if (ch == '\b') {  // Backspace
                deleteCharacter(&i);
            }
            else {
                buffer[i] = ch;
                i++;
                putchar(ch);
            }
        }
    } while (!isValidText(buffer));
    strncpy(new_address->city, buffer, sizeof(new_address->city));

    // PLZ
    while (1) {
        printf("\nPLZ: \n");
        fflush(stdin);
        int i = 0;
        while (1) {
            int ch = _getch();
            if (ch == VK_ESCAPE) {
                free(new_address);
                return;
            }
            else if (ch == '\r') {
                buffer[i] = '\0';
                break;
            }
            else if (ch == '\b') {  // Backspace
                deleteCharacter(&i);
            }
            else {
                buffer[i] = ch;
                i++;
                putchar(ch);
            }
        }

        if (isNumeric(buffer) && strlen(buffer) == 5) {
            // Eingabe für PLZ ist gültig
            strncpy(new_address->zip, buffer, sizeof(new_address->zip));
            break;
        }
        else {
            printf("\nUngültige PLZ. Die PLZ muss aus 5 Ziffern bestehen.\n");
        }
    }

    // Geburtsdatum
    int day, month, year;
    while (1) {
        printf("\nGeburtsdatum (dd.MM.yyyy): \n");
        fflush(stdin);
        int i = 0;
        while (1) {
            int ch = _getch();
            if (ch == VK_ESCAPE) {
                free(new_address);
                return;
            }
            else if (ch == '\r') {
                buffer[i] = '\0';
                break;
            }
            else if (ch == '\b') {  // Backspace
                deleteCharacter(&i);
            }
            else {
                buffer[i] = ch;
                i++;
                putchar(ch);
            }
        }

        if (isValidDate(buffer)) {
            if (sscanf(buffer, "%d.%d.%d", &day, &month, &year) == 3) {
                if (year >= 0 && year <= 99) {
                    int currentYear = 2023;
                    if (year >= (currentYear % 100)) {
                        year += 1900;
                    }
                    else {
                        year += 2000;
                    }
                }
                // Datum richtig formatieren und speichern
                snprintf(new_address->birthday, sizeof(new_address->birthday), "%02d.%02d.%04d", day, month, year);
                break;
            }
            else {
                printf("\nUngültiges Geburtsdatum.\n");
            }
        }
    }

    // Fügen Sie die Adresse zur Liste hinzu und erhöhen Sie die ID
    new_address->ID = 0; // Setzen Sie die vorläufige ID auf 0
    if (*tail != NULL) {
        new_address->ID = (*tail)->ID + 1; // Erhöhen Sie die ID basierend auf der letzten Adresse
        (*tail)->next = new_address;
    }
    else {
        *head = new_address; // Wenn die Liste leer ist, setzen Sie die Adresse als erstes Element
    }
    new_address->prev = *tail;
    new_address->next = NULL;
    *tail = new_address;

    saveAddresses(*head); // Speichern der Liste in die Datei
}

// Funktion zum Bearbeiten einer Adresse aus der Liste
void editAddress(struct Address* head, struct Address* address) {
    int fieldChoice;
    int day, month, year;

    printf("\nDaten der Adresse %d bearbeiten:\n", address->ID);
    printf("1. Nachname\n");
    printf("2. Vorname\n");
    printf("3. Strasse\n");
    printf("4. Ort\n");
    printf("5. PLZ\n");
    printf("6. Geburtstag\n");
    printf("7. LOESCHEN\n");
    printf("Esc zum Beenden\n");
    printf("Bitte geben Sie die Nummer des zu bearbeitenden Feldes ein: ");

    while (1) {

        fieldChoice = _getch();

        if (fieldChoice == 27) { // Prüfung auf ESC-Taste
            clear();
            displayAddress(head); // Zurück zur Anzeige der Adressen
            return;
        }

        fieldChoice -= '0';

        switch (fieldChoice) {
        case 1:
            do {
                printf("\nBitte neuen Nachnamen eingeben: ");
                scanf("%s", address->last_name);
            } while (!isValidText(address->last_name));
            clear();
            saveAddresses(head);
            displayAddress(head);
            return;
        case 2:
            do {
                printf("\nBitte neuen Vornamen eingeben: ");
                scanf("%s", address->first_name);
            } while (!isValidText(address->first_name));
            clear();
            saveAddresses(head);
            displayAddress(head);
            return;
        case 3:
            printf("\nBitte neue Strasse eingeben: ");
            fgets(address->street, sizeof(address->street), stdin);
            strtok(address->street, "\n");
            clear();
            saveAddresses(head);
            displayAddress(head);
            return;
        case 4:
            do {
                printf("\nBitte neuen Ort eingeben: ");
                scanf("%s", address->city);
            } while (!isValidText(address->city));
            clear();
            saveAddresses(head);
            displayAddress(head);
            return;
        case 5:
            do {
                printf("\nBitte neue PLZ eingeben: ");
                scanf("%s", address->zip);
            } while (!isNumeric(address->zip) || strlen(address->zip) != 5);
            clear();
            saveAddresses(head);
            displayAddress(head);
            return;
        case 6:
            while (1) {
                char input[10];
                printf("\nBitte neuen Geburtstag eingeben (dd.MM.yyyy): ");
                fgets(input, sizeof(input), stdin);
                strtok(input, "\n"); // Zeilenumbruch entfernen

                if (sscanf(input, "%d.%d.%d", &day, &month, &year) == 3 && isValidDate(input)) {
                    // Prüfen und Korrektur von verkürzten Jahren
                    if (year >= 0 && year <= 99) {
                        int currentYear = 2023;
                        if (year >= (currentYear % 100)) {
                            year += 1900;
                        }
                        else {
                            year += 2000;
                        }
                    }

                    // Datum richtig formatieren und speichern
                    snprintf(address->birthday, sizeof(address->birthday), "%02d.%02d.%04d", day, month, year);

                    break; // Die Schleife beenden, wenn die Eingabe gültig ist
                }
                else {
                    printf("\nUngueltiges Geburtsdatum.\n");
                }
            }
            clear();
            saveAddresses(head);
            //displayAddress(head);
            return;
        case 7:
            deleteAddress(&head, &tail, address);
            clear();
            saveAddresses(head);
            displayAddress(head);
            return;

        default:
            printf("\nUngueltige Auswahl.\n");
        }
        clear();
        saveAddresses(head);
        // Die Schleife läuft weiter, bis die Bearbeitung abgeschlossen ist oder der Benutzer die ESC-Taste drückt.
    }
}

// Funktion zum Löschen einer Adresse aus der Liste
void deleteAddress(struct Address** head, struct Address** tail, struct Address* address) {
    if (*head == NULL || address == NULL) {
        printf("Fehler: Adresse oder Liste ist ungültig.\n");
        return;
    }

    if (address == *head) {
        *head = address->next;
        if (*head != NULL) {
            (*head)->prev = NULL;
        }
    }
    else {
        if (address->prev != NULL) {
            address->prev->next = address->next;
        }
        if (address->next != NULL) {
            address->next->prev = address->prev;
        }
    }

    if (address == *tail) {
        *tail = address->prev;
    }

    free(address);
    updateIDs(*head);
    saveAddresses(*head);
}

// Funktion zum Anzeigen der Liste
void displayAddress(struct Address* head) {
    if (head == NULL) {
        printCentered("Die Adressliste ist leer.\n");
        clear();
        return;
    }

    printCentered("--- Adressliste ---\n");
    printf("\n%4s | %15s | %15s | %20s | %15s | %10s | %11s\n", "ID", "Nachname", "Vorname", "Strasse", "Ort", "PLZ", "Geburtstag");
    printf("-------------------------------------------------------------------------------"
        "----------------------------------\n");

    struct Address* current = head;
    int index = 1;

    while (current != NULL) {
        printf("%4d | %15s | %15s | %20s | %15s | %10s | %11s\n",
            current->ID,
            current->last_name,
            current->first_name,
            current->street,
            current->city,
            current->zip,
            current->birthday);

        current = current->next;
        index++;
    }

    while (1) {
        printf("Zum Bearbeiten entsprechenden Index eingeben. (Esc zum Abbrechen)\n");
        int choice = 0;
        int digit;
        while (1) {
            int ch = _getch();
            if (ch == 27) {  // ASCII-Code für ESC
                clear();
                return;  // Funktion beenden und zur Hauptauswahl zurückkehren
            }
            else if (isdigit(ch)) {
                digit = ch - '0';
                choice = choice * 10 + digit;
                putchar(ch); // Zeichen anzeigen
            }
            else if (ch == '\r') { // Enter-Taste, Auswahl beenden
                if (choice > 0 && choice <= index - 1) {
                    current = head;
                    for (int i = 1; i < choice; i++)
                        current = current->next;
                    // ausgewählten Datensatz an die editAddress-Funktion übergeben
                    editAddress(head, current);
                    return; // Zur Hauptauswahl zurückkehren
                }
                else {
                    printf("\nUngültige Auswahl. Erneut eingeben oder ESC zum Abbrechen drücken.\n");
                    choice = 0; // Zurücksetzen der Auswahl
                }
            }
        }
    }
}

void loadAddresses(struct Address** head, struct Address** tail) {
    FILE* file = fopen("addresses.txt", "r");
    if (file == NULL) {
        printf("Fehler beim Oeffnen der Datei!\n");
        return;
    }

    // Löschen aller vorhandenen Adressen in der Liste
    while (*head != NULL) {
        struct Address* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    *tail = NULL;

    int nextID = 1; // Start-ID für bereits gespeicherte Adressen

    while (1) {
        struct Address* new_address = (struct Address*)malloc(sizeof(struct Address));
        if (new_address == NULL) {
            printf("Fehler: Speicher konnte nicht reserviert werden.\n");
            exit(1); // Beenden Sie das Programm mit einem Fehlercode
        }

        if (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]\n",
            new_address->last_name,
            new_address->first_name,
            new_address->street,
            new_address->city,
            new_address->zip,
            new_address->birthday) != 6) {
            free(new_address); // Freigeben des nicht verwendeten Speichers
            break;
        }

        new_address->next = NULL;
        new_address->prev = *tail;

        if (*tail != NULL) {
            (*tail)->next = new_address;
        }
        *tail = new_address;

        if (*head == NULL) {
            *head = new_address;
        }

        new_address->ID = nextID;
        nextID++; // Erhöhen der ID für die nächste Adresse
    }

    fclose(file);
}

void updateIDs(struct Address* head) {
    int id = 1;
    struct Address* current = head;

    while (current != NULL) {
        current->ID = id;
        id++;
        current = current->next;
    }
}
void displaySingleAddress(struct Address* address) {
    printf("%4d | %15s | %15s | %20s | %15s | %10s | %11s\n",
        address->ID, address->last_name, address->first_name,
        address->street, address->city, address->zip, address->birthday);
}