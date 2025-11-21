#HealthDash – Personal Wellness Tracker (C Project)

HealthDash is a menu-driven health and fitness tracking system built entirely in C.
It was originally created as part of a First Semester Internal Hackathon (University Hackathon) and has since been slightly improved with a cleaned-up version.
# Project README

## Overview

This project contains two C source files:

* **healthdash.c** – the original version.
* **healthdashupdated.c** – the cleaned and improved version with reduced unchecked exceptions, better error handling, and code stability.

Additionally, a **project report PDF** has been included for documentation and reference.

---

## How to Run the Project

### **Requirements**

* A C compiler (GCC or Clang recommended)
* Terminal/Command Prompt

---

## **Running on Windows**

1. Open **Command Prompt**.
2. Navigate to the folder containing the project files:

   ```cmd
   cd path\to\your\project
   ```
3. Compile either version:

   ```cmd
   gcc healthdash.c -o healthdash
   ```

   or

   ```cmd
   gcc healthdashupdated.c -o healthdashupdated
   ```
4. Run the compiled program:

   ```cmd
   healthdash.exe
   ```

   or

   ```cmd
   healthdashupdated.exe
   ```

---

## **Running on macOS**

1. Open **Terminal**.
2. Navigate to the project directory:

   ```bash
   cd /path/to/project
   ```
3. Compile using Clang (default mac compiler):

   ```bash
   clang healthdash.c -o healthdash
   ```

   or

   ```bash
   clang healthdashupdated.c -o healthdashupdated
   ```
4. Run the program:

   ```bash
   ./healthdash
   ```

   or

   ```bash
   ./healthdashupdated
   ```

---

## **Running on Linux (Ubuntu / Fedora / others)**

1. Open **Terminal**.
2. Navigate to the project directory:

   ```bash
   cd /path/to/project
   ```
3. Install GCC if not already installed:

   ```bash
   sudo apt install gcc        # Ubuntu / Debian
   sudo dnf install gcc        # Fedora
   sudo pacman -S gcc          # Arch
   ```
4. Compile the code:

   ```bash
   gcc healthdash.c -o healthdash
   ```

   or

   ```bash
   gcc healthdashupdated.c -o healthdashupdated
   ```
5. Run the program:

   ```bash
   ./healthdash
   ```

   or

   ```bash
   ./healthdashupdated
   ```

---

## Notes

* The **updated file** is not recommended for execution unless the original doesn't work.
* Ensure all required input files (if any) are placed in the same directory before running.
* The project report PDF can be opened using any standard PDF viewer.

---



## The program allows each user to:

Create an account

Log various daily health metrics

View historical progress

Export CSV data

Plot graphs using gnuplot

Set reminders for wellness tasks

📂 Repository Contents
File	Description
Healthdash(sem1).c	The original hackathon submission code. Primary version of the project.
HealthDashUpdated.c	A cleaned and error-handled version. Use only if the original file throws compile/runtime errors on your system.
Project_Report.pdf	The official PDF report submitted during the internal hackathon presentation.
🔍 Project Overview

HealthDash is a terminal-based personal health dashboard where each user manages their own set of logs.
The system stores records as individual text files per user, allowing long-term tracking across:

Hydration

Diet

Workout

Sleep

Weight

The program also supports:

CSV export for Sleep & Weight

Graph plotting using gnuplot

User authentication

Health reminders

📌 Key Functionalities
1. User System

✔ Login
✔ Signup
✔ Username-password validation
✔ Individual data files for each category

User credentials are stored in users.txt.

2. Manage Daily Records

For each logged-in user, the program creates and updates individual .txt files:

Example filenames:

username_Hydration.txt  
username_Diet.txt  
username_Workout.txt  
username_Sleep.txt  
username_Weight.txt  


Each category supports:

Add new record

View records

Delete records (single or all)

Each entry automatically gets a timestamp:

YYYY-MM-DD HH:MM:SS

3. View Progress

The progress menu includes:

A. View All Records

Shows the contents of all health files for the logged-in user.

B. Graphical Report

Uses gnuplot to display graphs for:

Sleep duration over time

Weight change over time

Behind the scenes:

Sleep and weight logs are exported to CSV

CSV is passed to gnuplot for graphing

CSV examples:

sleep_data.csv
weight_data.csv

4. Health Reminders

You can:

Set a reminder

View all reminders saved for your username

Stored in:

reminders.txt

🧭 Program Flow Summary
START
 |
 |---> Login or Signup
        |
        |---> Successful login?
                |
                |---> MAIN MENU
                        |
                        |-- 1. Manage Daily Records
                        |       ├─ Add Hydration/Diet/Workout/Sleep/Weight
                        |       ├─ View Records
                        |       └─ Delete Records
                        |
                        |-- 2. View Progress
                        |       ├─ View all records
                        |       └─ Plot graphs (Sleep / Weight)
                        |
                        |-- 3. Health Reminders
                        |       ├─ Set reminder
                        |       └─ View reminders
                        |
                        |-- 4. Exit Program

⚙️ How to Compile & Run
Prerequisites

A C compiler (gcc recommended)

gnuplot installed (for graph plotting)



🗃 File Creation During Program Execution

When users log data, the program automatically creates these files:

User credentials:

users.txt


Per-user health logs:

username_Hydration.txt
username_Workout.txt
username_Diet.txt
username_Sleep.txt
username_Weight.txt


Reminder file:

reminders.txt


CSV files (generated during graph plotting):

sleep_data.csv
weight_data.csv
