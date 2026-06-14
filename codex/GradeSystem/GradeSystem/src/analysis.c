#include <string.h>

#include "student.h"

/* 根据排序类型判断左右两条记录是否需要交换。 */
static int shouldSwap(Student left, Student right, SortType type) {
    switch (type) {
        case SORT_BY_TOTAL_DESC: return left.total < right.total;
        case SORT_BY_AVERAGE_DESC: return left.average < right.average;
        case SORT_BY_C_DESC: return left.cScore < right.cScore;
        case SORT_BY_MATH_DESC: return left.mathScore < right.mathScore;
        case SORT_BY_ENGLISH_DESC: return left.englishScore < right.englishScore;
        case SORT_BY_NUM_ASC: return strcmp(left.num, right.num) > 0;
        default: return 0;
    }
}

void sortStudents(Node *head, SortType type) {
    if (isListEmpty(head)) return;
    /*
     * 数据量较小，使用直观的双重循环排序。
     * 只交换 Student 数据，不改变链表结点之间的连接关系。
     */
    for (Node *i = head->next; i != NULL; i = i->next) {
        for (Node *j = i->next; j != NULL; j = j->next) {
            if (shouldSwap(i->data, j->data, type)) {
                swapStudentData(i, j);
            }
        }
    }
}
