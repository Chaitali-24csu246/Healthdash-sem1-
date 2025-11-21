// healthdash.c
// Cleaned and fixed version of your HEALTHDASH program.
// Compile: gcc -std=c11 healthdash.c -o healthdash
// Run: ./healthdash

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h> // for access() on POSIX

#define MAXLEN 50
#define LINEBUF 256

/* ---------- Prototypes ---------- */
/* Main submenu functions */
void mng_record(char *username);  // Manage Records (Add, Delete, View)
void export_sleep_data_to_csv(const char *username);
void export_weight_data_to_csv(const char *username);
void plot_graph(const char *csv_filename);

/* helpers */
void display_file_content(const char *filename);
int file_exists(const char *filename);
void read_line(char *buf, size_t n);

/* Progress / graphs */
void progress(const char *username);

/* Health reminders */
void hlth_remndr(const char *username);
void set_reminder(const char *username);
void view_reminders(const char *username);

/* User entry */
int userenter(char *username);    // User login/signup
int login(char *username);        // Login
int signup(char *username);       // Signup

/* Manage records */
void update_record(char *username, const char *type);
void delete_record(char *username, const char *type);
void view_record(char *username, const char *type);

/* Main menu */
int mainmenu(void);

/* ---------- Implementations ---------- */

void read_line(char *buf, size_t n) {
    if (!buf || n == 0) return;
    if (fgets(buf, (int)n, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    // strip newline
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
}

/* Check if a file exists and is readable */
int file_exists(const char *filename) {
    return (access(filename, R_OK) == 0);
}

/* Display contents of a file to stdout */
void display_file_content(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s.\n", filename);
        return;
    }

    char line[LINEBUF];
    while (fgets(line, sizeof(line), file)) {
        fputs(line, stdout);
    }

    fclose(file);
}

/* Export sleep data in user's file to CSV */
void export_sleep_data_to_csv(const char *username) {
    char sleep_filename[120], csv_filename[120];
    snprintf(sleep_filename, sizeof(sleep_filename), "%s_Sleep.txt", username);
    snprintf(csv_filename, sizeof(csv_filename), "sleep_data.csv");

    FILE *sleep_file = fopen(sleep_filename, "r");
    if (sleep_file == NULL) {
        printf("No sleep records found for user '%s'.\n", username);
        return;
    }

    FILE *csv_file = fopen(csv_filename, "w");
    if (csv_file == NULL) {
        printf("Error opening CSV file for writing.\n");
        fclose(sleep_file);
        return;
    }

    fprintf(csv_file, "DateTime, SleepDuration_minutes\n");

    char line[LINEBUF];
    char datetime[LINEBUF];
    int sleep_duration;

    while (fgets(line, sizeof(line), sleep_file)) {
        // original lines look like: "Sleep: 480 minutes, DateTime: 2025-02-21 10:45:32"
        // Use %[^\n] to capture rest of line with spaces
        if (sscanf(line, "Sleep: %d minutes, DateTime: %[^\n]", &sleep_duration, datetime) == 2) {
            // Trim whitespace from datetime (leading spaces)
            char *p = datetime;
            while (*p && isspace((unsigned char)*p)) p++;
            fprintf(csv_file, "%s,%d\n", p, sleep_duration);
        }
    }

    fclose(sleep_file);
    fclose(csv_file);
    printf("Sleep data exported to %s\n", csv_filename);
}

/* Export weight data in user's file to CSV */
void export_weight_data_to_csv(const char *username) {
    char weight_filename[120], csv_filename[120];
    snprintf(weight_filename, sizeof(weight_filename), "%s_Weight.txt", username);
    snprintf(csv_filename, sizeof(csv_filename), "weight_data.csv");

    FILE *weight_file = fopen(weight_filename, "r");
    if (weight_file == NULL) {
        printf("No weight records found for user '%s'.\n", username);
        return;
    }

    FILE *csv_file = fopen(csv_filename, "w");
    if (csv_file == NULL) {
        printf("Error opening CSV file for writing.\n");
        fclose(weight_file);
        return;
    }

    fprintf(csv_file, "DateTime, Weight_kg\n");

    char line[LINEBUF];
    char datetime[LINEBUF];
    float weight;

    while (fgets(line, sizeof(line), weight_file)) {
        // lines like: "Weight: 72.50 kg, DateTime: 2025-02-21 10:45:32"
        if (sscanf(line, "Weight: %f kg, DateTime: %[^\n]", &weight, datetime) == 2) {
            char *p = datetime;
            while (*p && isspace((unsigned char)*p)) p++;
            fprintf(csv_file, "%s,%.2f\n", p, weight);
        }
    }

    fclose(weight_file);
    fclose(csv_file);
    printf("Weight data exported to %s\n", csv_filename);
}

/* Plot graph using gnuplot (if available) */
void plot_graph(const char *csv_filename) {
    // Check if gnuplot present
    int gp_check = system("gnuplot --version > /dev/null 2>&1");
    if (gp_check != 0) {
        printf("gnuplot not found on PATH. Install gnuplot to use plotting feature.\n");
        return;
    }

    char command[512];
    // We use single-quoted filename; set datafile separator and auto-format xlabels
    // pause -1 keeps window open until user closes gnuplot window
    snprintf(command, sizeof(command),
             "gnuplot -persist -e \"set datafile separator ','; set xdata time; set timefmt '%%Y-%%m-%%d %%H:%%M:%%S'; "
             "set format x '%%Y-%%m-%%d\\n%%H:%%M'; set title 'Health Data - %s'; set xlabel 'DateTime'; set ylabel 'Value'; "
             "plot '%s' using 1:2 with linespoints title '%s'\"", csv_filename, csv_filename, csv_filename);

    int result = system(command);
    if (result == 0) {
        printf("Graph plotted (gnuplot used).\n");
    } else {
        printf("Failed to plot the graph (gnuplot returned error).\n");
    }
}

/* Progress function: view records or produce graphs */
void progress(const char *username) {
    int choice = 0;

    printf("\nProgress Menu for user '%s'\n", username);
    printf("1. View all records for your username\n");
    printf("2. Graphical Report (Sleep / Weight)\n");
    printf("3. Exit to main menu\n");
    printf("Enter your choice: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    if (sscanf(buf, "%d", &choice) != 1) {
        printf("Invalid selection. Returning to main menu.\n");
        return;
    }

    if (choice == 1) {
        int records_found = 0;
        const char *file_types[] = {"Workout", "Diet", "Hydration", "Sleep", "Weight", "Steps"};

        for (size_t i = 0; i < sizeof(file_types)/sizeof(file_types[0]); ++i) {
            char filename[120];
            snprintf(filename, sizeof(filename), "%s_%s.txt", username, file_types[i]);

            if (file_exists(filename)) {
                records_found = 1;
                printf("\n--- Contents of %s ---\n", filename);
                display_file_content(filename);
                printf("\n-----------------------\n");
            }
        }

        if (!records_found) {
            printf("No records found for user %s.\n", username);
        }
    } else if (choice == 2) {
        int graph_choice = 0;
        printf("Which graph do you want to plot?\n");
        printf("1. Sleep Graph\n");
        printf("2. Weight Graph\n");
        printf("Enter your choice: ");
        read_line(buf, sizeof(buf));
        if (sscanf(buf, "%d", &graph_choice) != 1) {
            printf("Invalid selection. Returning to main menu.\n");
            return;
        }

        if (graph_choice == 1) {
            export_sleep_data_to_csv(username);
            plot_graph("sleep_data.csv");
        } else if (graph_choice == 2) {
            export_weight_data_to_csv(username);
            plot_graph("weight_data.csv");
        } else {
            printf("Invalid choice. Returning to main menu.\n");
        }
    } else if (choice == 3) {
        printf("Returning to main menu.\n");
    } else {
        printf("Invalid choice. Returning to main menu.\n");
    }
}

/* Create or view health reminders */
void hlth_remndr(const char *username) {
    printf("\nHealth Reminders Menu for %s\n", username);
    printf("1. Set reminder\n");
    printf("2. View reminders\n");
    printf("Enter your choice: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    int action = 0;
    if (sscanf(buf, "%d", &action) != 1) {
        printf("Invalid input. Returning to main menu.\n");
        return;
    }

    if (action == 1) {
        set_reminder(username);
    } else if (action == 2) {
        view_reminders(username);
    } else {
        printf("Invalid input. Returning to main menu.\n");
    }
}

/* Set reminder: appended to reminders.txt with username */
void set_reminder(const char *username) {
    char reminder[LINEBUF];
    printf("Enter a health reminder (single line): ");
    read_line(reminder, sizeof(reminder));
    if (reminder[0] == '\0') {
        printf("Empty reminder. Aborting.\n");
        return;
    }

    FILE *file = fopen("reminders.txt", "a");
    if (!file) {
        printf("Error opening reminders file.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_time[100];
    strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", t);

    fprintf(file, "Reminder: %s, DateTime: %s, User: %s\n", reminder, date_time, username);
    fclose(file);
    printf("Reminder set successfully!\n");
}

/* View reminders filtered by username */
void view_reminders(const char *username) {
    FILE *file = fopen("reminders.txt", "r");
    if (!file) {
        printf("No reminders found.\n");
        return;
    }

    char line[LINEBUF];
    printf("Viewing reminders for user %s:\n", username);
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, username) != NULL) {
            fputs(line, stdout);
            found = 1;
        }
    }
    if (!found) printf("(No reminders for %s)\n", username);
    fclose(file);
}

/* Signup: create user and a stub health file */
int signup(char *username) {
    char password[MAXLEN];
    printf("Enter new username: ");
    read_line(username, MAXLEN);
    if (username[0] == '\0') {
        printf("Username cannot be empty.\n");
        return 0;
    }

    // Check and create users.txt if not present
    FILE *file = fopen("users.txt", "a+");
    if (!file) {
        printf("Error opening users.txt\n");
        return 0;
    }

    // Check duplicates
    rewind(file);
    char file_user[MAXLEN], file_pass[MAXLEN];
    while (fscanf(file, "%49s %49s", file_user, file_pass) == 2) {
        if (strcmp(file_user, username) == 0) {
            printf("Username already exists. Choose another.\n");
            fclose(file);
            return 0;
        }
    }

    printf("Enter new password: ");
    read_line(password, MAXLEN);
    if (password[0] == '\0') {
        printf("Password cannot be empty.\n");
        fclose(file);
        return 0;
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);

    // create user-specific files
    const char *types[] = {"Workout", "Diet", "Hydration", "Sleep", "Weight", "Steps"};
    for (size_t i = 0; i < sizeof(types)/sizeof(types[0]); ++i) {
        char fname[120];
        snprintf(fname, sizeof(fname), "%s_%s.txt", username, types[i]);
        FILE *f = fopen(fname, "a");
        if (f) fclose(f);
    }

    printf("Signup successful! Welcome user %s\n", username);
    return 1;
}

/* Login */
int login(char *username) {
    char password[MAXLEN];
    char file_username[MAXLEN], file_password[MAXLEN];

    if (!file_exists("users.txt")) {
        printf("No users found. Please sign up first.\n");
        return 0;
    }

    printf("Enter username: ");
    read_line(username, MAXLEN);
    printf("Enter password: ");
    read_line(password, MAXLEN);

    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("Error opening users file.\n");
        return 0;
    }
    int success = 0;
    while (fscanf(file, "%49s %49s", file_username, file_password) == 2) {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            success = 1;
            break;
        }
    }
    fclose(file);
    if (success) {
        printf("Welcome to Healthdash user %s\n", username);
        return 1;
    } else {
        printf("Invalid username or password.\n");
        return 0;
    }
}

