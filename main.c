#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 100
#define NAME_LEN 100
#define PATIENT_FILE "patients.txt"
#define HISTORY_FILE "treated_history.txt"
#define DOCTOR_FILE "doctors.txt"
/* ===== Terminal Colors ===== */
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define RESET   "\033[0m"
/* ===== Patient Structure ===== */
typedef struct
{
    int id;
    char name[NAME_LEN];
    int age;
    int emergency;  /* 1 = Critical, 2 = Serious, 3 = Normal */
    int arrival;
    int doctorId;
    char doctorName[NAME_LEN];
} Patient;

typedef struct
{
    int id;
    char name[NAME_LEN];
    char specialization[NAME_LEN];
    char phone[30];
    int room;
    int available;   /* 1 = Available, 0 = Unavailable */
} Doctor;

Patient patients[MAX];
Patient treated[MAX];
Doctor doctors[MAX];

int patientCount = 0;
int treatedCount = 0;
int doctorCount = 0;
int arrivalCount = 0;

/* ===== Function Prototypes ===== */
void showTitle(void);
void showMenu(void);
int readInt(const char *prompt);
void readText(const char *prompt, char *text, int size);
void pauseScreen(void);

int patientIdExists(int id);
void addPatient(void);
void displayPatients(void);
void sortPatients(void);
void treatPatient(void);
void searchPatient(void);
void deletePatient(void);
void updatePatient(void);
void showHistory(void);

int doctorIdExists(int id);
Doctor *findDoctor(int id);
void addDoctor(void);
void displayDoctors(void);
void searchDoctor(void);
void updateDoctor(void);
void deleteDoctor(void);

void saveData(void);
void loadData(void);
int adminLogin(void);

const char *priorityName(int priority);

/* ===== Input Functions ===== */
int readInt(const char *prompt)
{
    char buffer[100];
    char *end;
    long value;

    while (1)
    {
        printf("%s", prompt);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            return 0;
        }

        value = strtol(buffer, &end, 10);

        while (isspace((unsigned char)*end))
        {
            end++;
        }

        if (end != buffer && *end == '\0')
        {
            if (value >= -2147483647L - 1 &&
                    value <= 2147483647L)
            {
                return (int)value;
            }
        }

        printf(RED "Invalid input! Enter a number.\n" RESET);
    }
}

void readText(const char *prompt, char *text, int size)
{
    while (1)
    {
        printf("%s", prompt);

        if (fgets(text, size, stdin) == NULL)
        {
            text[0] = '\0';
            return;
        }

        text[strcspn(text, "\n")] = '\0';

        /* Remove leading spaces */
        char *start = text;
        while (isspace((unsigned char)*start))
        {
            start++;
        }

        if (start != text)
        {
            memmove(text, start, strlen(start) + 1);
        }

        /* Remove trailing spaces */
        int len = (int)strlen(text);
        while (len > 0 && isspace((unsigned char)text[len - 1]))
        {
            text[--len] = '\0';
        }

        if (strlen(text) == 0)
        {
            printf(RED "Input cannot be empty!\n" RESET);
            continue;
        }

        /* Pipe is reserved for file storage */
        if (strchr(text, '|') != NULL)
        {
            printf(RED "The character | is not allowed.\n" RESET);
            continue;
        }

        return;
    }
}

void pauseScreen(void)
{
    char temp[10];
    printf(YELLOW "\nPress Enter to continue..." RESET);
    fgets(temp, sizeof(temp), stdin);
}

/* ===== Title and Menu ===== */
void showTitle(void)
{
    printf(CYAN "\n");
    printf("====================================================\n");
    printf("       HOSPITAL PATIENT MANAGEMENT SYSTEM\n");
    printf("                   MADE BY \n");
    printf("       TANJIUL RAIHAN *** S*** ***\n");
    printf("====================================================\n");
    printf(RESET);
}

