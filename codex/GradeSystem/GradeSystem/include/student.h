#ifndef STUDENT_H
#define STUDENT_H

/* 学生数据、链表、文件读写和排序模块的公共接口。 */

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_LEN 20       /* 学号缓冲区长度，包含字符串结束符。 */
#define NAME_LEN 20      /* 姓名缓冲区长度，按 UTF-8 字节计算。 */
#define LINE_LEN 256     /* 读取数据文件时单行缓冲区长度。 */

/* 程序运行目录中的数据文件名称。 */
#define DATA_FILE "data.txt"
#define BACKUP_FILE "backup.txt"
#define EXPORT_FILE "export.txt"

/* 成绩判断标准。 */
#define PASS_SCORE 60.0f
#define EXCELLENT_SCORE 90.0f

/* 排序类型编号，与图形界面中的排序选项保持一致。 */
#define SORT_BY_TOTAL_DESC 1
#define SORT_BY_AVERAGE_DESC 2
#define SORT_BY_C_DESC 3
#define SORT_BY_MATH_DESC 4
#define SORT_BY_ENGLISH_DESC 5
#define SORT_BY_NUM_ASC 6
typedef int SortType;

/* 单名学生的基本信息及自动计算出的总分、平均分。 */
typedef struct Student {
    char num[NUM_LEN];
    char name[NAME_LEN];
    float cScore;
    float mathScore;
    float englishScore;
    float total;
    float average;
} Student;

/* 带头结点单向链表的结点结构。 */
typedef struct Node {
    Student data;
    struct Node *next;
} Node;

/* 链表的创建、释放和增删查操作。 */
Node *createList(void);
void freeList(Node *head);
void clearList(Node *head);
int isListEmpty(Node *head);
int isNumExist(Node *head, const char *num);
int appendStudent(Node *head, Student stu);
int removeByNum(Node *head, const char *num, Student *removed);
void swapStudentData(Node *a, Node *b);

/* 学生字段校验和派生成绩计算。 */
void calculateScore(Student *stu);
int isScoreValid(float score);
int isNumFormatValid(const char *num);
int isNameFormatValid(const char *name);

/* 数据持久化和报表导出。 */
int loadFromFile(Node *head, const char *filename);
int saveToFile(Node *head, const char *filename);
int exportReport(Node *head, const char *filename);

/* 按指定规则对链表中的学生数据排序。 */
void sortStudents(Node *head, SortType type);

#ifdef __cplusplus
}
#endif

#endif
