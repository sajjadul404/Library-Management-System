#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <stdbool.h>

#define MAX_BOOKS 5000
#define MAX_USERS 1000

struct Book {
    int id;
    char title[100];
    char author[100];
    bool isBorrowed;
};

struct User {
    char username[50];
    char password[50];
    char role[20];
};

struct Book library[MAX_BOOKS];
int bookCount = 0;
struct User users[MAX_USERS];
int userCount = 0;
struct User currentUser;

const char* libraryFile = "library.txt";
const char* userFile = "users.txt";

/// UI Functions
void SetColor(int ForgC) {
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
        wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
}

void ClearConsoleToColors(int ForgC, int BackC) {
    WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    SetConsoleTextAttribute(hStdOut, wColor);
    if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
        FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
        FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
        SetConsoleCursorPosition(hStdOut, coord);
    }
}

void SetColorAndBackground(int ForgC, int BackC) {
    WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
}

void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void drawBox(int left, int top, int right, int bottom) {
    int i;

    // Draw top border
    gotoxy(left, top);
    printf("%c", 201);  // Top-left corner
    for (i = left + 1; i < right; i++) {
        printf("%c", 205);  // Horizontal line
    }
    printf("%c", 187);  // Top-right corner

    // Draw side borders
    for (i = top + 1; i < bottom; i++) {
        gotoxy(left, i);
        printf("%c", 186);  // Left vertical line
        gotoxy(right, i);
        printf("%c", 186);  // Right vertical line
    }

    // Draw bottom border
    gotoxy(left, bottom);
    printf("%c", 200);  // Bottom-left corner
    for (i = left + 1; i < right; i++) {
        printf("%c", 205);  // Horizontal line
    }
    printf("%c", 188);  // Bottom-right corner
}

void clearMainBox() {
    int i,j;
    for(i = 1; i < 78; i++) {
        for(j = 7; j < 25; j++) {
            gotoxy(i,j);printf(" ");
        }
    }
}

void clearHeaderBox() {
    int i,j;
    for(i = 1; i < 78; i++) {
        for(j = 1; j < 6; j++) {
            gotoxy(i,j);printf(" ");
        }
    }
}

void window() {
    // Clear screen
    system("cls");

    // Draw header box (smaller, top)
    drawBox(0, 0, 78, 5);

    // Draw main box (larger, bottom)
    drawBox(0, 6, 78, 25);

    // Add header information
    gotoxy(28,1);
    SetColor(1);
    printf("LIBRARY MANAGEMENT SYSTEM");
    gotoxy(17,2);
    printf("Bangladesh University of Business and Technology");
    gotoxy(17,3);
    printf("Plot #77-78 Road #9, Rupnagar,Mirpur-2, Dhaka-1216");
    gotoxy(25,24);
    SetColor(17);
}

void print_heading(const char st[]) {
    gotoxy(2,7);printf("SRS : %s",st);
    SetColorAndBackground(17,15);
}

void loadingAnimation() {
    gotoxy(35,12);printf("Loading");
    for(int i = 0; i < 3; i++) {
        printf(".");
        Sleep(300);
    }
    gotoxy(35,12);printf("       ");
}

/// Book Functions
void loadBooks() {
    FILE *file = fopen(libraryFile, "r");
    if (file == NULL) return;

    bookCount = 0;
    while(fscanf(file, "%d|%[^|]|%[^|]|%d\n",
        &library[bookCount].id,
        library[bookCount].title,
        library[bookCount].author,
        &library[bookCount].isBorrowed) == 4) {
        bookCount++;
    }
    fclose(file);
}

void saveBooks() {
    FILE *file = fopen(libraryFile, "w");
    if (file == NULL) return;

    for(int i = 0; i < bookCount; i++) {
        fprintf(file, "%d|%s|%s|%d\n",
            library[i].id,
            library[i].title,
            library[i].author,
            library[i].isBorrowed);
    }
    fclose(file);
}

void loadUsers() {
    FILE *file = fopen(userFile, "r");
    if (file == NULL) return;

    userCount = 0;
    while(fscanf(file, "%[^|]|%[^|]|%[^\n]\n",
        users[userCount].username,
        users[userCount].password,
        users[userCount].role) == 3) {
        userCount++;
    }
    fclose(file);
}

void saveUsers() {
    FILE *file = fopen(userFile, "w");
    if (file == NULL) return;

    for(int i = 0; i < userCount; i++) {
        fprintf(file, "%s|%s|%s\n",
            users[i].username,
            users[i].password,
            users[i].role);
    }
    fclose(file);
}

void displayBooks() {
    clearMainBox();
    print_heading("All Books");

    if (bookCount == 0) {
        gotoxy(2,9);printf("No books in the library.");
        return;
    }

    gotoxy(2,9);printf("+------+----------------------+----------------------+-------------+");
    gotoxy(2,10);printf("| ID   | Title                | Author               | Status      |");
    gotoxy(2,11);printf("+------+----------------------+----------------------+-------------+");

    for(int i = 0; i < bookCount; i++) {
        gotoxy(2,12+i);
        printf("| %-4d | %-20s | %-20s | %-11s |",
            library[i].id,
            library[i].title,
            library[i].author,
            library[i].isBorrowed ? "Borrowed" : "Available");
    }

    gotoxy(2,12+bookCount);printf("+------+----------------------+----------------------+-------------+");
}