void showMenu(void)
{
    printf(BLUE "\n--------------- MAIN MENU ----------------\n" RESET);
    printf(WHITE "1.  Add New Patient\n");
    printf("2.  Display Waiting Patients\n");
    printf("3.  Sort by Emergency Priority\n");
    printf("4.  Treat Next Patient\n");
    printf("5.  Search Patient\n");
    printf("6.  Delete Waiting Patient\n");
    printf("7.  Update Patient Information\n");
    printf("8.  View Treated Patient History\n");
    printf("9.  Save Data\n");
    printf("10. Add New Doctor\n");
    printf("11. Display Doctors\n");
    printf("12. Search Doctor\n");
    printf("13. Update Doctor Information\n");
    printf("14. Delete Doctor\n");
    printf("0.  Exit Program\n" RESET);
    printf(BLUE "------------------------------------------\n" RESET);
}

const char *priorityName(int priority)
{
    switch (priority)
    {
    case 1:
        return "CRITICAL";
    case 2:
        return "SERIOUS";
    case 3:
        return "NORMAL";
    default:
        return "UNKNOWN";
    }
}

/* ===== Check Duplicate IDs ===== */
int patientIdExists(int id)
{
    int i;

    for (i = 0; i < patientCount; i++)
    {
        if (patients[i].id == id)
        {
            return 1;
        }
    }

    for (i = 0; i < treatedCount; i++)
    {
        if (treated[i].id == id)
        {
            return 1;
        }
    }

    return 0;
}


/* ===== Doctor Functions ===== */
int doctorIdExists(int id)
{
    int i;

    for (i = 0; i < doctorCount; i++)
    {
        if (doctors[i].id == id)
        {
            return 1;
        }
    }

    return 0;
}

Doctor *findDoctor(int id)
{
    int i;

    for (i = 0; i < doctorCount; i++)
    {
        if (doctors[i].id == id)
        {
            return &doctors[i];
        }
    }

    return NULL;
}

void addDoctor(void)
{
    Doctor d;

    if (doctorCount >= MAX)
    {
        printf(RED "Doctor list is full!\n" RESET);
        return;
    }

    printf(CYAN "\n========== ADD NEW DOCTOR ==========\n" RESET);

    d.id = readInt("Enter Doctor ID: ");

    if (d.id <= 0)
    {
        printf(RED "Doctor ID must be positive.\n" RESET);
        return;
    }

    if (doctorIdExists(d.id))
    {
        printf(RED "This Doctor ID already exists!\n" RESET);
        return;
    }

    readText("Enter Doctor Name: ", d.name, sizeof(d.name));
    readText("Enter Specialization: ", d.specialization,
             sizeof(d.specialization));
    readText("Enter Phone Number: ", d.phone, sizeof(d.phone));

    d.room = readInt("Enter Room Number: ");

    if (d.room <= 0)
    {
        printf(RED "Room number must be positive.\n" RESET);
        return;
    }

    printf(YELLOW "\nDoctor Availability:\n");
    printf("1. Available\n");
    printf("0. Unavailable\n" RESET);

    d.available = readInt("Select status (1/0): ");

    if (d.available != 0 && d.available != 1)
    {
        printf(RED "Invalid availability status!\n" RESET);
        return;
    }

    doctors[doctorCount++] = d;

    saveData();

    printf(GREEN "\nDoctor added successfully!\n" RESET);
}

void displayDoctors(void)
{
    int i;

    printf(CYAN "\n========== DOCTOR INFORMATION ==========\n" RESET);

    if (doctorCount == 0)
    {
        printf(YELLOW "No doctor information available.\n" RESET);
        return;
    }

    printf(WHITE "%-6s %-20s %-20s %-15s %-8s %-12s\n",
           "ID", "NAME", "SPECIALIZATION", "PHONE", "ROOM", "STATUS");
    printf("--------------------------------------------------------------------------------\n" RESET);

    for (i = 0; i < doctorCount; i++)
    {
        printf("%s%-6d %-20.20s %-20.20s %-15.15s %-8d %-12s\n" RESET,
               doctors[i].available ? GREEN : RED,
               doctors[i].id,
               doctors[i].name,
               doctors[i].specialization,
               doctors[i].phone,
               doctors[i].room,
               doctors[i].available ? "AVAILABLE" : "UNAVAILABLE");
    }
}

