#include <graphics.h>
#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>

#include "student.h"

namespace {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 760;
constexpr int SIDEBAR_WIDTH = 210;
constexpr int PAGE_SIZE = 11;

const COLORREF COLOR_BG = RGB(244, 247, 250);
const COLORREF COLOR_PANEL = RGB(255, 255, 255);
const COLORREF COLOR_SIDEBAR = RGB(27, 38, 51);
const COLORREF COLOR_PRIMARY = RGB(33, 150, 243);
const COLORREF COLOR_PRIMARY_DARK = RGB(25, 118, 210);
const COLORREF COLOR_TEXT = RGB(35, 45, 55);
const COLORREF COLOR_MUTED = RGB(105, 117, 130);
const COLORREF COLOR_BORDER = RGB(220, 226, 232);
const COLORREF COLOR_DANGER = RGB(220, 68, 68);
const COLORREF COLOR_SUCCESS = RGB(46, 160, 100);

struct Rect {
    int left;
    int top;
    int right;
    int bottom;

    bool contains(int x, int y) const {
        return x >= left && x <= right && y >= top && y <= bottom;
    }
};

enum class View {
    Students,
    Statistics
};

struct AppState {
    Node *head = nullptr;
    View view = View::Students;
    int page = 0;
    std::string filter;
    std::wstring status = L"就绪";
};

std::wstring utf8ToWide(const char *text) {
    if (text == nullptr || text[0] == '\0') return L"";
    int length = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    if (length <= 0) return L"";
    std::wstring result(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text, -1, result.data(), length);
    result.pop_back();
    return result;
}

std::string wideToUtf8(const wchar_t *text) {
    if (text == nullptr || text[0] == L'\0') return "";
    int length = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
    if (length <= 0) return "";
    std::string result(static_cast<size_t>(length), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text, -1, result.data(), length, nullptr, nullptr);
    result.pop_back();
    return result;
}

void setFont(int height, int weight = FW_NORMAL) {
    settextstyle(height, 0, L"Microsoft YaHei UI", 0, 0, weight, false, false, false);
}

void drawText(const std::wstring &text, int x, int y, COLORREF color, int size = 18,
              int weight = FW_NORMAL) {
    settextcolor(color);
    setFont(size, weight);
    outtextxy(x, y, text.c_str());
}

void fillPanel(const Rect &rect, COLORREF color) {
    setfillcolor(color);
    solidrectangle(rect.left, rect.top, rect.right, rect.bottom);
}

void drawButton(const Rect &rect, const wchar_t *label, COLORREF color,
                COLORREF textColor = WHITE) {
    fillPanel(rect, color);
    setlinecolor(color);
    rectangle(rect.left, rect.top, rect.right, rect.bottom);
    settextcolor(textColor);
    setFont(17, FW_SEMIBOLD);
    int width = textwidth(label);
    int height = textheight(label);
    outtextxy(rect.left + (rect.right - rect.left - width) / 2,
              rect.top + (rect.bottom - rect.top - height) / 2, label);
}

void showMessage(const wchar_t *text, UINT type = MB_OK | MB_ICONINFORMATION) {
    MessageBoxW(GetHWnd(), text, L"学生成绩管理系统", type);
}

bool promptText(const wchar_t *title, const wchar_t *prompt, std::wstring &value,
                const wchar_t *defaultValue = L"") {
    wchar_t buffer[128] = {};
    wcsncpy(buffer, defaultValue, 127);
    if (!InputBox(buffer, 128, prompt, title, defaultValue, 360, 150, false)) return false;
    value = buffer;
    return !value.empty();
}

bool parseScore(const std::wstring &text, float &score) {
    wchar_t *end = nullptr;
    score = wcstof(text.c_str(), &end);
    return end != text.c_str() && *end == L'\0' && score >= 0.0f && score <= 100.0f;
}

bool promptScore(const wchar_t *prompt, float &score, float defaultScore = 0.0f) {
    wchar_t defaultValue[32];
    swprintf(defaultValue, 32, L"%.2f", defaultScore);
    std::wstring input;
    if (!promptText(L"成绩输入", prompt, input, defaultValue)) return false;
    if (!parseScore(input, score)) {
        showMessage(L"成绩必须是 0 到 100 之间的数字。", MB_OK | MB_ICONWARNING);
        return false;
    }
    return true;
}

bool promptStudent(AppState &app, Student &student, bool editing) {
    std::wstring input;
    if (!editing) {
        if (!promptText(L"添加学生", L"请输入学号（4-19 位数字）：", input)) return false;
        std::string number = wideToUtf8(input.c_str());
        if (!isNumFormatValid(number.c_str()) || isNumExist(app.head, number.c_str())) {
            showMessage(L"学号格式不正确，或该学号已经存在。", MB_OK | MB_ICONWARNING);
            return false;
        }
        strncpy(student.num, number.c_str(), NUM_LEN - 1);
        student.num[NUM_LEN - 1] = '\0';
    }

    std::wstring currentName = editing ? utf8ToWide(student.name) : L"";
    if (!promptText(editing ? L"修改学生" : L"添加学生", L"请输入姓名：",
                    input, currentName.c_str())) {
        return false;
    }
    std::string name = wideToUtf8(input.c_str());
    if (!isNameFormatValid(name.c_str()) || name.size() >= NAME_LEN) {
        showMessage(L"姓名不能为空，且 UTF-8 长度不能超过 19 字节。", MB_OK | MB_ICONWARNING);
        return false;
    }
    strncpy(student.name, name.c_str(), NAME_LEN - 1);
    student.name[NAME_LEN - 1] = '\0';

    if (!promptScore(L"请输入 C 语言成绩：", student.cScore,
                     editing ? student.cScore : 0.0f)) return false;
    if (!promptScore(L"请输入数学成绩：", student.mathScore,
                     editing ? student.mathScore : 0.0f)) return false;
    if (!promptScore(L"请输入英语成绩：", student.englishScore,
                     editing ? student.englishScore : 0.0f)) return false;
    calculateScore(&student);
    return true;
}

std::vector<Node *> filteredStudents(const AppState &app) {
    std::vector<Node *> result;
    for (Node *node = app.head->next; node != nullptr; node = node->next) {
        if (app.filter.empty() ||
            strstr(node->data.num, app.filter.c_str()) != nullptr ||
            strstr(node->data.name, app.filter.c_str()) != nullptr) {
            result.push_back(node);
        }
    }
    return result;
}

void drawSidebar(const AppState &app) {
    fillPanel({0, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT}, COLOR_SIDEBAR);
    drawText(L"GradeSystem", 26, 28, WHITE, 27, FW_BOLD);
    drawText(L"EasyX 图形版", 27, 64, RGB(170, 188, 204), 15);

    const Rect students{18, 125, 192, 171};
    const Rect statistics{18, 181, 192, 227};
    drawButton(students, L"学生列表",
               app.view == View::Students ? COLOR_PRIMARY : RGB(48, 63, 78));
    drawButton(statistics, L"成绩统计",
               app.view == View::Statistics ? COLOR_PRIMARY : RGB(48, 63, 78));

    drawText(L"数据文件", 26, 620, RGB(135, 155, 174), 14);
    drawText(L"data.txt", 26, 646, WHITE, 16, FW_SEMIBOLD);
    drawText(L"自动保存修改", 26, 675, RGB(135, 155, 174), 14);
}

void drawHeader(const AppState &app) {
    fillPanel({SIDEBAR_WIDTH, 0, WINDOW_WIDTH, 80}, COLOR_PANEL);
    drawText(app.view == View::Students ? L"学生成绩管理" : L"班级成绩统计",
             242, 20, COLOR_TEXT, 28, FW_BOLD);
    drawText(app.status, 244, 54, COLOR_MUTED, 14);
}

void drawStudentsView(const AppState &app) {
    const auto students = filteredStudents(app);
    const int pageCount = std::max(1, static_cast<int>(
        std::ceil(students.size() / static_cast<double>(PAGE_SIZE))));
    int page = std::min(app.page, pageCount - 1);

    drawButton({242, 102, 340, 142}, L"添加学生", COLOR_SUCCESS);
    drawButton({352, 102, 450, 142}, L"搜索", COLOR_PRIMARY);
    drawButton({462, 102, 570, 142}, L"清除搜索", RGB(104, 117, 130));
    drawButton({582, 102, 680, 142}, L"排序", RGB(126, 87, 194));
    drawButton({692, 102, 790, 142}, L"保存", COLOR_PRIMARY_DARK);
    drawButton({802, 102, 900, 142}, L"重新加载", RGB(76, 112, 134));
    drawButton({912, 102, 1010, 142}, L"导出报表", RGB(0, 137, 123));
    drawButton({1128, 102, 1246, 142}, L"退出", COLOR_DANGER);

    fillPanel({242, 162, 1246, 650}, COLOR_PANEL);
    drawText(L"学号", 264, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"姓名", 390, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"C 语言", 530, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"数学", 625, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"英语", 710, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"总分", 805, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"平均分", 895, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    drawText(L"操作", 1060, 178, COLOR_MUTED, 15, FW_SEMIBOLD);
    setlinecolor(COLOR_BORDER);
    line(258, 205, 1228, 205);

    int start = page * PAGE_SIZE;
    int end = std::min(start + PAGE_SIZE, static_cast<int>(students.size()));
    for (int index = start; index < end; ++index) {
        int row = index - start;
        int y = 215 + row * 38;
        const Student &student = students[index]->data;
        if (row % 2 == 1) fillPanel({252, y - 5, 1236, y + 30}, RGB(248, 250, 252));
        drawText(utf8ToWide(student.num), 264, y, COLOR_TEXT, 15);
        drawText(utf8ToWide(student.name), 390, y, COLOR_TEXT, 15);

        wchar_t value[32];
        swprintf(value, 32, L"%.1f", student.cScore);
        drawText(value, 535, y, COLOR_TEXT, 15);
        swprintf(value, 32, L"%.1f", student.mathScore);
        drawText(value, 625, y, COLOR_TEXT, 15);
        swprintf(value, 32, L"%.1f", student.englishScore);
        drawText(value, 710, y, COLOR_TEXT, 15);
        swprintf(value, 32, L"%.1f", student.total);
        drawText(value, 805, y, COLOR_TEXT, 15);
        swprintf(value, 32, L"%.2f", student.average);
        drawText(value, 895, y, COLOR_TEXT, 15);
        drawButton({1020, y - 3, 1092, y + 27}, L"修改", COLOR_PRIMARY);
        drawButton({1102, y - 3, 1174, y + 27}, L"删除", COLOR_DANGER);
    }

    if (students.empty()) {
        drawText(app.filter.empty() ? L"暂无学生记录" : L"没有符合搜索条件的记录",
                 650, 380, COLOR_MUTED, 20);
    }

    wchar_t pageText[64];
    swprintf(pageText, 64, L"共 %d 条记录  第 %d / %d 页",
             static_cast<int>(students.size()), page + 1, pageCount);
    drawText(pageText, 260, 676, COLOR_MUTED, 15);
    drawButton({1035, 666, 1125, 706}, L"上一页", page > 0 ? COLOR_PRIMARY : COLOR_BORDER,
               page > 0 ? WHITE : COLOR_MUTED);
    drawButton({1135, 666, 1225, 706}, L"下一页",
               page + 1 < pageCount ? COLOR_PRIMARY : COLOR_BORDER,
               page + 1 < pageCount ? WHITE : COLOR_MUTED);
}

void drawStatCard(int x, int y, int width, const wchar_t *title,
                  const std::wstring &value, COLORREF accent) {
    fillPanel({x, y, x + width, y + 118}, COLOR_PANEL);
    fillPanel({x, y, x + 6, y + 118}, accent);
    drawText(title, x + 24, y + 20, COLOR_MUTED, 15);
    drawText(value, x + 24, y + 55, COLOR_TEXT, 30, FW_BOLD);
}

void drawStatisticsView(const AppState &app) {
    int count = 0;
    int allPass = 0;
    int excellent = 0;
    float totalAverage = 0.0f;
    Student best{};
    bool hasBest = false;

    for (Node *node = app.head->next; node != nullptr; node = node->next) {
        const Student &student = node->data;
        ++count;
        totalAverage += student.average;
        if (student.cScore >= PASS_SCORE && student.mathScore >= PASS_SCORE &&
            student.englishScore >= PASS_SCORE) {
            ++allPass;
        }
        if (student.average >= EXCELLENT_SCORE) ++excellent;
        if (!hasBest || student.average > best.average) {
            best = student;
            hasBest = true;
        }
    }

    wchar_t text[64];
    swprintf(text, 64, L"%d", count);
    drawStatCard(242, 112, 230, L"学生人数", text, COLOR_PRIMARY);
    swprintf(text, 64, L"%.2f", count > 0 ? totalAverage / count : 0.0f);
    drawStatCard(492, 112, 230, L"班级平均分", text, COLOR_SUCCESS);
    swprintf(text, 64, L"%d", allPass);
    drawStatCard(742, 112, 230, L"三科全部及格", text, RGB(255, 152, 0));
    swprintf(text, 64, L"%d", excellent);
    drawStatCard(992, 112, 230, L"平均分优秀", text, RGB(126, 87, 194));

    fillPanel({242, 258, 1222, 620}, COLOR_PANEL);
    drawText(L"班级概览", 270, 282, COLOR_TEXT, 22, FW_BOLD);
    if (hasBest) {
        std::wstring bestText = utf8ToWide(best.num) + L"  " + utf8ToWide(best.name);
        drawText(L"平均分最高学生", 280, 342, COLOR_MUTED, 16);
        drawText(bestText, 280, 378, COLOR_TEXT, 26, FW_BOLD);
        swprintf(text, 64, L"%.2f 分", best.average);
        drawText(text, 280, 420, COLOR_PRIMARY, 22, FW_SEMIBOLD);
    }

    float passRate = count > 0 ? allPass * 100.0f / count : 0.0f;
    float excellentRate = count > 0 ? excellent * 100.0f / count : 0.0f;
    drawText(L"全部及格率", 650, 342, COLOR_MUTED, 16);
    swprintf(text, 64, L"%.1f%%", passRate);
    drawText(text, 650, 378, COLOR_SUCCESS, 28, FW_BOLD);
    drawText(L"优秀率", 900, 342, COLOR_MUTED, 16);
    swprintf(text, 64, L"%.1f%%", excellentRate);
    drawText(text, 900, 378, RGB(126, 87, 194), 28, FW_BOLD);

    drawButton({242, 656, 350, 700}, L"导出报表", RGB(0, 137, 123));
    drawButton({362, 656, 470, 700}, L"保存数据", COLOR_PRIMARY);
    drawButton({1120, 656, 1222, 700}, L"退出", COLOR_DANGER);
}

void render(const AppState &app) {
    setbkcolor(COLOR_BG);
    cleardevice();
    drawSidebar(app);
    drawHeader(app);
    if (app.view == View::Students) drawStudentsView(app);
    else drawStatisticsView(app);
    FlushBatchDraw();
}

void addStudentFromGui(AppState &app) {
    Student student{};
    if (!promptStudent(app, student, false)) return;
    if (appendStudent(app.head, student)) {
        saveToFile(app.head, DATA_FILE);
        app.status = L"学生已添加并保存";
    } else {
        showMessage(L"添加失败，请检查学号是否重复。", MB_OK | MB_ICONERROR);
    }
}

void editStudentFromGui(AppState &app, Node *node) {
    if (node == nullptr) return;
    Student edited = node->data;
    if (!promptStudent(app, edited, true)) return;
    node->data = edited;
    saveToFile(app.head, DATA_FILE);
    app.status = L"学生信息已修改并保存";
}

void deleteStudentFromGui(AppState &app, Node *node) {
    if (node == nullptr) return;
    std::wstring message = L"确定删除学生 " + utf8ToWide(node->data.name) + L" 吗？";
    if (MessageBoxW(GetHWnd(), message.c_str(), L"确认删除",
                    MB_YESNO | MB_ICONWARNING) != IDYES) return;
    std::string number = node->data.num;
    removeByNum(app.head, number.c_str(), nullptr);
    saveToFile(app.head, DATA_FILE);
    app.status = L"学生记录已删除";
}

void searchFromGui(AppState &app) {
    std::wstring keyword;
    if (!promptText(L"搜索学生", L"输入学号或姓名关键字：", keyword)) return;
    app.filter = wideToUtf8(keyword.c_str());
    app.page = 0;
    app.status = L"正在显示搜索结果";
}

void sortFromGui(AppState &app) {
    std::wstring choice;
    if (!promptText(L"成绩排序",
                    L"输入排序方式：1 总分降序，2 平均分降序，3 C 降序，"
                    L"4 数学降序，5 英语降序，6 学号升序",
                    choice, L"1")) {
        return;
    }
    int value = _wtoi(choice.c_str());
    if (value < 1 || value > 6) {
        showMessage(L"排序方式必须是 1 到 6。", MB_OK | MB_ICONWARNING);
        return;
    }
    sortStudents(app.head, static_cast<SortType>(value));
    app.page = 0;
    app.status = L"排序完成";
}

bool handleStudentsClick(AppState &app, int x, int y) {
    if (Rect{242, 102, 340, 142}.contains(x, y)) addStudentFromGui(app);
    else if (Rect{352, 102, 450, 142}.contains(x, y)) searchFromGui(app);
    else if (Rect{462, 102, 570, 142}.contains(x, y)) {
        app.filter.clear();
        app.page = 0;
        app.status = L"已清除搜索条件";
    } else if (Rect{582, 102, 680, 142}.contains(x, y)) sortFromGui(app);
    else if (Rect{692, 102, 790, 142}.contains(x, y)) {
        saveToFile(app.head, DATA_FILE);
        app.status = L"数据已保存";
    } else if (Rect{802, 102, 900, 142}.contains(x, y)) {
        loadFromFile(app.head, DATA_FILE);
        app.page = 0;
        app.status = L"已重新加载 data.txt";
    } else if (Rect{912, 102, 1010, 142}.contains(x, y)) {
        exportReport(app.head, EXPORT_FILE);
        app.status = L"报表已导出到 export.txt";
    } else if (Rect{1128, 102, 1246, 142}.contains(x, y)) {
        return false;
    } else if (Rect{1035, 666, 1125, 706}.contains(x, y)) {
        if (app.page > 0) --app.page;
    } else if (Rect{1135, 666, 1225, 706}.contains(x, y)) {
        auto students = filteredStudents(app);
        int pageCount = std::max(1, static_cast<int>(
            std::ceil(students.size() / static_cast<double>(PAGE_SIZE))));
        if (app.page + 1 < pageCount) ++app.page;
    } else {
        auto students = filteredStudents(app);
        int start = app.page * PAGE_SIZE;
        for (int row = 0; row < PAGE_SIZE; ++row) {
            int index = start + row;
            if (index >= static_cast<int>(students.size())) break;
            int rowY = 215 + row * 38;
            if (Rect{1020, rowY - 3, 1092, rowY + 27}.contains(x, y)) {
                editStudentFromGui(app, students[index]);
                break;
            }
            if (Rect{1102, rowY - 3, 1174, rowY + 27}.contains(x, y)) {
                deleteStudentFromGui(app, students[index]);
                break;
            }
        }
    }
    return true;
}

bool handleClick(AppState &app, int x, int y) {
    if (Rect{18, 125, 192, 171}.contains(x, y)) {
        app.view = View::Students;
        return true;
    }
    if (Rect{18, 181, 192, 227}.contains(x, y)) {
        app.view = View::Statistics;
        return true;
    }
    if (app.view == View::Students) return handleStudentsClick(app, x, y);
    if (Rect{242, 656, 350, 700}.contains(x, y)) {
        exportReport(app.head, EXPORT_FILE);
        app.status = L"报表已导出到 export.txt";
    } else if (Rect{362, 656, 470, 700}.contains(x, y)) {
        saveToFile(app.head, DATA_FILE);
        app.status = L"数据已保存";
    } else if (Rect{1120, 656, 1222, 700}.contains(x, y)) {
        return false;
    }
    return true;
}

} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    AppState app;
    app.head = createList();
    if (app.head == nullptr) {
        MessageBoxW(nullptr, L"内存分配失败，程序无法启动。", L"启动失败",
                    MB_OK | MB_ICONERROR);
        return 1;
    }

    loadFromFile(app.head, DATA_FILE);
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    SetWindowTextW(GetHWnd(), L"学生成绩管理系统 - EasyX");
    setbkmode(TRANSPARENT);
    BeginBatchDraw();
    render(app);

    bool running = true;
    while (running) {
        ExMessage message = getmessage(EX_MOUSE | EX_KEY);
        if (message.message == WM_LBUTTONDOWN) {
            running = handleClick(app, message.x, message.y);
            if (running) render(app);
        } else if (message.message == WM_KEYDOWN && message.vkcode == VK_ESCAPE) {
            running = false;
        }
    }

    saveToFile(app.head, DATA_FILE);
    freeList(app.head);
    EndBatchDraw();
    closegraph();
    return 0;
}
