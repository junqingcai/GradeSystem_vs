/*
 * Member B responsibility:
 * Student record management, linked-list operations and input validation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "student.h"
/* Remove remaining characters in stdin after unusual input. */
void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}
void pauseScreen(void) {
    printf("Press Enter to continue...");
    fflush(stdout);
    clearInputBuffer();
}
/* Read y/n confirmation. It is used before risky operations. */
int askYesNo(const char *prompt) {
    char line[LINE_LEN];
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            clearInputBuffer();
            continue;
        }
        if (line[0] == 'y' || line[0] == 'Y') return 1;
        if (line[0] == 'n' || line[0] == 'N') return 0;
        printf("Please input y or n.\n");
    }
}
/* Read an integer in a fixed range to avoid invalid menu choices. */
int readInt(const char *prompt, int minValue, int maxValue) {
    char line[LINE_LEN];
    int value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            clearInputBuffer();
            continue;
        }
        if (sscanf(line, "%d %c", &value, &extra) == 1 &&
            value >= minValue && value <= maxValue) {
            return value;
        }
        printf("Input error. Please enter an integer from %d to %d.\n",
               minValue, maxValue);
    }
}
/* Read a floating-point number in a fixed range. */
float readFloat(const char *prompt, float minValue, float maxValue) {
    char line[LINE_LEN];
    float value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            clearInputBuffer();
            continue;
        }
        if (sscanf(line, "%f %c", &value, &extra) == 1 &&
            value >= minValue && value <= maxValue) {
            return value;
        }
        printf("Input error. Please enter a number from %.2f to %.2f.\n",
               minValue, maxValue);
    }
}
float readScore(const char *prompt) {
    return readFloat(prompt, 0.0f, 100.0f);
}
/* Read a word without spaces. Names in data.txt also use this rule. */
void readString(const char *prompt, char *buffer, int size) {
    char line[LINE_LEN], temp[LINE_LEN];
    int len;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            clearInputBuffer();
            continue;
        }
        line[strcspn(line, "\n")] = '\0';
        if (sscanf(line, "%255s", temp) == 1) {
            len = (int)strlen(temp);
            if (len > 0 && len < size) {
                strcpy(buffer, temp);
                return;
            }
        }
        printf("Input cannot be empty or longer than %d characters.\n", size - 1);
    }
}
/* Create a list with a head node. The head node does not store real data. */
Node *createList(void) {
    Node *head = (Node *)malloc(sizeof(Node));
    if (head != NULL) {
        memset(&head->data, 0, sizeof(Student));
        head->next = NULL;
    }
    return head;
}
Node *createNode(Student stu) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) return NULL;
    node->data = stu;
    node->next = NULL;
    return node;
}
/* Release every node including the head node. */
void freeList(Node *head) {
    Node *p = head, *next;
    while (p != NULL) {
        next = p->next;
        free(p);
        p = next;
    }
}
/* Clear all real student nodes but keep the head node for later use. */
void clearList(Node *head) {
    Node *p, *next;
    if (head == NULL) return;
    p = head->next;
    while (p != NULL) {
        next = p->next;
        free(p);
        p = next;
    }
    head->next = NULL;
}
int isListEmpty(Node *head) {
    return head == NULL || head->next == NULL;
}
int getListLength(Node *head) {
    int count = 0;
    Node *p = head == NULL ? NULL : head->next;
    while (p != NULL) {
        count++;
        p = p->next;
    }
    return count;
}
Node *getLastNode(Node *head) {
    Node *p = head;
    if (p == NULL) return NULL;
    while (p->next != NULL) p = p->next;
    return p;
}
Node *getNodeByIndex(Node *head, int index) {
    int i = 1;
    Node *p = head == NULL ? NULL : head->next;
    while (p != NULL) {
        if (i == index) return p;
        i++;
        p = p->next;
    }
    return NULL;
}
/* Find the previous node, which makes deletion and insertion easier. */
Node *findPrevByNum(Node *head, const char *num) {
    Node *p;
    if (head == NULL || num == NULL) return NULL;
    p = head;
    while (p->next != NULL) {
        if (strcmp(p->next->data.num, num) == 0) return p;
        p = p->next;
    }
    return NULL;
}
Node *findByNum(Node *head, const char *num) {
    Node *prev = findPrevByNum(head, num);
    return prev == NULL ? NULL : prev->next;
}
int isNumExist(Node *head, const char *num) {
    return findByNum(head, num) != NULL;
}
int isNameExist(Node *head, const char *name) {
    Node *p = head == NULL ? NULL : head->next;
    while (p != NULL) {
        if (strcmp(p->data.name, name) == 0) return 1;
        p = p->next;
    }
    return 0;
}
int appendStudent(Node *head, Student stu) {
    Node *last, *node;
    if (head == NULL || isNumExist(head, stu.num)) return 0;
    node = createNode(stu);
    if (node == NULL) return 0;
    last = getLastNode(head);
    if (last == NULL) {
        free(node);
        return 0;
    }
    last->next = node;
    return 1;
}
/* Insert a new node before the node whose student number is beforeNum. */
int insertBeforeNum(Node *head, const char *beforeNum, Student stu) {
    Node *prev, *node;
    if (head == NULL || beforeNum == NULL || isNumExist(head, stu.num)) return 0;
    prev = findPrevByNum(head, beforeNum);
    if (prev == NULL) return 0;
    node = createNode(stu);
    if (node == NULL) return 0;
    node->next = prev->next;
    prev->next = node;
    return 1;
}
/* Delete by student number. If removed is not NULL, return deleted data to caller. */
int removeByNum(Node *head, const char *num, Student *removed) {
    Node *prev, *target;
    if (head == NULL || num == NULL) return 0;
    prev = findPrevByNum(head, num);
    if (prev == NULL) return 0;
    target = prev->next;
    if (removed != NULL) *removed = target->data;
    prev->next = target->next;
    free(target);
    return 1;
}
void swapStudentData(Node *a, Node *b) {
    Student temp;
    if (a == NULL || b == NULL) return;
    temp = a->data;
    a->data = b->data;
    b->data = temp;
}
int isScoreValid(float score) {
    return score >= 0.0f && score <= 100.0f;
}
/* Student number must be all digits and have a reasonable length. */
int isNumFormatValid(const char *num) {
    int i, len;
    if (num == NULL) return 0;
    len = (int)strlen(num);
    if (len < 4 || len >= NUM_LEN) return 0;
    for (i = 0; i < len; i++) {
        if (!isdigit((unsigned char)num[i])) return 0;
    }
    return 1;
}
/* Student name is stored as one word, so spaces are not allowed. */
int isNameFormatValid(const char *name) {
    int i, len;
    if (name == NULL) return 0;
    len = (int)strlen(name);
    if (len <= 0 || len >= NAME_LEN) return 0;
    for (i = 0; i < len; i++) {
        if (isspace((unsigned char)name[i])) return 0;
    }
    return 1;
}
void calculateScore(Student *stu) {
    if (stu == NULL) return;
    stu->total = stu->cScore + stu->mathScore + stu->englishScore;
    stu->average = stu->total / 3.0f;
}
/* Let search and analysis code get different score fields through one function. */
float getStudentScore(Student stu, int subject) {
    switch (subject) {
        case SUBJECT_C: return stu.cScore;
        case SUBJECT_MATH: return stu.mathScore;
        case SUBJECT_ENGLISH: return stu.englishScore;
        case SUBJECT_TOTAL: return stu.total;
        case SUBJECT_AVERAGE: return stu.average;
        default: return 0.0f;
    }
}
const char *getSubjectName(int subject) {
    switch (subject) {
        case SUBJECT_C: return "C";
        case SUBJECT_MATH: return "Math";
        case SUBJECT_ENGLISH: return "English";
        case SUBJECT_TOTAL: return "Total";
        case SUBJECT_AVERAGE: return "Average";
        default: return "Unknown";
    }
}
void inputScores(Student *stu) {
    if (stu == NULL) return;
    stu->cScore = readScore("Input C score: ");
    stu->mathScore = readScore("Input math score: ");
    stu->englishScore = readScore("Input English score: ");
    calculateScore(stu);
}
void copyStudent(Student *dest, Student src) {
    if (dest != NULL) *dest = src;
}
/* Read a complete student record and check duplicate student number if needed. */
Student inputStudent(Node *head, int checkDuplicate) {
    Student stu;
    memset(&stu, 0, sizeof(Student));
    while (1) {
        readString("Input student number (4-19 digits): ", stu.num, NUM_LEN);
        if (!isNumFormatValid(stu.num)) {
            printf("Invalid number. It must contain 4 to 19 digits.\n");
        } else if (checkDuplicate && isNumExist(head, stu.num)) {
            printf("This student number already exists.\n");
        } else {
            break;
        }
    }
    while (1) {
        readString("Input name (no spaces): ", stu.name, NAME_LEN);
        if (isNameFormatValid(stu.name)) break;
        printf("Invalid name. Please input again.\n");
    }
    inputScores(&stu);
    return stu;
}
void addStudent(Node *head) {
    Student stu;
    printf("\n--- Add Student ---\n");
    stu = inputStudent(head, 1);
    if (appendStudent(head, stu)) {
        printf("Add success.\n");
        printStudentHeader();
        printStudent(stu);
    } else {
        printf("Add failed. Memory may be insufficient or number is duplicate.\n");
    }
}
void deleteByNumMenu(Node *head) {
    char num[NUM_LEN];
    Student removed;
    if (isListEmpty(head)) {
        printf("No student record.\n");
        return;
    }
    readString("Input number to delete: ", num, NUM_LEN);
    if (removeByNum(head, num, &removed)) {
        printf("Delete success. Deleted record:\n");
        printStudentHeader();
        printStudent(removed);
    } else {
        printf("Number not found. Delete failed.\n");
    }
}
void deleteFailedStudents(Node *head) {
    Node *prev, *target;
    int count = 0;
    if (isListEmpty(head)) {
        printf("No student record.\n");
        return;
    }
    printf("This will delete students who fail any subject.\n");
    if (!askYesNo("Confirm delete? (y/n): ")) {
        printf("Batch delete canceled.\n");
        return;
    }
    prev = head;
    /* prev is not advanced when a node is deleted, because prev->next has changed. */
    while (prev->next != NULL) {
        target = prev->next;
        if (target->data.cScore < PASS_SCORE ||
            target->data.mathScore < PASS_SCORE ||
            target->data.englishScore < PASS_SCORE) {
            prev->next = target->next;
            free(target);
            count++;
        } else {
            prev = prev->next;
        }
    }
    printf("Deleted %d failed student records.\n", count);
}
void deleteAllStudents(Node *head) {
    int count;
    if (isListEmpty(head)) {
        printf("No student record.\n");
        return;
    }
    count = getListLength(head);
    printf("There are %d records.\n", count);
    if (!askYesNo("Clear all records? (y/n): ")) {
        printf("Clear canceled.\n");
        return;
    }
    clearList(head);
    printf("All records cleared.\n");
}
static void showDeleteMenu(void) {
    printf("\n--- Delete Student ---\n");
    printf("1. Delete one student by number\n");
    printf("2. Delete students who fail any subject\n");
    printf("3. Clear all records\n");
    printf("0. Return\n");
}
void deleteStudent(Node *head) {
    int choice, running = 1;
    while (running) {
        showDeleteMenu();
        choice = readInt("Choose: ", 0, 3);
        switch (choice) {
            case 1: deleteByNumMenu(head); break;
            case 2: deleteFailedStudents(head); break;
            case 3: deleteAllStudents(head); break;
            case 0: running = 0; break;
            default: printf("Invalid choice.\n"); break;
        }
    }
}
void searchByNumMenu(Node *head) {
    char num[NUM_LEN];
    Node *p;
    readString("Input number: ", num, NUM_LEN);
    p = findByNum(head, num);
    if (p == NULL) {
        printf("Student not found.\n");
    } else {
        printStudentHeader();
        printStudent(p->data);
    }
}
void searchByNameMenu(Node *head) {
    char name[NAME_LEN];
    Node *p = head->next;
    int found = 0;
    readString("Input name: ", name, NAME_LEN);
    while (p != NULL) {
        if (strcmp(p->data.name, name) == 0) {
            if (!found) printStudentHeader();
            printStudent(p->data);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("No student with this name.\n");
}
void searchByFuzzyNameMenu(Node *head) {
    char key[NAME_LEN];
    Node *p = head->next;
    int found = 0;
    readString("Input name keyword: ", key, NAME_LEN);
    while (p != NULL) {
        if (strstr(p->data.name, key) != NULL) {
            if (!found) printStudentHeader();
            printStudent(p->data);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("No student name contains this keyword.\n");
}
void searchByScoreRangeMenu(Node *head) {
    int subject, found = 0;
    float low, high, score;
    Node *p;
    printf("1. C\n2. Math\n3. English\n4. Total\n5. Average\n");
    subject = readInt("Choose subject: ", 1, 5);
    low = readFloat("Input low score: ", 0.0f, 300.0f);
    high = readFloat("Input high score: ", low, 300.0f);
    p = head->next;
    /* Total score can reach 300, while each single subject is within 100. */
    while (p != NULL) {
        score = getStudentScore(p->data, subject);
        if (score >= low && score <= high) {
            if (!found) {
                printf("Subject: %s, range: %.2f - %.2f\n",
                       getSubjectName(subject), low, high);
                printStudentHeader();
            }
            printStudent(p->data);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("No student in this score range.\n");
}
void searchFailedStudentsMenu(Node *head) {
    Node *p = head->next;
    int found = 0;
    while (p != NULL) {
        if (p->data.cScore < PASS_SCORE ||
            p->data.mathScore < PASS_SCORE ||
            p->data.englishScore < PASS_SCORE) {
            if (!found) {
                printf("Students who fail at least one subject:\n");
                printStudentHeader();
            }
            printStudent(p->data);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("No failed student.\n");
}
void searchExcellentStudentsMenu(Node *head) {
    Node *p = head->next;
    int found = 0;
    while (p != NULL) {
        if (p->data.average >= EXCELLENT_SCORE) {
            if (!found) {
                printf("Excellent students by average score:\n");
                printStudentHeader();
            }
            printStudent(p->data);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("No excellent student by average score.\n");
}
static void showSearchMenu(void) {
    printf("\n--- Search Student ---\n");
    printf("1. Search by number\n");
    printf("2. Search by exact name\n");
    printf("3. Search by fuzzy name\n");
    printf("4. Search by score range\n");
    printf("5. Search failed students\n");
    printf("6. Search excellent students\n");
    printf("0. Return\n");
}
void searchStudent(Node *head) {
    int choice, running = 1;
    if (isListEmpty(head)) {
        printf("No student record.\n");
        return;
    }
    while (running) {
        showSearchMenu();
        choice = readInt("Choose: ", 0, 6);
        switch (choice) {
            case 1: searchByNumMenu(head); break;
            case 2: searchByNameMenu(head); break;
            case 3: searchByFuzzyNameMenu(head); break;
            case 4: searchByScoreRangeMenu(head); break;
            case 5: searchFailedStudentsMenu(head); break;
            case 6: searchExcellentStudentsMenu(head); break;
            case 0: running = 0; break;
            default: printf("Invalid choice.\n"); break;
        }
    }
}
void modifyStudent(Node *head) {
    char num[NUM_LEN];
    Node *p;
    if (isListEmpty(head)) {
        printf("No student record.\n");
        return;
    }
    printf("\n--- Modify Student ---\n");
    readString("Input number to modify: ", num, NUM_LEN);
    p = findByNum(head, num);
    if (p == NULL) {
        printf("Number not found.\n");
        return;
    }
    printf("Current record:\n");
    printStudentHeader();
    printStudent(p->data);
    /* The student number is not changed, because it is the key for searching. */
    while (1) {
        readString("Input new name: ", p->data.name, NAME_LEN);
        if (isNameFormatValid(p->data.name)) break;
        printf("Invalid name. Please input again.\n");
    }
    inputScores(&p->data);
    printf("Modify success. New record:\n");
    printStudentHeader();
    printStudent(p->data);
}
void insertStudent(Node *head) {
    char beforeNum[NUM_LEN];
    Student stu;
    printf("\n--- Insert Student ---\n");
    if (isListEmpty(head)) {
        printf("List is empty. The new record will be added as the first record.\n");
        addStudent(head);
        return;
    }
    readString("Input target number to insert before: ", beforeNum, NUM_LEN);
    if (findByNum(head, beforeNum) == NULL) {
        printf("Target number not found. Insert failed.\n");
        return;
    }
    /* Reuse inputStudent so inserted records follow the same validation rules. */
    stu = inputStudent(head, 1);
    if (insertBeforeNum(head, beforeNum, stu)) {
        printf("Insert success.\n");
    } else {
        printf("Insert failed. Memory may be insufficient or number is duplicate.\n");
    }
}
void printStudentHeader(void) {
    printf("%-15s %-15s %10s %10s %10s %10s %10s\n",
           "Number", "Name", "C", "Math", "English", "Total", "Average");
    printf("--------------------------------------------------------------------------------\n");
}
void printStudent(Student stu) {
    printf("%-15s %-15s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
           stu.num, stu.name, stu.cScore, stu.mathScore,
           stu.englishScore, stu.total, stu.average);
}
void printStudentDetail(Student stu) {
    printf("Number: %s\n", stu.num);
    printf("Name: %s\n", stu.name);
    printf("C score: %.2f\n", stu.cScore);
    printf("Math score: %.2f\n", stu.mathScore);
    printf("English score: %.2f\n", stu.englishScore);
    printf("Total: %.2f\n", stu.total);
    printf("Average: %.2f\n", stu.average);
}
void displayStudentDetail(Student stu) {
    printf("\n--- Student Detail ---\n");
    printStudentDetail(stu);
}
void displayAllStudents(Node *head) {
    Node *p;
    int count = 0;
    if (isListEmpty(head)) {
        printf("No student record.\n");
        return;
    }
    printf("\n--- All Student Records ---\n");
    printStudentHeader();
    p = head->next;
    while (p != NULL) {
        printStudent(p->data);
        count++;
        p = p->next;
    }
    printf("Total records: %d\n", count);
}
void displaySimpleStudents(Node *head) {
    Node *p = head == NULL ? NULL : head->next;
    int index = 1;
    if (p == NULL) {
        printf("No student record.\n");
        return;
    }
    printf("%-6s %-15s %-15s\n", "Index", "Number", "Name");
    printf("----------------------------------------\n");
    while (p != NULL) {
        printf("%-6d %-15s %-15s\n", index, p->data.num, p->data.name);
        index++;
        p = p->next;
    }
}
void initSampleData(Node *head) {
    Student samples[] = {
        {"2024001", "ZhangSan", 88, 92, 80, 0, 0},
        {"2024002", "LiSi", 76, 85, 90, 0, 0},
        {"2024003", "WangWu", 60, 70, 65, 0, 0},
        {"2024004", "ZhaoLiu", 95, 91, 94, 0, 0},
        {"2024005", "QianQi", 58, 73, 62, 0, 0},
        {"2024006", "SunBa", 82, 79, 88, 0, 0},
        {"2024007", "ZhouJiu", 67, 55, 72, 0, 0},
        {"2024008", "WuShi", 91, 96, 89, 0, 0}
    };
    int i, count = (int)(sizeof(samples) / sizeof(samples[0]));
    printf("Sample initialization will clear current list.\n");
    if (!askYesNo("Confirm initialization? (y/n): ")) {
        printf("Initialization canceled.\n");
        return;
    }
    /* Sample data is useful for classroom demonstration and quick testing. */
    clearList(head);
    for (i = 0; i < count; i++) {
        calculateScore(&samples[i]);
        appendStudent(head, samples[i]);
    }
    printf("Initialized %d sample records.\n", count);
}
void showProjectHelp(void) {
    printf("\n========== Project Help ==========\n");
    printf("1. Standard C language is used. No database is used.\n");
    printf("2. A singly linked list stores student records in memory.\n");
    printf("3. The system loads data.txt automatically at startup.\n");
    printf("4. Add, delete, search, modify, insert and display are supported.\n");
    printf("5. Sorting supports total, average, single subject and number.\n");
    printf("6. Statistics support max, min, average, pass rate and excellent rate.\n");
    printf("7. File backup, report export and reload are supported.\n");
    printf("==================================\n");
}
void showMemberDivision(void) {
    printf("\n========== Member Division ==========\n");
    printf("Member A: main.c, menu and process control.\n");
    printf("Member B: student.c, linked list and student record management.\n");
    printf("Member C: file.c and analysis.c, file storage, sorting and statistics.\n");
    printf("=====================================\n");
}
void showFileFormatHelp(void) {
    printf("\n========== data.txt Format ==========\n");
    printf("One student per line. Fields are separated by spaces.\n");
    printf("Format: number name C_score math_score English_score\n");
    printf("Example: 2024001 ZhangSan 88 92 80\n");
    printf("Note: name should not contain spaces; score range is 0 to 100.\n");
    printf("=====================================\n");
}