void searchDoctor(void)
{
    int id, i;

    id = readInt("Enter Doctor ID to search: ");

    for (i = 0; i < doctorCount; i++)
    {
        if (doctors[i].id == id)
        {
            printf(GREEN "\nDoctor found:\n" RESET);
            printf("ID             : %d\n", doctors[i].id);
            printf("Name           : %s\n", doctors[i].name);
            printf("Specialization : %s\n", doctors[i].specialization);
            printf("Phone          : %s\n", doctors[i].phone);
            printf("Room           : %d\n", doctors[i].room);
            printf("Status         : %s\n",
                   doctors[i].available ? "AVAILABLE" : "UNAVAILABLE");
            return;
        }
    }

    printf(RED "Doctor not found!\n" RESET);
}

void updateDoctor(void)
{
    int id, i, room, status;
    char name[NAME_LEN];
    char specialization[NAME_LEN];
    char phone[30];

    id = readInt("Enter Doctor ID to update: ");

    for (i = 0; i < doctorCount; i++)
    {
        if (doctors[i].id == id)
        {
            printf(CYAN "\nUpdating doctor: %s\n" RESET,
                   doctors[i].name);

            readText("Enter new name: ", name, sizeof(name));
            readText("Enter new specialization: ",
                     specialization, sizeof(specialization));
            readText("Enter new phone number: ", phone, sizeof(phone));

            room = readInt("Enter new room number: ");

            if (room <= 0)
            {
                printf(RED "Invalid room number!\n" RESET);
                return;
            }

            printf(YELLOW "1. Available  0. Unavailable\n" RESET);
            status = readInt("Enter new status: ");

            if (status != 0 && status != 1)
            {
                printf(RED "Invalid availability status!\n" RESET);
                return;
            }

            strcpy(doctors[i].name, name);
            strcpy(doctors[i].specialization, specialization);
            strcpy(doctors[i].phone, phone);
            doctors[i].room = room;
            doctors[i].available = status;

            saveData();

            printf(GREEN "Doctor information updated successfully!\n" RESET);
            return;
        }
    }

    printf(RED "Doctor not found!\n" RESET);
}

void deleteDoctor(void)
{
    int id, i, j;
    char confirm[10];

    id = readInt("Enter Doctor ID to delete: ");

    for (i = 0; i < doctorCount; i++)
    {
        if (doctors[i].id == id)
        {
            printf(YELLOW "Delete doctor %s (ID %d)?\n" RESET,
                   doctors[i].name, doctors[i].id);

            readText("Type yes to confirm: ", confirm, sizeof(confirm));

            if (strcmp(confirm, "yes") != 0 &&
                    strcmp(confirm, "YES") != 0 &&
                    strcmp(confirm, "Yes") != 0)
            {
                printf(YELLOW "Deletion cancelled.\n" RESET);
                return;
            }

            int k;

            for (k = 0; k < patientCount; k++)
            {
                if (patients[k].doctorId == id)
                {
                    printf(RED "Cannot delete this doctor. The doctor is assigned to waiting patient %d.\n" RESET,
                           patients[k].id);
                    return;
                }
            }

            for (k = 0; k < treatedCount; k++)
            {
                if (treated[k].doctorId == id)
                {
                    printf(RED "Cannot delete this doctor because treatment history contains this doctor.\n" RESET);
                    return;
                }
            }

            for (j = i; j < doctorCount - 1; j++)
            {
                doctors[j] = doctors[j + 1];
            }

            doctorCount--;
            saveData();

            printf(GREEN "Doctor deleted successfully!\n" RESET);
            return;
        }
    }

    printf(RED "Doctor not found!\n" RESET);
}