void searchBook() {
    clearMainBox();
    print_heading("Search Book");

    char searchTitle[100];
    gotoxy(2,9);printf("Enter title to search: ");
    fflush(stdin);
    gets(searchTitle);

    bool found = false;
    for(int i = 0; i < bookCount; i++) {
        if(strstr(library[i].title, searchTitle) != NULL) {
            if(!found) {
                gotoxy(2,11);printf("+------+----------------------+----------------------+-------------+");
                gotoxy(2,12);printf("| ID   | Title                | Author               | Status      |");
                gotoxy(2,13);printf("+------+----------------------+----------------------+-------------+");
            }

            gotoxy(2,14+found);
            printf("| %-4d | %-20s | %-20s | %-11s |",
                library[i].id,
                library[i].title,
                library[i].author,
                library[i].isBorrowed ? "Borrowed" : "Available");
            found = true;
        }
    }

    if(found) {
        gotoxy(2,14+found);printf("+------+----------------------+----------------------+-------------+");
    } else {
        gotoxy(2,11);printf("Book not found.");
    }
}

void addBook() {
    clearMainBox();
    print_heading("Add Book");

    if(strcmp(currentUser.role, "teacher") != 0) {
        gotoxy(2,9);printf("Only teachers can add books.");
        return;
    }

    struct Book newBook;
    gotoxy(2,9);printf("Enter book ID: ");
    scanf("%d", &newBook.id);
    fflush(stdin);

    gotoxy(2,11);printf("Enter book title: ");
    gets(newBook.title);

    gotoxy(2,13);printf("Enter book author: ");
    gets(newBook.author);

    newBook.isBorrowed = false;
    library[bookCount++] = newBook;
    saveBooks();

    gotoxy(2,15);printf("Book added successfully!");
}

void borrowBook() {
    clearMainBox();
    print_heading("Borrow Book");

    int id;
    gotoxy(2,9);printf("Enter book ID to borrow: ");
    scanf("%d", &id);

    for(int i = 0; i < bookCount; i++) {
        if(library[i].id == id) {
            if(library[i].isBorrowed) {
                gotoxy(2,11);printf("Book is already borrowed.");
            } else {
                library[i].isBorrowed = true;
                saveBooks();
                gotoxy(2,11);printf("You borrowed: %s", library[i].title);
            }
            return;
        }
    }

    gotoxy(2,11);printf("Book not found.");
}

void acceptBook() {
    clearMainBox();
    print_heading("Accept Return");

    if(strcmp(currentUser.role, "teacher") != 0) {
        gotoxy(2,9);printf("Only teachers can accept returned books.");
        return;
    }

    int id;
    gotoxy(2,9);printf("Enter book ID to accept return: ");
    scanf("%d", &id);

    for(int i = 0; i < bookCount; i++) {
        if(library[i].id == id) {
            if(!library[i].isBorrowed) {
                gotoxy(2,11);printf("This book was not borrowed.");
            } else {
                library[i].isBorrowed = false;
                saveBooks();
                gotoxy(2,11);printf("Book return accepted: %s", library[i].title);
            }
            return;
        }
    }

    gotoxy(2,11);printf("Book not found.");
}

void removeBook() {
    clearMainBox();
    print_heading("Remove Book");

    if(strcmp(currentUser.role, "teacher") != 0) {
        gotoxy(2,9);printf("Only teachers can remove books.");
        return;
    }

    int id;
    gotoxy(2,9);printf("Enter book ID to remove: ");
    scanf("%d", &id);

    for(int i = 0; i < bookCount; i++) {
        if(library[i].id == id) {
            gotoxy(2,11);printf("Removing: %s", library[i].title);
            for(int j = i; j < bookCount-1; j++) {
                library[j] = library[j+1];
            }
            bookCount--;
            saveBooks();
            gotoxy(2,13);printf("Book removed successfully.");
            return;
        }
    }

    gotoxy(2,11);printf("Book not found.");
}

/// User Functions
bool userExists(const char* username, const char* role) {
    for(int i = 0; i < userCount; i++) {
        if(strcmp(users[i].username, username) == 0 && strcmp(users[i].role, role) == 0) {
            return true;
        }
    }
    return false;
}

bool validateLogin(const char* username, const char* password, const char* role) {
    for(int i = 0; i < userCount; i++) {
        if(strcmp(users[i].username, username) == 0 &&
           strcmp(users[i].password, password) == 0 &&
           strcmp(users[i].role, role) == 0) {
            return true;
        }
    }
    return false;
}

