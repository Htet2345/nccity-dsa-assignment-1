#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define MAX_USERS 100
#define NAME_CAPACITY 100
#define ADDRESS_CAPACITY 200

typedef struct {
    int id;
    char name[NAME_CAPACITY];
    int age;
    char address[ADDRESS_CAPACITY];
} User;

static User users[MAX_USERS];
static int userCount = 0;
static int nextId = 1;
static char input[ADDRESS_CAPACITY];
static int number;
static int endOfInput = 0;

/* Read and consume an entire line, even if it is too long. */
static int readLine(int capacity)
{
    int ch;
    int length = 0;
    int tooLong = 0;
    int start = 0;
    int i;

    fflush(stdout);
    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (length < capacity - 1) {
            input[length++] = (char)ch;
        } else {
            tooLong = 1;
        }
        if (ch == '\0') {
            tooLong = 1; /* A text field cannot contain an embedded null. */
        }
    }
    input[length] = '\0';
    if (ch == EOF && length == 0 && !tooLong) {
        endOfInput = 1;
        return 0;
    }
    if (tooLong) {
        printf("Input too long! Maximum %d bytes. Try again!\n", capacity - 1);
        return -1;
    }

    /* Trim outside whitespace; preserve spaces within the field. */
    while (start < length && isspace((unsigned char)input[start])) {
        start++;
    }
    while (length > start && isspace((unsigned char)input[length - 1])) {
        length--;
    }
    for (i = start; i < length; i++) {
        input[i - start] = input[i];
    }
    input[length - start] = '\0';
    if (input[0] == '\0') {
        printf("Input cannot be blank! Try again!\n");
        return -1;
    }
    return 1;
}

static int readText(int capacity)
{
    int result;
    do {
        result = readLine(capacity);
    } while (result == -1);
    return result;
}

/* Convert digits with a loop, checking overflow before multiplying. */
static int readNumber(void)
{
    int i;
    int value;
    int sign;
    int valid;
    int digit;

    while (readText(ADDRESS_CAPACITY)) {
        i = 0;
        value = 0;
        sign = 1;
        valid = 1;
        if (input[i] == '-' || input[i] == '+') {
            if (input[i] == '-') {
                sign = -1;
            }
            i++;
        }
        if (input[i] == '\0') {
            valid = 0;
        }
        for (; input[i] != '\0'; i++) {
            if (input[i] < '0' || input[i] > '9') {
                valid = 0;
                break;
            }
            digit = input[i] - '0';
            if (value > (INT_MAX - digit) / 10) {
                valid = 0;
                break;
            }
            value = value * 10 + digit;
        }
        if (valid) {
            number = sign * value;
            return 1;
        }
        printf("Please enter a whole number within -%d to %d. Try again!\n",
               INT_MAX, INT_MAX);
    }
    return 0;
}

static int readAge(void)
{
    while (!endOfInput) {
        printf("Age (1-150): ");
        if (!readNumber()) {
            return 0;
        }
        if (number >= 1 && number <= 150) {
            return 1;
        }
        printf("Age must be between 1 and 150! Try again!\n");
    }
    return 0;
}

static int findUserIndex(int id)
{
    int i;
    for (i = 0; i < userCount; i++) {
        if (users[i].id == id) {
            return i;
        }
    }
    return -1;
}

static int requestUserIndex(void)
{
    int index;
    while (!endOfInput) {
        printf("ID: ");
        if (!readNumber()) {
            return -1;
        }
        index = findUserIndex(number);
        if (index != -1) {
            return index;
        }
        printf("Your provided id not found! Try again!\n");
    }
    return -1;
}

static void createUser(void)
{
    User newUser;
    printf("Create User Page\n");
    if (userCount == MAX_USERS) {
        printf("User capacity reached! Maximum 100 users.\n");
        return;
    }
    if (nextId == INT_MAX) {
        printf("No more IDs available!\n");
        return;
    }
    printf("Name: ");
    if (!readText(NAME_CAPACITY)) {
        return;
    }
    strcpy(newUser.name, input);
    if (!readAge()) {
        return;
    }
    newUser.age = number;
    printf("Address: ");
    if (!readText(ADDRESS_CAPACITY)) {
        return;
    }
    strcpy(newUser.address, input);
    newUser.id = nextId;
    users[userCount] = newUser;
    userCount++;
    nextId++;
    printf("User created successfully! ID: %d\n", newUser.id);
}

static void readAllUsers(void)
{
    int i;
    printf("Reading All Users\n");
    if (userCount == 0) {
        printf("No users found!\n");
        return;
    }
    for (i = 0; i < userCount; i++) {
        printf("ID: %d\nName: %s\nAge: %d\nAddress: %s\n\n",
               users[i].id, users[i].name, users[i].age, users[i].address);
    }
}

static void updateUser(void)
{
    int index;
    User replacement;
    printf("Update User Page\n");
    if (userCount == 0) {
        printf("No users found!\n");
        return;
    }
    index = requestUserIndex();
    if (index == -1) {
        return;
    }
    printf("User found! Going to update...\n");
    replacement.id = users[index].id;
    printf("Name: ");
    if (!readText(NAME_CAPACITY)) {
        return;
    }
    strcpy(replacement.name, input);
    if (!readAge()) {
        return;
    }
    replacement.age = number;
    printf("Address: ");
    if (!readText(ADDRESS_CAPACITY)) {
        return;
    }
    strcpy(replacement.address, input);
    /* Commit only after all replacement fields have been read. */
    users[index] = replacement;
    printf("User updated successfully! ID: %d\n", replacement.id);
}

static void deleteUser(void)
{
    int index;
    int id;
    int i;
    printf("Delete User Page\n");
    if (userCount == 0) {
        printf("No users found!\n");
        return;
    }
    index = requestUserIndex();
    if (index == -1) {
        return;
    }
    id = users[index].id;
    for (i = index; i < userCount - 1; i++) {
        users[i] = users[i + 1];
    }
    userCount--;
    printf("User deleted successfully! ID: %d\n", id);
}

int main(void)
{
    int option;
    while (!endOfInput) {
        printf("\n1. Create User\n2. Read All User\n3. Update User\n"
               "4. Delete User\n5. Exit\nChoose an option: ");
        if (!readNumber()) {
            break;
        }
        option = number;
        switch (option) {
            case 1: createUser(); break;
            case 2: readAllUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 0:
            case 5:
                printf("Goodbye!\n");
                return 0;
            default: printf("Invalid option!\n"); break;
        }
    }
    printf("\nEnd of input. Goodbye!\n");
    return 0;
}
