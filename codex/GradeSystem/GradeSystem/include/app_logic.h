#ifndef APP_LOGIC_H
#define APP_LOGIC_H

/* 与 EasyX 无关的界面业务逻辑，使用 C 语言实现。 */

#include <wchar.h>

#include "student.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 班级统计页一次绘制所需的汇总数据。 */
typedef struct StudentStats {
    int count;               /* 学生总人数。 */
    int allPass;             /* 三科全部及格人数。 */
    int excellent;           /* 平均分达到优秀标准的人数。 */
    float classAverage;      /* 全班平均分。 */
    float passRate;          /* 三科全部及格率。 */
    float excellentRate;     /* 优秀率。 */
    int hasBest;             /* 是否存在最高平均分学生。 */
    Student best;            /* 平均分最高的学生。 */
} StudentStats;

/* 输入文本校验及学生字段安全写入。 */
int parseScoreText(const wchar_t *text, float *score);
int setStudentNumber(Node *head, Student *student, const char *number);
int setStudentName(Student *student, const char *name);

/* 搜索、分页及按显示索引访问学生。 */
int studentMatchesFilter(const Student *student, const char *filter);
int countFilteredStudents(Node *head, const char *filter);
Node *getFilteredStudentAt(Node *head, const char *filter, int index);
int calculatePageCount(int itemCount, int pageSize);

/* 排序选项校验与班级统计。 */
int isSortTypeValid(int value);
void calculateStudentStats(Node *head, StudentStats *stats);

#ifdef __cplusplus
}
#endif

#endif
