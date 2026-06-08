#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_LEN 20
#define NAME_LEN 20
#define LINE_LEN 256

#define DATA_FILE "data.txt"
#define BACKUP_FILE "backup.txt"
#define EXPORT_FILE "export.txt"

#define PASS_SCORE 60.0f
#define EXCELLENT_SCORE 90.0f

#define SUBJECT_C 1
#define SUBJECT_MATH 2
#define SUBJECT_ENGLISH 3
#define SUBJECT_TOTAL 4
#define SUBJECT_AVERAGE 5

typedef struct Student {
    char num[NUM_LEN];
    char name[NAME_LEN];
    float cScore;
    float mathScore;
    float englishScore;
    float total;
    float average;
} Student;

typedef struct Node {
    Student data;
    struct Node *next;
} Node;

typedef enum SortType {
    SORT_BY_TOTAL_DESC = 1,
    SORT_BY_AVERAGE_DESC,
    SORT_BY_C_DESC,
    SORT_BY_MATH_DESC,
    SORT_BY_ENGLISH_DESC,
    SORT_BY_NUM_ASC,
    SORT_BY_C_ASC,
    SORT_BY_MATH_ASC,
    SORT_BY_ENGLISH_ASC
} SortType;

typedef struct SubjectStat {
    float maxScore;
    float minScore;
    float sum;
    int passCount;
    int excellentCount;
    int ranges[5];
    Student maxStudent;
    Student minStudent;
} SubjectStat;

void clearInputBuffer(void);
void pauseScreen(void);
int askYesNo(const char *prompt);
int readInt(const char *prompt, int minValue, int maxValue);
float readFloat(const char *prompt, float minValue, float maxValue);
float readScore(const char *prompt);
void readString(const char *prompt, char *buffer, int size);

Node *createList(void);
Node *createNode(Student stu);
void freeList(Node *head);
void clearList(Node *head);
int isListEmpty(Node *head);
int getListLength(Node *head);
Node *getLastNode(Node *head);
Node *getNodeByIndex(Node *head, int index);
Node *findPrevByNum(Node *head, const char *num);
Node *findByNum(Node *head, const char *num);
int isNumExist(Node *head, const char *num);
int isNameExist(Node *head, const char *name);
int appendStudent(Node *head, Student stu);
int insertBeforeNum(Node *head, const char *beforeNum, Student stu);
int removeByNum(Node *head, const char *num, Student *removed);
void swapStudentData(Node *a, Node *b);

void calculateScore(Student *stu);
int isScoreValid(float score);
int isNumFormatValid(const char *num);
int isNameFormatValid(const char *name);
float getStudentScore(Student stu, int subject);
const char *getSubjectName(int subject);
Student inputStudent(Node *head, int checkDuplicate);
void inputScores(Student *stu);
void copyStudent(Student *dest, Student src);

void addStudent(Node *head);
void deleteStudent(Node *head);
void deleteByNumMenu(Node *head);
void deleteFailedStudents(Node *head);
void deleteAllStudents(Node *head);
void searchStudent(Node *head);
void searchByNumMenu(Node *head);
void searchByNameMenu(Node *head);
void searchByFuzzyNameMenu(Node *head);
void searchByScoreRangeMenu(Node *head);
void searchFailedStudentsMenu(Node *head);
void searchExcellentStudentsMenu(Node *head);
void modifyStudent(Node *head);
void insertStudent(Node *head);
void displayAllStudents(Node *head);
void displayStudentDetail(Student stu);
void displaySimpleStudents(Node *head);
void printStudentHeader(void);
void printStudent(Student stu);
void printStudentDetail(Student stu);

void initSampleData(Node *head);
void showProjectHelp(void);
void showMemberDivision(void);
void showFileFormatHelp(void);

int loadFromFile(Node *head, const char *filename);
int saveToFile(Node *head, const char *filename);
int backupDataFile(const char *sourceName, const char *backupName);
int exportReport(Node *head, const char *filename);
int createEmptyDataFile(const char *filename);
int fileExists(const char *filename);
void fileMenu(Node *head);

void sortMenu(Node *head);
void statisticMenu(Node *head);
void sortStudents(Node *head, SortType type);
void displayTopAndBottom(Node *head);
void displaySubjectStatistics(Node *head);
void displayDistribution(Node *head);
void displayClassSummary(Node *head);

#ifdef __cplusplus
}
#endif

#endif