/* ===== Save Data to Files ===== */
void saveData(void)
{
    FILE *fp;
    int i;

    fp = fopen(PATIENT_FILE, "w");

    if (fp == NULL)
    {
        printf(RED "Error: Cannot save patient data!\n" RESET);
        return;
    }

    /* Header stores arrivalCount and number of waiting patients */
    fprintf(fp, "%d %d\n", arrivalCount, patientCount);

    for (i = 0; i < patientCount; i++)
    {
        fprintf(fp, "%d|%s|%d|%d|%d|%d|%s\n",
                patients[i].id,
                patients[i].name,
                patients[i].age,
                patients[i].emergency,
                patients[i].arrival,
                patients[i].doctorId,
                patients[i].doctorName);
    }

    fclose(fp);

    fp = fopen(DOCTOR_FILE, "w");

    if (fp == NULL)
    {
        printf(RED "Error: Cannot save doctor data!\n" RESET);
        return;
    }

    fprintf(fp, "%d\n", doctorCount);

    for (i = 0; i < doctorCount; i++)
    {
        fprintf(fp, "%d|%s|%s|%s|%d|%d\n",
                doctors[i].id,
                doctors[i].name,
                doctors[i].specialization,
                doctors[i].phone,
                doctors[i].room,
                doctors[i].available);
    }

    fclose(fp);

    fp = fopen(HISTORY_FILE, "w");

    if (fp == NULL)
    {
        printf(RED "Error: Cannot save treatment history!\n" RESET);
        return;
    }

    fprintf(fp, "%d\n", treatedCount);

    for (i = 0; i < treatedCount; i++)
    {
        fprintf(fp, "%d|%s|%d|%d|%d|%d|%s\n",
                treated[i].id,
                treated[i].name,
                treated[i].age,
                treated[i].emergency,
                treated[i].arrival,
                treated[i].doctorId,
                treated[i].doctorName);
    }

    fclose(fp);

    printf(GREEN "Data saved successfully!\n" RESET);
}

