#include <stdio.h>
#include <string.h>

#include "student.h"

/* 判断指定数据文件是否已经存在。 */
static int fileExists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return 0;
    fclose(fp);
    return 1;
}

static int createEmptyDataFile(const char *filename) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) return 0;
    fclose(fp);
    return 1;
}

static int parseStudentLine(const char *line, Student *stu) {
    char extra[LINE_LEN];
    if (line == NULL || stu == NULL) return 0;
    memset(stu, 0, sizeof(Student));
#ifdef _MSC_VER
    int parsed = sscanf_s(line, "%19s %19s %f %f %f %255s",
                          stu->num, (unsigned)NUM_LEN,
                          stu->name, (unsigned)NAME_LEN,
                          &stu->cScore, &stu->mathScore, &stu->englishScore,
                          extra, (unsigned)LINE_LEN);
#else
    int parsed = sscanf(line, "%19s %19s %f %f %f %255s",
                        stu->num, stu->name,
                        &stu->cScore, &stu->mathScore, &stu->englishScore,
                        extra);
#endif
    /* 必须恰好读取五个字段，多余字段或字段不足都视为非法数据。 */
    if (parsed != 5) {
        return 0;
    }
    if (!isNumFormatValid(stu->num) || !isNameFormatValid(stu->name)) return 0;
    if (!isScoreValid(stu->cScore) ||
        !isScoreValid(stu->mathScore) ||
        !isScoreValid(stu->englishScore)) {
        return 0;
    }
    calculateScore(stu);
    return 1;
}

int loadFromFile(Node *head, const char *filename) {
    if (head == NULL || filename == NULL) return 0;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        createEmptyDataFile(filename);
        return 0;
    }

    /* 重新加载前清空原链表，防止重复追加相同记录。 */
    clearList(head);
    char line[LINE_LEN];
    Student stu;
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '\n' || line[0] == '\0') continue;
        /* 非法行或重复学号会被跳过，不影响其余数据继续加载。 */
        if (parseStudentLine(line, &stu) && appendStudent(head, stu)) {
            count++;
        }
    }
    fclose(fp);
    return count;
}

static int backupDataFile(const char *sourceName, const char *backupName) {
    if (sourceName == NULL || backupName == NULL) return 0;
    FILE *src = fopen(sourceName, "r");
    if (src == NULL) return 0;

    FILE *dst = fopen(backupName, "w");
    if (dst == NULL) {
        fclose(src);
        return 0;
    }

    /* 按字节复制，保持原数据文件内容不变。 */
    int ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dst);
    }
    fclose(src);
    fclose(dst);
    return 1;
}

int saveToFile(Node *head, const char *filename) {
    if (head == NULL || filename == NULL) return 0;
    /* 覆盖数据前先保存上一版本，便于误操作后人工恢复。 */
    if (fileExists(filename)) backupDataFile(filename, BACKUP_FILE);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return 0;

    int count = 0;
    for (Node *p = head->next; p != NULL; p = p->next) {
        fprintf(fp, "%s %s %.2f %.2f %.2f\n",
                p->data.num, p->data.name,
                p->data.cScore, p->data.mathScore,
                p->data.englishScore);
        count++;
    }
    fclose(fp);
    return count;
}

static void writeReportHeader(FILE *fp) {
    fprintf(fp, "学生成绩管理系统报告\n");
    fprintf(fp, "========================================\n");
    fprintf(fp, "%-15s %-15s %10s %10s %10s %10s %10s\n",
            "学号", "姓名", "C语言", "数学", "英语", "总分", "平均分");
    fprintf(fp, "--------------------------------------------------------------------------------\n");
}

static void writeReportStudent(FILE *fp, Student stu) {
    fprintf(fp, "%-15s %-15s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
            stu.num, stu.name, stu.cScore, stu.mathScore,
            stu.englishScore, stu.total, stu.average);
}

int exportReport(Node *head, const char *filename) {
    if (head == NULL || filename == NULL) return 0;
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return 0;

    /* 导出文件包含表头、逐条记录以及记录总数。 */
    writeReportHeader(fp);
    int count = 0;
    for (Node *p = head->next; p != NULL; p = p->next) {
        writeReportStudent(fp, p->data);
        count++;
    }
    fprintf(fp, "--------------------------------------------------------------------------------\n");
    fprintf(fp, "总记录： %d\n", count);
    fclose(fp);
    return count;
}
