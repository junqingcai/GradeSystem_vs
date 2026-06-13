#include <string.h>
#include <wchar.h>

#include "app_logic.h"

int parseScoreText(const wchar_t *text, float *score) {
    wchar_t *end = NULL;
    float value;

    if (text == NULL || score == NULL || text[0] == L'\0') return 0;
    value = wcstof(text, &end);
    if (end == text || *end != L'\0' || !isScoreValid(value)) return 0;
    *score = value;
    return 1;
}

int setStudentNumber(Node *head, Student *student, const char *number) {
    size_t length;

    if (head == NULL || student == NULL || !isNumFormatValid(number)) return 0;
    if (isNumExist(head, number)) return 0;
    length = strlen(number);
    memcpy(student->num, number, length + 1);
    return 1;
}

int setStudentName(Student *student, const char *name) {
    size_t length;

    if (student == NULL || !isNameFormatValid(name)) return 0;
    length = strlen(name);
    memcpy(student->name, name, length + 1);
    return 1;
}

int studentMatchesFilter(const Student *student, const char *filter) {
    if (student == NULL) return 0;
    if (filter == NULL || filter[0] == '\0') return 1;
    return strstr(student->num, filter) != NULL ||
           strstr(student->name, filter) != NULL;
}

int countFilteredStudents(Node *head, const char *filter) {
    int count = 0;
    Node *node;

    if (head == NULL) return 0;
    for (node = head->next; node != NULL; node = node->next) {
        if (studentMatchesFilter(&node->data, filter)) count++;
    }
    return count;
}

Node *getFilteredStudentAt(Node *head, const char *filter, int index) {
    int current = 0;
    Node *node;

    if (head == NULL || index < 0) return NULL;
    for (node = head->next; node != NULL; node = node->next) {
        if (!studentMatchesFilter(&node->data, filter)) continue;
        if (current == index) return node;
        current++;
    }
    return NULL;
}

int calculatePageCount(int itemCount, int pageSize) {
    if (pageSize <= 0 || itemCount <= 0) return 1;
    return (itemCount + pageSize - 1) / pageSize;
}

int isSortTypeValid(int value) {
    return value >= SORT_BY_TOTAL_DESC && value <= SORT_BY_NUM_ASC;
}

void calculateStudentStats(Node *head, StudentStats *stats) {
    float averageSum = 0.0f;
    Node *node;

    if (stats == NULL) return;
    memset(stats, 0, sizeof(*stats));
    if (head == NULL) return;

    for (node = head->next; node != NULL; node = node->next) {
        Student *student = &node->data;
        stats->count++;
        averageSum += student->average;
        if (student->cScore >= PASS_SCORE &&
            student->mathScore >= PASS_SCORE &&
            student->englishScore >= PASS_SCORE) {
            stats->allPass++;
        }
        if (student->average >= EXCELLENT_SCORE) stats->excellent++;
        if (!stats->hasBest || student->average > stats->best.average) {
            stats->best = *student;
            stats->hasBest = 1;
        }
    }

    if (stats->count > 0) {
        stats->classAverage = averageSum / stats->count;
        stats->passRate = stats->allPass * 100.0f / stats->count;
        stats->excellentRate = stats->excellent * 100.0f / stats->count;
    }
}