void registerUser() {
    clearMainBox();
    print_heading("Register");

    int roleChoice;
    gotoxy(2,9);printf("Select role:");
    gotoxy(2,11);printf("1. Teacher");
    gotoxy(2,12);printf("2. Student");
    gotoxy(2,13);printf("Enter choice (1 or 2): ");
    scanf("%d", &roleChoice);

    char role[20];
    if(roleChoice == 1) strcpy(role, "teacher");
    else if(roleChoice == 2) strcpy(role, "student");
    else {
        gotoxy(2,15);printf("Invalid role! Registration aborted.");
        return;
    }

    char username[50], password[50];
    gotoxy(2,15);printf("Enter username: ");
    fflush(stdin);
    gets(username);

    if(userExists(username, role)) {
        gotoxy(2,17);printf("Username already exists!");
        return;
    }

    gotoxy(2,17);printf("Enter password: ");
    gets(password);

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    strcpy(users[userCount].role, role);
    userCount++;
    saveUsers();

    gotoxy(2,19);printf("Registration successful for %s!", role);
}

bool loginUser() {
    clearMainBox();
    print_heading("Login");

    int roleChoice;
    gotoxy(2,9);printf("Select role:");
    gotoxy(2,11);printf("1. Teacher");
    gotoxy(2,12);printf("2. Student");
    gotoxy(2,13);printf("Enter choice (1 or 2): ");
    scanf("%d", &roleChoice);

    char role[20];
    if(roleChoice == 1) strcpy(role, "teacher");
    else if(roleChoice == 2) strcpy(role, "student");
    else {
        gotoxy(2,15);printf("Invalid role!");
        return false;
    }

    char username[50], password[50];
    gotoxy(2,15);printf("Enter username: ");
    fflush(stdin);
    gets(username);

    gotoxy(2,17);printf("Enter password: ");
    gets(password);

    if(validateLogin(username, password, role)) {
        strcpy(currentUser.username, username);
        strcpy(currentUser.password, password);
        strcpy(currentUser.role, role);
        gotoxy(2,19);printf("Login successful. Welcome, %s (%s)!", username, role);
        return true;
    } else {
        gotoxy(2,19);printf("Invalid credentials!");
        return false;
    }
}

/// Main Menu Functions
void authMenu() {
    int choice;
    bool isAuthenticated = false;

    do {
        clearMainBox();
        print_heading("Welcome");

        gotoxy(2,9);printf("1. Register");
        gotoxy(2,11);printf("2. Login");
        gotoxy(2,13);printf("3. Exit");
        gotoxy(2,15);printf("Choose an option: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                registerUser();
                break;
            case 2:
                isAuthenticated = loginUser();
                break;
            case 3:
                exit(0);
            default:
                gotoxy(2,17);printf("Invalid choice. Try again.");
        }

        if(!isAuthenticated && choice != 3) {
            gotoxy(2,19);printf("Press any key to continue...");
            getch();
        }
    } while(!isAuthenticated);

    loadingAnimation();
    loadBooks();
}

void mainMenu() {
    int choice;

    do {
        clearMainBox();
        print_heading("Main Menu");

        gotoxy(2,9);printf("Logged in as: %s (%s)", currentUser.username, currentUser.role);
        gotoxy(2,11);printf("1. Display All Books");
        gotoxy(2,12);printf("2. Search Book by Title");
        gotoxy(2,13);printf("3. Borrow Book");

        if(strcmp(currentUser.role, "teacher") == 0) {
            gotoxy(2,14);printf("4. Add Book");
            gotoxy(2,15);printf("5. Remove Book");
            gotoxy(2,16);printf("6. Accept Returned Book");
            gotoxy(2,17);printf("7. Exit");
        } else {
            gotoxy(2,14);printf("4. Exit");
        }

        gotoxy(2,19);printf("Choose an option: ");
        scanf("%d", &choice);

        if(strcmp(currentUser.role, "teacher") == 0) {
            switch(choice) {
                case 1: displayBooks(); break;
                case 2: searchBook(); break;
                case 3: borrowBook(); break;
                case 4: addBook(); break;
                case 5: removeBook(); break;
                case 6: acceptBook(); break;
                case 7: exit(0);
                default: gotoxy(2,21);printf("Invalid choice. Try again.");
            }
        } else {
            switch(choice) {
                case 1: displayBooks(); break;
                case 2: searchBook(); break;
                case 3: borrowBook(); break;
                case 4: exit(0);
                default: gotoxy(2,21);printf("Invalid choice. Try again.");
            }
        }

        if((strcmp(currentUser.role, "teacher") == 0 && choice != 7) ||
           (strcmp(currentUser.role, "student") == 0 && choice != 4)) {
            gotoxy(2,21);printf("Press any key to continue...");
            getch();
        }
    } while(1);
}

int main() {
    ClearConsoleToColors(17,15);
    SetConsoleTitle("BUBT LIBRARY MANAGEMENT SYSTEM");
    window();

    loadUsers();
    authMenu();
    mainMenu();

    return 0;
}
