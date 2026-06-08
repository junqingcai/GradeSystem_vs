#include <stdio.h>
#include <string.h>
#include "student.h"
// 根据当前选择的排序规则，判断两条学生记录是否需要交换位置。需要交换则返回 1
static int shouldSwap(Student left, Student right, SortType type) {
    switch (type) {
        case SORT_BY_TOTAL_DESC: return left.total < right.total;
        case SORT_BY_AVERAGE_DESC: return left.average < right.average;
        case SORT_BY_C_DESC: return left.cScore < right.cScore;
        case SORT_BY_MATH_DESC: return left.mathScore < right.mathScore;
        case SORT_BY_ENGLISH_DESC: return left.englishScore < right.englishScore;
        case SORT_BY_NUM_ASC: return strcmp(left.num, right.num) > 0;
        case SORT_BY_C_ASC: return left.cScore > right.cScore;
        case SORT_BY_MATH_ASC: return left.mathScore > right.mathScore;
        case SORT_BY_ENGLISH_ASC: return left.englishScore > right.englishScore;
        default: return 0;
    }
}
static const char *getSortTypeName(SortType type) {
    switch (type) {
        case SORT_BY_TOTAL_DESC: return "按总分从高到低排序";
        case SORT_BY_AVERAGE_DESC: return "按平均分从高到低排序";
        case SORT_BY_C_DESC: return "按C语言分数从高到低排序";
        case SORT_BY_MATH_DESC: return "按数学分数从高到低排序";
        case SORT_BY_ENGLISH_DESC: return "按英语分数从高到低排序";
        case SORT_BY_NUM_ASC: return "按学号从小到大排序";
        case SORT_BY_C_ASC: return "按C语言分数从低到高排序";
        case SORT_BY_MATH_ASC: return "按数学分数从低到高排序";
        case SORT_BY_ENGLISH_ASC: return "按英语分数从高到低排序";
        default: return "未知";
    }
}
void sortStudents(Node *head, SortType type) {
    Node *i, *j;
    if (isListEmpty(head)) {
        printf("没有学生记录。\n");
        return;
    }
    /* 交换节点中的数据，而不是改变链表指针，这对初学者更容易理解。 */
    for (i = head->next; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (shouldSwap(i->data, j->data, type)) {
                swapStudentData(i, j);
            }
        }
    }
    printf("已按 %s 排序。\n", getSortTypeName(type));
    displayAllStudents(head);
}
static void showSortMenu(void) {
    printf("\n--- 成绩排序功能 ---\n");
    printf("1. 总分降序\n");
    printf("2. 平均分降序\n");
    printf("3. C语言成绩降序\n");
    printf("4. 数学成绩降序\n");
    printf("5. 英语成绩降序\n");
    printf("6. 学号升序\n");
    printf("7. C语言成绩升序\n");
    printf("8. 数学成绩升序\n");
    printf("9. 英语成绩升序\n");
    printf("0. 返回\n");
}
void sortMenu(Node *head) {
    int choice, running = 1;
    if (isListEmpty(head)) {
        printf("没有学生记录。\n");
        return;
    }
    while (running) {
        showSortMenu();
        choice = readInt("选择： ", 0, 9);
        if (choice == 0) running = 0;
        else sortStudents(head, (SortType)choice);
    }
}
static int getRangeIndex(float score) {
    /* 0: 90-100, 1: 80-89, 2: 70-79, 3: 60-69, 4: below 60 */
    if (score >= 90.0f) return 0;
    if (score >= 80.0f) return 1;
    if (score >= 70.0f) return 2;
    if (score >= 60.0f) return 3;
    return 4;
}
static void initSubjectStat(SubjectStat *stat) {
    int i;
    stat->maxScore = -1.0f;
    stat->minScore = 101.0f;
    stat->sum = 0.0f;
    stat->passCount = 0;
    stat->excellentCount = 0;
    memset(&stat->maxStudent, 0, sizeof(Student));
    memset(&stat->minStudent, 0, sizeof(Student));
    for (i = 0; i < 5; i++) stat->ranges[i] = 0;
}
/* 更新某一科目的最高分、最低分、总分、及格/优秀人数和分数段人数。 */
static void updateSubjectStat(SubjectStat *stat, Student stu, float score) {
    int rangeIndex;
    if (score > stat->maxScore) {
        stat->maxScore = score;
        stat->maxStudent = stu;
    }
    if (score < stat->minScore) {
        stat->minScore = score;
        stat->minStudent = stu;
    }
    stat->sum += score;
    if (score >= PASS_SCORE) stat->passCount++;
    if (score >= EXCELLENT_SCORE) stat->excellentCount++;
    rangeIndex = getRangeIndex(score);
    stat->ranges[rangeIndex]++;
}
static void collectStatistics(Node *head,
                              SubjectStat *cStat,
                              SubjectStat *mathStat,
                              SubjectStat *engStat,
                              int *count,
                              float *totalSum) {
    Node *p;
    /* 三门科目使用相同的统计逻辑。 */
    initSubjectStat(cStat);
    initSubjectStat(mathStat);
    initSubjectStat(engStat);
    *count = 0;
    *totalSum = 0.0f;
    p = head->next;
    while (p != NULL) {
        updateSubjectStat(cStat, p->data, p->data.cScore);
        updateSubjectStat(mathStat, p->data, p->data.mathScore);
        updateSubjectStat(engStat, p->data, p->data.englishScore);
        *totalSum += p->data.total;
        (*count)++;
        p = p->next;
    }
}
static void printSubjectStatLine(const char *subjectName,
                                 SubjectStat stat,
                                 int count) {
    float average = 0.0f, passRate = 0.0f, excellentRate = 0.0f;
    if (count > 0) {
        average = stat.sum / count;
        passRate = stat.passCount * 100.0f / count;
        excellentRate = stat.excellentCount * 100.0f / count;
    }
    printf("%-10s %10.2f %10.2f %10.2f %10d %9.2f%% %10d %9.2f%%\n",
           subjectName,
           stat.maxScore,
           stat.minScore,
           average,
           stat.passCount,
           passRate,
           stat.excellentCount,
           excellentRate);
}
void displaySubjectStatistics(Node *head) {
    SubjectStat cStat, mathStat, engStat;
    int count;
    float totalSum;
    if (isListEmpty(head)) {
        printf("没有学生记录。\n");
        return;
    }
    collectStatistics(head, &cStat, &mathStat, &engStat, &count, &totalSum);
    printf("\n--- 科目统计 ---\n");
    printf("%-10s %10s %10s %10s %10s %10s %10s %10s\n",
           "科目", "最高分", "最低分", "平均分", "及格人数", "及格率", "优秀人数", "优秀率");
    printf("--------------------------------------------------------------------------------\n");
    printSubjectStatLine("C语言", cStat, count);
    printSubjectStatLine("数学", mathStat, count);
    printSubjectStatLine("英语", engStat, count);
}
static void printRangeLine(const char *subjectName, SubjectStat stat) {
    printf("%-10s %10d %10d %10d %10d %10d\n",
           subjectName,
           stat.ranges[0],
           stat.ranges[1],
           stat.ranges[2],
           stat.ranges[3],
           stat.ranges[4]);
}
void displayDistribution(Node *head) {
    SubjectStat cStat, mathStat, engStat;
    int count;
    float totalSum;
    if (isListEmpty(head)) {
        printf("没有学生记录\n");
        return;
    }
    collectStatistics(head, &cStat, &mathStat, &engStat, &count, &totalSum);
    printf("\n--- 成绩分布 ---\n");
    printf("%-10s %10s %10s %10s %10s %10s\n",
           "学科", "90-100", "80-89", "70-79", "60-69", "Below60");
    printf("--------------------------------------------------------------------\n");
    printRangeLine("C语言", cStat);
    printRangeLine("数学", mathStat);
    printRangeLine("英语", engStat);
}
static void printTopBottomForSubject(const char *subjectName, SubjectStat stat) {
    printf("\n%s 最高分: %.2f, 学生: %s %s\n",
           subjectName, stat.maxScore, stat.maxStudent.num, stat.maxStudent.name);
    printf("%s 最低分: %.2f, 学生: %s %s\n",
           subjectName, stat.minScore, stat.minStudent.num, stat.minStudent.name);
}
void displayTopAndBottom(Node *head) {
    SubjectStat cStat, mathStat, engStat;
    int count;
    float totalSum;
    Node *p;
    Student totalMax, totalMin;
    if (isListEmpty(head)) {
        printf("没有学生记录\n");
        return;
    }
    collectStatistics(head, &cStat, &mathStat, &engStat, &count, &totalSum);
    printTopBottomForSubject("C语言", cStat);
    printTopBottomForSubject("数学", mathStat);
    printTopBottomForSubject("英语", engStat);
    p = head->next;
    totalMax = p->data;
    totalMin = p->data;
    while (p != NULL) {
        if (p->data.total > totalMax.total) totalMax = p->data;
        if (p->data.total < totalMin.total) totalMin = p->data;
        p = p->next;
    }
    printf("\n总分最高: %.2f, 学生: %s %s\n",
           totalMax.total, totalMax.num, totalMax.name);
    printf("总分最低: %.2f, 学生: %s %s\n",
           totalMin.total, totalMin.num, totalMin.name);
}
void displayClassSummary(Node *head) {
    Node *p;
    int count = 0, allPassCount = 0, allExcellentCount = 0;
    float totalSum = 0.0f;
    Student bestAverage, worstAverage;
    if (isListEmpty(head)) {
        printf("没有学生记录。\n");
        return;
    }
    p = head->next;
    /* 用第一个实际节点初始化最高分和最低分学生。 */
    bestAverage = p->data;
    worstAverage = p->data;
    while (p != NULL) {
        totalSum += p->data.total;
        if (p->data.cScore >= PASS_SCORE &&
            p->data.mathScore >= PASS_SCORE &&
            p->data.englishScore >= PASS_SCORE) {
            allPassCount++;
        }
        if (p->data.average >= EXCELLENT_SCORE) allExcellentCount++;
        if (p->data.average > bestAverage.average) bestAverage = p->data;
        if (p->data.average < worstAverage.average) worstAverage = p->data;
        count++;
        p = p->next;
    }
    printf("\n--- 班级汇总---\n");
    printf("学生人数: %d\n", count);
    printf("班级总分平均分: %.2f\n", totalSum / (count * 3.0f));
    printf("三科全部及格人数: %d\n", allPassCount);
    printf("平均分达到优秀的人数: %d\n", allExcellentCount);
    printf("平均分最高的学生： %s %s %.2f\n",
           bestAverage.num, bestAverage.name, bestAverage.average);
    printf("平均分最低的学生 %s %s %.2f\n",
           worstAverage.num, worstAverage.name, worstAverage.average);
}
static void showStatisticMenu(void) {
    printf("\n--- 成绩数据 ---\n");
    printf("1. 各科最高分/最低分/平均分/及格情况/优秀情况\n");
    printf("2. 成绩分布\n");
    printf("3. 最高分与最低分学生\n");
    printf("4. 班级总结\n");
    printf("5. 全部数据\n");
    printf("0. 返回\n");
}
void statisticMenu(Node *head) {
    int choice, running = 1;
    if (isListEmpty(head)) {
        printf("没有学生记录。\n");
        return;
    }
    while (running) {
        showStatisticMenu();
        choice = readInt("选择： ", 0, 5);
        switch (choice) {
            case 1: displaySubjectStatistics(head); break;
            case 2: displayDistribution(head); break;
            case 3: displayTopAndBottom(head); break;
            case 4: displayClassSummary(head); break;
            case 5:
                displaySubjectStatistics(head);
                displayDistribution(head);
                displayTopAndBottom(head);
                displayClassSummary(head);
                break;
            case 0: running = 0; break;
            default: printf("无效选择。\n"); break;
        }
    }
}