/* userenter: choose login or signup */
int userenter(char *username) {
    printf("\nAre you already a user, or do you want to sign up?\n");
    printf("1. Already a user\n2. Sign up\nEnter choice: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    int a = 0;
    if (sscanf(buf, "%d", &a) != 1) return 0;

    if (a == 1) return login(username);
    if (a == 2) return signup(username);
    return 0;
}

/* Manage records (menu) */
void mng_record(char *username) {
    printf("\nChoose a category to manage records:\n");
    printf("1. Hydration\n2. Diet\n3. Workout\n4. Sleep\n5. Weight\nEnter your choice: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    int choice = 0;
    if (sscanf(buf, "%d", &choice) != 1) {
        printf("Invalid selection.\n");
        return;
    }

    if (choice >= 1 && choice <= 5) {
        printf("\n1. Add record\n2. View records\n3. Delete record\nEnter your choice: ");
        read_line(buf, sizeof(buf));
        int action = 0;
        if (sscanf(buf, "%d", &action) != 1) {
            printf("Invalid selection.\n");
            return;
        }

        const char *type = (choice == 1) ? "Hydration" :
                           (choice == 2) ? "Diet" :
                           (choice == 3) ? "Workout" :
                           (choice == 4) ? "Sleep" : "Weight";

        if (action == 1) update_record(username, type);
        else if (action == 2) view_record(username, type);
        else if (action == 3) delete_record(username, type);
        else printf("Invalid action. Returning.\n");
    } else {
        printf("Invalid category.\n");
    }
}

/* Update records */
void update_record(char *username, const char *type) {
    char filename[120];
    snprintf(filename, sizeof(filename), "%s_%s.txt", username, type);
    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error opening %s file for appending.\n", filename);
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_time[100];
    strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", t);

    if (strcmp(type, "Workout") == 0) {
        printf("Select workout type:\n1. Cardio\n2. Yoga\n3. Gym\n4. Running\n5. Sport\nEnter choice: ");
        char buf[32];
        read_line(buf, sizeof(buf));
        int workout_type = 0;
        if (sscanf(buf, "%d", &workout_type) != 1) workout_type = 0;
        const char *workout_type_str = "Unknown";
        switch (workout_type) {
            case 1: workout_type_str = "Cardio"; break;
            case 2: workout_type_str = "Yoga"; break;
            case 3: workout_type_str = "Gym"; break;
            case 4: workout_type_str = "Running"; break;
            case 5: workout_type_str = "Sport"; break;
        }
        printf("Enter duration in minutes: ");
        read_line(buf, sizeof(buf));
        int duration = 0;
        sscanf(buf, "%d", &duration);
        fprintf(file, "Workout: %s, Duration: %d minutes, DateTime: %s\n", workout_type_str, duration, date_time);
    } else if (strcmp(type, "Diet") == 0) {
        char food_item[LINEBUF];
        char qbuf[32];
        printf("Enter the food item you ate (single line): ");
        read_line(food_item, sizeof(food_item));
        printf("Enter quantity in grams: ");
        read_line(qbuf, sizeof(qbuf));
        int quantity = 0;
        sscanf(qbuf, "%d", &quantity);
        fprintf(file, "Food: %s, Quantity: %d grams, DateTime: %s\n", food_item, quantity, date_time);
    } else if (strcmp(type, "Hydration") == 0) {
        char buf[64];
        printf("Enter hydration amount in liters (e.g., 0.5): ");
        read_line(buf, sizeof(buf));
        float hydration = 0.0f;
        sscanf(buf, "%f", &hydration);
        fprintf(file, "Hydration: %.2f liters, DateTime: %s\n", hydration, date_time);
    } else if (strcmp(type, "Weight") == 0) {
        char buf[64];
        printf("Enter weight in kg (e.g., 72.5): ");
        read_line(buf, sizeof(buf));
        float weight = 0.0f;
        sscanf(buf, "%f", &weight);
        fprintf(file, "Weight: %.2f kg, DateTime: %s\n", weight, date_time);
    } else if (strcmp(type, "Sleep") == 0) {
        char buf[64];
        printf("Enter sleep duration in minutes (e.g., 480): ");
        read_line(buf, sizeof(buf));
        int sleep_duration = 0;
        sscanf(buf, "%d", &sleep_duration);
        fprintf(file, "Sleep: %d minutes, DateTime: %s\n", sleep_duration, date_time);
    }

    fclose(file);
    printf("%s record added successfully!\n", type);
}

/* Delete records: all or specific */
void delete_record(char *username, const char *type) {
    char filename[120];
    snprintf(filename, sizeof(filename), "%s_%s.txt", username, type);
    if (!file_exists(filename)) {
        printf("No records found for %s. Nothing to delete.\n", type);
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file for reading.\n");
        return;
    }

    char line[LINEBUF];
    int record_count = 0;
    while (fgets(line, sizeof(line), file)) {
        record_count++;
        printf("%d: %s", record_count, line);
    }
    fclose(file);

    if (record_count == 0) {
        printf("No records to delete.\n");
        return;
    }

    printf("\n1. Delete all records\n2. Delete specific record\nEnter your choice: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    int action = 0;
    if (sscanf(buf, "%d", &action) != 1) {
        printf("Invalid choice. Returning.\n");
        return;
    }

    if (action == 1) {
        if (remove(filename) == 0) printf("All %s records deleted.\n", type);
        else printf("Error deleting all %s records.\n", type);
    } else if (action == 2) {
        printf("Enter the record number to delete: ");
        read_line(buf, sizeof(buf));
        int record_to_delete = 0;
        if (sscanf(buf, "%d", &record_to_delete) != 1 || record_to_delete <= 0) {
            printf("Invalid record number.\n");
            return;
        }

        FILE *rf = fopen(filename, "r");
        FILE *temp = fopen("temp_healthdash.txt", "w");
        if (!rf || !temp) {
            printf("Error opening files.\n");
            if (rf) fclose(rf);
            if (temp) fclose(temp);
            return;
        }

        int current = 1;
        int deleted = 0;
        while (fgets(line, sizeof(line), rf)) {
            if (current != record_to_delete) {
                fputs(line, temp);
            } else {
                deleted = 1;
            }
            current++;
        }
        fclose(rf);
        fclose(temp);

        if (deleted) {
            if (remove(filename) == 0 && rename("temp_healthdash.txt", filename) == 0) {
                printf("Record %d deleted successfully.\n", record_to_delete);
            } else {
                printf("Error replacing original file.\n");
                remove("temp_healthdash.txt");
            }
        } else {
            printf("Record %d not found.\n", record_to_delete);
            remove("temp_healthdash.txt");
        }
    } else {
        printf("Invalid choice. Returning.\n");
    }
}

/* View record simple */
void view_record(char *username, const char *type) {
    char filename[120];
    snprintf(filename, sizeof(filename), "%s_%s.txt", username, type);
    if (!file_exists(filename)) {
        printf("No records found for %s.\n", type);
        return;
    }
    printf("Viewing records for %s:\n", type);
    display_file_content(filename);
}

/* Main menu print and read */
int mainmenu(void) {
    printf("\n_________________________\n");
    printf("*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    printf("|------ MAIN MENU ------|\n");
    printf("|1. Manage Daily Records|\n");
    printf("|2. View Progress       |\n");
    printf("|3. Health Reminders    |\n");
    printf("|4. Exit Program        |\n");
    printf("|_______________________|\n");
    printf("Enter your choice: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    int d = 0;
    if (sscanf(buf, "%d", &d) != 1) d = 0;
    return d;
}

/* ---------- main ---------- */
int main(void) {
    printf("*******************************************************\n");
    printf("              Welcome to HEALTHDASH\n         Your personal wellness companion\n");
    printf("*******************************************************\n");
    printf("Please login or sign up to continue:\n");

    char username[MAXLEN] = {0};

    while (1) {
        if (userenter(username) == 1) {
            // mainmenu loop
            while (1) {
                int choice = mainmenu();
                switch (choice) {
                    case 1: mng_record(username); break;
                    case 2: progress(username); break;
                    case 3: hlth_remndr(username); break;
                    case 4:
                        printf("Exiting the program. Goodbye!\n");
                        return 0;
                    default:
                        printf("Invalid input. Try again.\n");
                }
            }
        } else {
            printf("Invalid choice, user or password. Do you want to try again? Y/N: ");
            char ans[8];
            read_line(ans, sizeof(ans));
            if (ans[0] == 'N' || ans[0] == 'n') {
                printf("Exiting\n See you next time!\n");
                return 0;
            }
        }
    }
    return 0;
}
