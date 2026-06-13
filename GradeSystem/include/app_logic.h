#ifndef APP_LOGIC_H
#define APP_LOGIC_H

#include <wchar.h>

#include "student.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StudentStats {
    int count;
    int allPass;
    int excellent;
    float classAverage;
    float passRate;
    float excellentRate;
    int hasBest;
    Student best;
} StudentStats;

int parseScoreText(const wchar_t *text, float *score);
int setStudentNumber(Node *head, Student *student, const char *number);
int setStudentName(Student *student, const char *name);

int studentMatchesFilter(const Student *student, const char *filter);
int countFilteredStudents(Node *head, const char *filter);
Node *getFilteredStudentAt(Node *head, const char *filter, int index);
int calculatePageCount(int itemCount, int pageSize);

int isSortTypeValid(int value);
void calculateStudentStats(Node *head, StudentStats *stats);

#ifdef __cplusplus
}
#endif

#endif