/* ===== Load Patient Data ===== */
void loadData(void)
{
    FILE *fp;
    char line[300];
    int savedCount;
    int i;

    patientCount = 0;
    treatedCount = 0;
    arrivalCount = 0;

    fp = fopen(PATIENT_FILE, "r");

    if (fp != NULL)
    {
        if (fscanf(fp, "%d %d\n",
                   &arrivalCount, &savedCount) == 2)
        {

            if (savedCount < 0) savedCount = 0;
            if (savedCount > MAX) savedCount = MAX;

            for (i = 0; i < savedCount; i++)
            {
                Patient p;

                if (fgets(line, sizeof(line), fp) == NULL)
                {
                    break;
                }

                if (sscanf(line, "%d|%99[^|]|%d|%d|%d|%d|%99[^|]",
                           &p.id, p.name, &p.age,
                           &p.emergency, &p.arrival,
                           &p.doctorId, p.doctorName) == 7)
                {

                    if (p.id > 0 &&
                            p.age >= 0 &&
                            p.emergency >= 1 &&
                            p.emergency <= 3 &&
                            p.doctorId >= 0 &&
                            !patientIdExists(p.id))
                    {

                        patients[patientCount++] = p;
                    }
                }
                else if (sscanf(line, "%d|%99[^|]|%d|%d|%d",
                                &p.id, p.name, &p.age,
                                &p.emergency, &p.arrival) == 5)
                {
                    p.doctorId = 0;
                    strcpy(p.doctorName, "Not Assigned");

                    if (p.id > 0 &&
                            p.age >= 0 &&
                            p.emergency >= 1 &&
                            p.emergency <= 3 &&
                            !patientIdExists(p.id))
                    {

                        patients[patientCount++] = p;
                    }
                }
            }
        }

        fclose(fp);
    }

    fp = fopen(HISTORY_FILE, "r");

    if (fp != NULL)
    {
        if (fscanf(fp, "%d\n", &savedCount) == 1)
        {

            if (savedCount < 0) savedCount = 0;
            if (savedCount > MAX) savedCount = MAX;

            for (i = 0; i < savedCount; i++)
            {
                Patient p;

                if (fgets(line, sizeof(line), fp) == NULL)
                {
                    break;
                }

                if (sscanf(line, "%d|%99[^|]|%d|%d|%d|%d|%99[^|]",
                           &p.id, p.name, &p.age,
                           &p.emergency, &p.arrival,
                           &p.doctorId, p.doctorName) == 7)
                {

                    if (p.id > 0 &&
                            p.age >= 0 &&
                            p.emergency >= 1 &&
                            p.emergency <= 3 &&
                            p.doctorId >= 0 &&
                            !patientIdExists(p.id))
                    {

                        treated[treatedCount++] = p;
                    }
                }
                else if (sscanf(line, "%d|%99[^|]|%d|%d|%d",
                                &p.id, p.name, &p.age,
                                &p.emergency, &p.arrival) == 5)
                {
                    p.doctorId = 0;
                    strcpy(p.doctorName, "Not Assigned");

                    if (p.id > 0 &&
                            p.age >= 0 &&
                            p.emergency >= 1 &&
                            p.emergency <= 3 &&
                            !patientIdExists(p.id))
                    {

                        treated[treatedCount++] = p;
                    }
                }
            }
        }

        fclose(fp);
    }

    fp = fopen(DOCTOR_FILE, "r");

    if (fp != NULL)
    {
        if (fscanf(fp, "%d\n", &savedCount) == 1)
        {

            if (savedCount < 0) savedCount = 0;
            if (savedCount > MAX) savedCount = MAX;

            for (i = 0; i < savedCount; i++)
            {
                Doctor d;

                if (fgets(line, sizeof(line), fp) == NULL)
                {
                    break;
                }

                if (sscanf(line, "%d|%99[^|]|%99[^|]|%29[^|]|%d|%d",
                           &d.id, d.name, d.specialization,
                           d.phone, &d.room, &d.available) == 6)
                {

                    if (d.id > 0 &&
                            d.room > 0 &&
                            (d.available == 0 || d.available == 1) &&
                            !doctorIdExists(d.id))
                    {

                        doctors[doctorCount++] = d;
                    }
                }
            }
        }

        fclose(fp);
    }

    /* Ensure the next arrival number is unique */
    for (i = 0; i < patientCount; i++)
    {
        if (patients[i].arrival > arrivalCount)
        {
            arrivalCount = patients[i].arrival;
        }
    }

    for (i = 0; i < treatedCount; i++)
    {
        if (treated[i].arrival > arrivalCount)
        {
            arrivalCount = treated[i].arrival;
        }
    }

    printf(GREEN "Saved data loaded successfully.\n" RESET);
}

/* ===== Admin Login ===== */
int adminLogin(void)
{
    char username[50];
    char password[50];
    int attempts;

    printf(MAGENTA "\n====================================\n");
    printf("           ADMIN LOGIN\n");
    printf("====================================\n" RESET);

    for (attempts = 1; attempts <= 3; attempts++)
    {
        readText("Username: ", username, sizeof(username));
        readText("Password: ", password, sizeof(password));

        if (strcmp(username, "jidansafrin") == 0 &&
                strcmp(password, "love") == 0)
        {

            printf(GREEN "\nLogin successful!\n" RESET);
            return 1;
        }

        printf(RED "Incorrect username or password! "
               "Attempts left: %d\n" RESET,
               3 - attempts);
    }

    printf(RED "\nAccess denied!\n" RESET);
    return 0;
}

