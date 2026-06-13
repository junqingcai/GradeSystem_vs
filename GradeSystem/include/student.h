#ifndef STUDENT_H
#define STUDENT_H

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

#define SORT_BY_TOTAL_DESC 1
#define SORT_BY_AVERAGE_DESC 2
#define SORT_BY_C_DESC 3
#define SORT_BY_MATH_DESC 4
#define SORT_BY_ENGLISH_DESC 5
#define SORT_BY_NUM_ASC 6
typedef int SortType;

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

Node *createList(void);
void freeList(Node *head);
void clearList(Node *head);
int isListEmpty(Node *head);
int isNumExist(Node *head, const char *num);
int appendStudent(Node *head, Student stu);
int removeByNum(Node *head, const char *num, Student *removed);
void swapStudentData(Node *a, Node *b);

void calculateScore(Student *stu);
int isScoreValid(float score);
int isNumFormatValid(const char *num);
int isNameFormatValid(const char *name);

int loadFromFile(Node *head, const char *filename);
int saveToFile(Node *head, const char *filename);
int exportReport(Node *head, const char *filename);

void sortStudents(Node *head, SortType type);

#ifdef __cplusplus
}
#endif

#endif
