#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "student.h"

/* 创建不保存学生数据的头结点，便于统一处理首结点的增删操作。 */
Node *createList(void) {
    Node *head = (Node *)malloc(sizeof(Node));
    if (head != NULL) {
        memset(&head->data, 0, sizeof(Student));
        head->next = NULL;
    }
    return head;
}

static Node *createNode(Student stu) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) return NULL;
    node->data = stu;
    node->next = NULL;
    return node;
}

/* 释放头结点以及后续所有学生结点。 */
void freeList(Node *head) {
    Node *p = head;
    while (p != NULL) {
        Node *next = p->next;
        free(p);
        p = next;
    }
}

/* 仅清空学生结点，保留头结点供程序继续使用。 */
void clearList(Node *head) {
    if (head == NULL) return;
    Node *p = head->next;
    while (p != NULL) {
        Node *next = p->next;
        free(p);
        p = next;
    }
    head->next = NULL;
}

int isListEmpty(Node *head) {
    return head == NULL || head->next == NULL;
}

static Node *getLastNode(Node *head) {
    Node *p = head;
    if (p == NULL) return NULL;
    while (p->next != NULL) p = p->next;
    return p;
}

static Node *findPrevByNum(Node *head, const char *num) {
    if (head == NULL || num == NULL) return NULL;
    Node *p = head;
    while (p->next != NULL) {
        if (strcmp(p->next->data.num, num) == 0) return p;
        p = p->next;
    }
    return NULL;
}

/* 通过前驱结点定位目标，可复用删除操作所需的查找结果。 */
static Node *findByNum(Node *head, const char *num) {
    Node *prev = findPrevByNum(head, num);
    return prev == NULL ? NULL : prev->next;
}

int isNumExist(Node *head, const char *num) {
    return findByNum(head, num) != NULL;
}

int appendStudent(Node *head, Student stu) {
    /* 学号在系统中作为唯一标识，禁止插入重复记录。 */
    if (head == NULL || isNumExist(head, stu.num)) return 0;
    Node *node = createNode(stu);
    if (node == NULL) return 0;
    Node *last = getLastNode(head);
    if (last == NULL) {
        free(node);
        return 0;
    }
    last->next = node;
    return 1;
}

int removeByNum(Node *head, const char *num, Student *removed) {
    if (head == NULL || num == NULL) return 0;
    Node *prev = findPrevByNum(head, num);
    if (prev == NULL) return 0;
    Node *target = prev->next;
    /* 调用者需要撤销或显示时，可以取得被删除记录的副本。 */
    if (removed != NULL) *removed = target->data;
    prev->next = target->next;
    free(target);
    return 1;
}

void swapStudentData(Node *a, Node *b) {
    if (a == NULL || b == NULL) return;
    Student temp = a->data;
    a->data = b->data;
    b->data = temp;
}

int isScoreValid(float score) {
    return score >= 0.0f && score <= 100.0f;
}

int isNumFormatValid(const char *num) {
    if (num == NULL) return 0;
    int len = (int)strlen(num);
    if (len < 4 || len >= NUM_LEN) return 0;
    /* 学号限定为 4 到 19 位数字。 */
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)num[i])) return 0;
    }
    return 1;
}

int isNameFormatValid(const char *name) {
    if (name == NULL) return 0;
    int len = (int)strlen(name);
    if (len <= 0 || len >= NAME_LEN) return 0;
    /* 数据文件以空格分隔字段，因此姓名中不允许出现空白字符。 */
    for (int i = 0; i < len; i++) {
        if (isspace((unsigned char)name[i])) return 0;
    }
    return 1;
}

void calculateScore(Student *stu) {
    if (stu == NULL) return;
    stu->total = stu->cScore + stu->mathScore + stu->englishScore;
    stu->average = stu->total / 3.0f;
}