/* ===== Add Patient ===== */
void addPatient(void)
{
    Patient p;

    if (patientCount >= MAX)
    {
        printf(RED "Waiting list is full!\n" RESET);
        return;
    }

    printf(CYAN "\n========== ADD NEW PATIENT ==========\n" RESET);

    p.id = readInt("Enter Patient ID: ");

    if (p.id <= 0)
    {
        printf(RED "Patient ID must be positive.\n" RESET);
        return;
    }

    if (patientIdExists(p.id))
    {
        printf(RED "This ID already exists!\n" RESET);
        return;
    }

    readText("Enter Patient Name: ", p.name, sizeof(p.name));

    p.age = readInt("Enter Patient Age: ");

    if (p.age < 0 || p.age > 130)
    {
        printf(RED "Please enter a valid age (0-130).\n" RESET);
        return;
    }

    printf(YELLOW "\nEmergency Level:\n");
    printf("1. Critical\n");
    printf("2. Serious\n");
    printf("3. Normal\n" RESET);

    p.emergency = readInt("Select priority (1-3): ");

    if (p.emergency < 1 || p.emergency > 3)
    {
        printf(RED "Invalid emergency level!\n" RESET);
        return;
    }

    if (doctorCount == 0)
    {
        printf(RED "No doctors available. Add a doctor first.\n" RESET);
        return;
    }

    displayDoctors();

    p.doctorId = readInt("Enter Doctor ID to assign: ");

    Doctor *assignedDoctor = findDoctor(p.doctorId);

    if (assignedDoctor == NULL)
    {
        printf(RED "Doctor not found! Patient was not added.\n" RESET);
        return;
    }

    if (!assignedDoctor->available)
    {
        printf(RED "Selected doctor is currently unavailable!\n" RESET);
        return;
    }

    strcpy(p.doctorName, assignedDoctor->name);

    p.arrival = ++arrivalCount;
    patients[patientCount++] = p;

    saveData();

    printf(GREEN "\nPatient added successfully!\n" RESET);
}

/* ===== Display Waiting Patients ===== */
void displayPatients(void)
{
    int i;

    printf(CYAN "\n========== WAITING PATIENTS ==========\n" RESET);

    if (patientCount == 0)
    {
        printf(YELLOW "No waiting patients.\n" RESET);
        return;
    }

    printf(WHITE "%-8s %-20s %-6s %-12s %-20s %-8s\n",
           "ID", "NAME", "AGE", "PRIORITY", "DOCTOR", "ARRIVAL");
    printf("--------------------------------------------------------------------------------\n" RESET);

    for (i = 0; i < patientCount; i++)
    {
        const char *color = RESET;

        if (patients[i].emergency == 1) color = RED;
        else if (patients[i].emergency == 2) color = YELLOW;
        else color = GREEN;

        printf("%s%-8d %-20.20s %-6d %-12s %-20.20s %-8d\n" RESET,
               color,
               patients[i].id,
               patients[i].name,
               patients[i].age,
               priorityName(patients[i].emergency),
               patients[i].doctorName,
               patients[i].arrival);
    }
}

/* ===== Sort by Priority, FIFO within Same Priority ===== */
void sortPatients(void)
{
    int i, j;
    Patient temp;

    if (patientCount == 0)
    {
        printf(YELLOW "No patients to sort.\n" RESET);
        return;
    }

    for (i = 0; i < patientCount - 1; i++)
    {
        for (j = 0; j < patientCount - i - 1; j++)
        {

            if (patients[j].emergency > patients[j + 1].emergency ||
                    (patients[j].emergency == patients[j + 1].emergency &&
                     patients[j].arrival > patients[j + 1].arrival))
            {

                temp = patients[j];
                patients[j] = patients[j + 1];
                patients[j + 1] = temp;
            }
        }
    }

    saveData();

    printf(GREEN "\nPatients sorted by emergency priority!\n" RESET);
    displayPatients();
}

