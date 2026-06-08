# 学生成绩管理系统

## 一、项目简介

本项目是一个使用 C/C++ 编写的“学生成绩管理系统”课程设计项目。程序使用 EasyX 图形界面运行，使用单向链表在内存中保存学生成绩信息，并通过 `data.txt` 文件实现数据的读取和保存。

## 二、主要功能

系统通过 EasyX 窗口提供学生列表、搜索、添加、修改、删除、排序、保存、重新加载、报表导出和成绩统计等操作。

已实现功能包括：

- 添加学生成绩记录
- 按学号删除学生记录
- 批量删除存在不及格科目的学生
- 清空全部学生记录
- 按学号查询
- 按姓名精确查询
- 按姓名关键字模糊查询
- 按分数区间查询
- 查询不及格学生
- 查询优秀学生
- 按学号修改学生姓名和成绩
- 在指定学号前插入学生记录
- 显示全部学生记录
- 按总分、平均分、单科成绩、学号排序
- 统计每科最高分、最低分、平均分
- 统计每科及格人数、优秀人数、及格率、优秀率
- 统计分数段人数
- 显示各科最高分和最低分学生
- 显示班级总览
- 保存数据到 `data.txt`
- 从 `data.txt` 重新读取数据
- 自动备份数据到 `backup.txt`
- 导出报表到 `export.txt`

## 三、项目文件结构

```text
GradeSystem/
├── CMakeLists.txt
├── README.md
├── main.c
├── student.h
├── student.c
├── file.c
├── analysis.c
├── data.txt
└── CLion运行说明.txt
```

各文件作用：

| 文件 | 作用 |
|---|---|
| `main.cpp` | EasyX 图形界面入口、窗口绘制、鼠标操作和表单输入 |
| `student.h` | 结构体定义、宏定义、函数声明 |
| `student.c` | 链表操作，学生记录添加、删除、查询、修改、插入、显示和输入检查 |
| `file.c` | 文件读取、保存、备份、导出和文件管理菜单 |
| `analysis.c` | 成绩排序、成绩统计、分数段统计和班级总览 |
| `data.txt` | 学生原始成绩数据文件 |
| `CMakeLists.txt` | CLion/CMake 构建配置文件 |
| `CLion运行说明.txt` | CLion 中文乱码处理说明 |

## 四、三人分工建议

本项目适合三人合作开发，源码注释中也标明了分工。

| 成员 | 负责文件 | 主要任务 |
|---|---|---|
| A | `main.cpp` | 图形界面、按钮事件、用户操作和整体流程控制 |
| B | `student.c`、`student.h` | 链表管理，学生信息增删改查，输入合法性检查 |
| C | `file.c`、`analysis.c` | 文件读写、备份导出、成绩排序、成绩统计 |

## 五、数据结构设计

学生信息结构体：

```c
typedef struct Student {
    char num[20];
    char name[20];
    float cScore;
    float mathScore;
    float englishScore;
    float total;
    float average;
} Student;
```

链表节点结构体：

```c
typedef struct Node {
    Student data;
    struct Node *next;
} Node;
```

程序使用带头结点的单向链表：

- 头结点不保存有效学生数据
- 第一条学生记录存放在 `head->next`
- 添加记录时插入链表尾部
- 删除记录时通过学号查找前驱节点
- 排序时交换节点中的 `Student` 数据

## 六、data.txt 文件格式

`data.txt` 每行保存一名学生的信息，字段之间用空格分隔。

格式：

```text
学号 姓名 C语言成绩 数学成绩 英语成绩
```

示例：

```text
2024001 ZhangSan 88 92 80
2024002 LiSi 76 85 90
2024003 WangWu 60 70 65
```

注意：

- 学号建议使用数字
- 姓名不要包含空格
- 各科成绩范围为 `0-100`
- 程序读取文件后会自动计算总分和平均分

## 七、在 CLion 中运行

1. 打开 CLion
2. 选择 `Open`
3. 打开 `GradeSystem` 项目文件夹
4. 等待 CLion 自动加载 `CMakeLists.txt`
5. 右上角选择运行目标 `GradeSystem`
6. 点击绿色运行按钮

`CMakeLists.txt` 中已经配置了：

- 自动编译 `main.cpp`、`student.c`、`file.c`、`analysis.c`
- 自动链接 EasyX 图形库
- 自动把 `data.txt` 复制到 CMake 构建目录
- Windows 下使用 UTF-8 显示中文

## 八、命令行编译运行

建议使用 CMake 编译。命令行环境需要先让 CLion 捆绑 MinGW 在 `PATH` 中：

```powershell
$env:Path = "D:\software\Toolbox\CLion\bin\mingw\bin;$env:Path"
cmake -S . -B cmake-build-debug -G Ninja `
  -DCMAKE_C_COMPILER="D:\software\Toolbox\CLion\bin\mingw\bin\gcc.exe" `
  -DCMAKE_CXX_COMPILER="D:\software\Toolbox\CLion\bin\mingw\bin\g++.exe" `
  -DCMAKE_MAKE_PROGRAM="D:\software\Toolbox\CLion\bin\ninja\win\x64\ninja.exe"
cmake --build cmake-build-debug
```

运行：

```powershell
.\cmake-build-debug\GradeSystem.exe
```

本版本使用 EasyX，仅支持 Windows + MinGW 环境。

## 九、中文乱码处理

本项目源码和程序字符串均使用 UTF-8。Windows 下程序会自动把控制台输入、输出代码页切换为 UTF-8，CMake 中也设置了对应编译选项。

如果在 CLion 中仍然出现中文乱码，请尝试：

1. 删除项目目录下的 `cmake-build-*` 文件夹
2. 在 CLion 中点击 `File -> Reload CMake Project`
3. 重新 Build
4. 重新 Run

也可以检查 CLion 编码设置：

```text
Settings -> Editor -> File Encodings
```

建议设置为：

```text
Global Encoding: UTF-8
Project Encoding: UTF-8
Default encoding for properties files: UTF-8
```

## 十、课程设计特点

本项目体现了 C 语言课程中的常见知识点：

- 结构体
- 单向链表
- 指针
- 动态内存分配
- 函数模块化设计
- 头文件声明
- 多文件编程
- 文件读写
- 输入合法性检查
- 菜单驱动程序设计
- 简单排序算法
- 数据统计分析

## 十一、注意事项

- 程序退出前会询问是否保存数据
- 添加、删除、修改、插入后会自动保存到 `data.txt`
- 保存数据前会自动备份旧文件到 `backup.txt`
- 如果 `data.txt` 不存在，程序会自动创建空文件
- 不建议手动修改 `data.txt` 为非法格式，否则程序读取时会跳过错误行

## 十二、项目总结

本系统以链表为核心数据结构，完成了学生成绩信息的录入、查询、修改、删除、排序、统计和文件保存功能。项目结构清晰，模块划分明确，功能完整，适合作为 C 语言课程设计项目提交和展示。
