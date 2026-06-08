#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
//在备份和加载前，先检查数据文件是否存在
int fileExists(const char *filename) {
    FILE *fp = fopen(filename, "r"); //如果能以只读方式打开文件，说明文件存在；否则不存在。
    if (fp == NULL) return 0;
    fclose(fp);
    return 1;
}
//第一次运行程序时，若data,txt不存在，则创建一个空的数据文件
int createEmptyDataFile(const char *filename) {
    FILE *fp = fopen(filename, "a"); //"a" 表示追加模式。如果文件不存在，就会自动创建；如果文件已经存在，不会清空原文件。
    if (fp == NULL) {
        printf("文件创建失败 %s.\n", filename);
        return 0;
    }
    fclose(fp);
    return 1;
}
//将一行文本转换为 Student 结构体，并验证所有字段是否合法
static int parseStudentLine(const char *line, Student *stu) {
    char extra[LINE_LEN];
    if (line == NULL || stu == NULL) return 0;
    memset(stu, 0, sizeof(Student));
    if (sscanf(line, "%19s %19s %f %f %f %255s",
               stu->num, stu->name,
               &stu->cScore, &stu->mathScore, &stu->englishScore,
               extra) != 5) {
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
    FILE *fp;
    char line[LINE_LEN];
    Student stu;
    int count = 0, lineNumber = 0, badCount = 0, duplicateCount = 0;
    if (head == NULL || filename == NULL) return 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("%s 未找到，正在创建一个空的数据文件\n", filename);
        createEmptyDataFile(filename);
        return 0;
    }
    // 重新加载时应该替换旧的内存数据，因此需要先清空链表。
    clearList(head);
    while (fgets(line, sizeof(line), fp) != NULL) {
        lineNumber++;
        if (line[0] == '\n' || line[0] == '\0') continue;
        if (!parseStudentLine(line, &stu)) {
            printf("第 %d 行格式无效，已跳过。\n", lineNumber);
            badCount++;
            continue;
        }
        if (!appendStudent(head, stu)) {
            printf("第 %d 行数据重复或无法加载。\n", lineNumber);
            duplicateCount++;
            continue;
        }
        count++;
    }
    fclose(fp);
    printf("已加载 %d 条记录，无效记录：%d，重复记录：%d。\n",
           count, badCount, duplicateCount);
    return count;
}
//把源文件复制到备份文件中
int backupDataFile(const char *sourceName, const char *backupName) {
    FILE *src, *dst;
    int ch, count = 0;
    if (sourceName == NULL || backupName == NULL) return 0;
    src = fopen(sourceName, "r");
    if (src == NULL) {
        printf("源文件 %s 不存在，备份失败。\n", sourceName);
        return 0;
    }
    dst = fopen(backupName, "w");
    if (dst == NULL) {
        fclose(src);
        printf("无法创建备份文件 %s。\n", backupName);
        return 0;
    }
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dst);
        count++;
    }
    fclose(src);
    fclose(dst);
    printf("已将 %d 个字符备份到 %s。\n", count, backupName);
    return 1;
}
//把链表中的学生数据保存到文件中
int saveToFile(Node *head, const char *filename) {
    FILE *fp;
    Node *p;
    int count = 0;
    if (head == NULL || filename == NULL) return 0;
    // 覆盖 data.txt 之前，先备份旧数据。
    if (fileExists(filename)) backupDataFile(filename, BACKUP_FILE);
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开 %s，保存失败。\n", filename);
        return 0;
    }
    p = head->next;
    // 只保存基本字段；总分和平均分会在加载后重新计算。
    while (p != NULL) {
        fprintf(fp, "%s %s %.2f %.2f %.2f\n",
                p->data.num, p->data.name,
                p->data.cScore, p->data.mathScore,
                p->data.englishScore);
        count++;
        p = p->next;
    }
    fclose(fp);
    printf("已将 %d 条记录保存到 %s。\n", count, filename);
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
//导出一个更适合查看的成绩报表
int exportReport(Node *head, const char *filename) {
    FILE *fp;
    Node *p;
    int count = 0;
    if (head == NULL || filename == NULL) return 0;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法创建导出文件 %s。\n", filename);
        return 0;
    }
    writeReportHeader(fp);
    p = head->next;
    while (p != NULL) {
        writeReportStudent(fp, p->data);
        count++;
        p = p->next;
    }
    fprintf(fp, "--------------------------------------------------------------------------------\n");
    fprintf(fp, "总记录： %d\n", count);
    fclose(fp);
    printf("已将 %d 条记录导出到 %s。\n", count, filename);
    return count;
}
//文件管理模块的菜单函数
static void showFileMenu(void) {
    printf("\n--- 文件管理 ---\n");
    printf("1.保存到 data.txt\n");
    printf("2.重新加载 data.txt\n");
    printf("3.将 data.txt 备份到 backup.txt\n");
    printf("4.导出报告到 export.txt\n");
    printf("5.查看文件格式说明\n");
    printf("0.返回\n");
}
void fileMenu(Node *head) {
    int choice;
    int running = 1;
    while (running) {
        showFileMenu();
        choice = readInt("选择: ", 0, 5);
        switch (choice) {
            case 1: saveToFile(head, DATA_FILE); break;
            case 2:
                if (askYesNo("重新加载将覆盖尚未保存的内存数据。是否继续？(y/n):")) {
                    loadFromFile(head, DATA_FILE);
                }
                break;
            case 3: backupDataFile(DATA_FILE, BACKUP_FILE); break;
            case 4: exportReport(head, EXPORT_FILE); break;
            case 5: showFileFormatHelp(); break;
            case 0: running = 0; break;
            default: printf("无效选择。\n"); break;
        }
    }
}