/* ===== Treat Next Patient ===== */
void treatPatient(void)
{
    int i;
    char confirm[10];

    if (patientCount == 0)
    {
        printf(YELLOW "No waiting patients to treat.\n" RESET);
        return;
    }

    if (treatedCount >= MAX)
    {
        printf(RED "Treatment history is full!\n" RESET);
        return;
    }

    /* Sort first so the highest-priority patient is first */
    sortPatients();

    printf(MAGENTA "\n========== NEXT PATIENT ==========\n" RESET);
    printf("ID       : %d\n", patients[0].id);
    printf("Name     : %s\n", patients[0].name);
    printf("Age      : %d\n", patients[0].age);
    printf("Priority : %s\n", priorityName(patients[0].emergency));
    printf("Doctor   : %s (ID: %d)\n",
           patients[0].doctorName, patients[0].doctorId);

    readText("\nConfirm treatment? (yes/no): ",
             confirm, sizeof(confirm));

    if (strcmp(confirm, "yes") != 0 &&
            strcmp(confirm, "YES") != 0 &&
            strcmp(confirm, "Yes") != 0)
    {

        printf(YELLOW "Treatment cancelled.\n" RESET);
        return;
    }

    treated[treatedCount++] = patients[0];

    for (i = 0; i < patientCount - 1; i++)
    {
        patients[i] = patients[i + 1];
    }

    patientCount--;

    saveData();

    printf(GREEN "\nPatient treatment recorded successfully!\n" RESET);
}

/* ===== Search Patient ===== */
void searchPatient(void)
{
    int id, i, found = 0;

    id = readInt("Enter Patient ID to search: ");

    for (i = 0; i < patientCount; i++)
    {
        if (patients[i].id == id)
        {
            printf(GREEN "\nPatient found in waiting list:\n" RESET);
            printf("ID: %d\nName: %s\nAge: %d\nPriority: %s\nDoctor: %s (ID: %d)\n",
                   patients[i].id,
                   patients[i].name,
                   patients[i].age,
                   priorityName(patients[i].emergency),
                   patients[i].doctorName,
                   patients[i].doctorId);
            found = 1;
        }
    }

    for (i = 0; i < treatedCount; i++)
    {
        if (treated[i].id == id)
        {
            printf(MAGENTA "\nPatient found in treated history:\n" RESET);
            printf("ID: %d\nName: %s\nAge: %d\nPriority: %s\nDoctor: %s (ID: %d)\n",
                   treated[i].id,
                   treated[i].name,
                   treated[i].age,
                   priorityName(treated[i].emergency),
                   treated[i].doctorName,
                   treated[i].doctorId);
            found = 1;
        }
    }

    if (!found)
    {
        printf(RED "Patient not found!\n" RESET);
    }
}

/* ===== Delete Waiting Patient ===== */
void deletePatient(void)
{
    int id, i, j;

    id = readInt("Enter Patient ID to delete: ");

    for (i = 0; i < patientCount; i++)
    {
        if (patients[i].id == id)
        {
            printf(YELLOW "Delete patient %s (ID %d)?\n" RESET,
                   patients[i].name, patients[i].id);

            char confirm[10];
            readText("Type yes to confirm: ", confirm, sizeof(confirm));

            if (strcmp(confirm, "yes") != 0 &&
                    strcmp(confirm, "YES") != 0 &&
                    strcmp(confirm, "Yes") != 0)
            {
                printf(YELLOW "Deletion cancelled.\n" RESET);
                return;
            }

            for (j = i; j < patientCount - 1; j++)
            {
                patients[j] = patients[j + 1];
            }

            patientCount--;
            saveData();

            printf(GREEN "Patient deleted successfully!\n" RESET);
            return;
        }
    }

    printf(RED "Waiting patient not found!\n" RESET);
}

/* ===== Update Patient ===== */
void updatePatient(void)
{
    int id, i, age, priority;
    char name[NAME_LEN];

    id = readInt("Enter Patient ID to update: ");

    for (i = 0; i < patientCount; i++)
    {
        if (patients[i].id == id)
        {
            printf(CYAN "\nUpdating patient: %s\n" RESET,
                   patients[i].name);

            readText("Enter new name: ", name, sizeof(name));
            age = readInt("Enter new age: ");

            if (age < 0 || age > 130)
            {
                printf(RED "Invalid age!\n" RESET);
                return;
            }

            printf(YELLOW "1. Critical  2. Serious  3. Normal\n" RESET);
            priority = readInt("Enter new priority: ");

            if (priority < 1 || priority > 3)
            {
                printf(RED "Invalid priority!\n" RESET);
                return;
            }

            if (doctorCount == 0)
            {
                printf(RED "No doctors available.\n" RESET);
                return;
            }

            displayDoctors();

            int newDoctorId = readInt("Enter new Doctor ID: ");
            Doctor *newDoctor = findDoctor(newDoctorId);

            if (newDoctor == NULL)
            {
                printf(RED "Doctor not found!\n" RESET);
                return;
            }

            if (!newDoctor->available)
            {
                printf(RED "Selected doctor is currently unavailable!\n" RESET);
                return;
            }

            strcpy(patients[i].name, name);
            patients[i].age = age;
            patients[i].emergency = priority;
            patients[i].doctorId = newDoctor->id;
            strcpy(patients[i].doctorName, newDoctor->name);

            saveData();

            printf(GREEN "Patient updated successfully!\n" RESET);
            return;
        }
    }

    printf(RED "Waiting patient not found!\n" RESET);
}

/* ===== Treatment History ===== */
void showHistory(void)
{
    int i;

    printf(MAGENTA "\n========== TREATED PATIENT HISTORY ==========\n" RESET);

    if (treatedCount == 0)
    {
        printf(YELLOW "No treatment history available.\n" RESET);
        return;
    }

    printf(WHITE "%-8s %-20s %-6s %-12s %-20s %-8s\n",
           "ID", "NAME", "AGE", "PRIORITY", "DOCTOR", "ARRIVAL");
    printf("--------------------------------------------------------------------------------\n" RESET);

    for (i = 0; i < treatedCount; i++)
    {
        printf("%s%-8d %-24.24s %-8d %-12s %-8d\n" RESET,
               GREEN,
               treated[i].id,
               treated[i].name,
               treated[i].age,
               priorityName(treated[i].emergency),
               treated[i].arrival);
    }
}

/* ===== Main Function ===== */
int main(void)
{
    int choice;

    showTitle();

    /* Load existing files before login */
    loadData();

    if (!adminLogin())
    {
        return 0;
    }

    do
    {
        showTitle();
        showMenu();

        choice = readInt(CYAN "Enter your choice: " RESET);

        switch (choice)
        {
        case 1:
            addPatient();
            break;

        case 2:
            displayPatients();
            break;

        case 3:
            sortPatients();
            break;

        case 4:
            treatPatient();
            break;

        case 5:
            searchPatient();
            break;

        case 6:
            deletePatient();
            break;

        case 7:
            updatePatient();
            break;

        case 8:
            showHistory();
            break;

        case 9:
            saveData();
            break;

        case 10:
            addDoctor();
            break;

        case 11:
            displayDoctors();
            break;

        case 12:
            searchDoctor();
            break;

        case 13:
            updateDoctor();
            break;

        case 14:
            deleteDoctor();
            break;

        case 0:
            saveData();
            printf(GREEN "\nThank you for using HMAP!\n" RESET);
            printf(CYAN "Program closed successfully.\n" RESET);
            break;

        default:
            printf(RED "\nInvalid choice! Please select 0-14.\n" RESET);
        }

        if (choice != 0)
        {
            pauseScreen();
        }

    }
    while (choice != 0);

    return 0;
}

