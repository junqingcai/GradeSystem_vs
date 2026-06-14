#include <graphics.h>
#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <cwchar>
#include <string>

#include "app_logic.h"
#include "creator_info.h"
#include "student.h"

namespace {

/* 所有界面元素都按固定逻辑尺寸布局，再统一缩放到实际窗口。 */
constexpr int LOGICAL_WIDTH = 1280;
constexpr int LOGICAL_HEIGHT = 680;
constexpr int SIDEBAR_WIDTH = 210;
constexpr int PAGE_SIZE = 10;

int canvasWidth = 1600;
int canvasHeight = 850;
IMAGE logicalCanvas(LOGICAL_WIDTH, LOGICAL_HEIGHT);

/* 界面统一配色。 */
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

/* 左侧导航栏对应的三个页面。 */
enum class View {
    Students,
    Statistics,
    About
};

/* 程序运行期间需要在各页面之间共享的状态。 */
struct AppState {
    Node *head = nullptr;
    View view = View::Students;
    int page = 0;
    std::string filter;
    std::wstring status = L"就绪";
};

/* 数据文件使用 UTF-8，EasyX 文本接口使用宽字符，因此需要双向转换。 */
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
    constexpr BYTE CLEARTYPE_NATURAL = 6;
    LOGFONTW font{};
    font.lfHeight = height;
    font.lfWeight = weight;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = CLEARTYPE_NATURAL;
    font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(font.lfFaceName, L"Microsoft YaHei UI");
    settextstyle(&font);
}

/* 启用高 DPI 感知，避免 Windows 对窗口进行二次模糊缩放。 */
void enableHighDpiRendering() {
    using SetDpiAwarenessContext = BOOL(WINAPI *)(HANDLE);
    using SetDpiAware = BOOL(WINAPI *)();
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32 != nullptr) {
        auto setDpiContext = reinterpret_cast<SetDpiAwarenessContext>(
            GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        if (setDpiContext != nullptr) {
            constexpr intptr_t PER_MONITOR_AWARE_V2 = -4;
            if (setDpiContext(reinterpret_cast<HANDLE>(PER_MONITOR_AWARE_V2))) return;
        }
        auto setDpiAware = reinterpret_cast<SetDpiAware>(
            GetProcAddress(user32, "SetProcessDPIAware"));
        if (setDpiAware != nullptr) setDpiAware();
    }
}

/* 根据当前显示器工作区计算接近全屏且不变形的画布尺寸。 */
void calculateCanvasSize(const RECT &workArea) {
    constexpr int OUTER_HORIZONTAL_MARGIN = 32;
    constexpr int OUTER_VERTICAL_SPACE = 88;
    const int workWidth = static_cast<int>(workArea.right - workArea.left);
    const int workHeight = static_cast<int>(workArea.bottom - workArea.top);
    const int availableWidth =
        std::max(640, workWidth - OUTER_HORIZONTAL_MARGIN);
    const int availableHeight =
        std::max(360, workHeight - OUTER_VERTICAL_SPACE);
    /* 横纵方向使用同一缩放比例，保持原界面宽高比。 */
    const double scale = std::min(
        availableWidth / static_cast<double>(LOGICAL_WIDTH),
        availableHeight / static_cast<double>(LOGICAL_HEIGHT));
    canvasWidth = static_cast<int>(LOGICAL_WIDTH * scale);
    canvasHeight = static_cast<int>(LOGICAL_HEIGHT * scale);
}

int toLogicalX(int value) {
    /* 鼠标消息给出物理坐标，需要换算回固定逻辑坐标。 */
    return value * LOGICAL_WIDTH / canvasWidth;
}

int toLogicalY(int value) {
    return value * LOGICAL_HEIGHT / canvasHeight;
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
    const size_t defaultLength = std::min<size_t>(wcslen(defaultValue), 127);
    wmemcpy(buffer, defaultValue, defaultLength);
    buffer[defaultLength] = L'\0';
    if (!InputBox(buffer, 128, prompt, title, defaultValue, 360, 150, false)) return false;
    value = buffer;
    return !value.empty();
}

bool promptScore(const wchar_t *prompt, float &score, float defaultScore = 0.0f) {
    wchar_t defaultValue[32];
    swprintf(defaultValue, 32, L"%.2f", defaultScore);
    std::wstring input;
    if (!promptText(L"成绩输入", prompt, input, defaultValue)) return false;
    if (!parseScoreText(input.c_str(), &score)) {
        showMessage(L"成绩必须是 0 到 100 之间的数字。", MB_OK | MB_ICONWARNING);
        return false;
    }
    return true;
}

bool promptStudent(AppState &app, Student &student, bool editing) {
    /* 添加和修改共用一套输入流程；修改时保留原学号。 */
    std::wstring input;
    if (!editing) {
        if (!promptText(L"添加学生", L"请输入学号（4-19 位数字）：", input)) return false;
        std::string number = wideToUtf8(input.c_str());
        if (!setStudentNumber(app.head, &student, number.c_str())) {
            showMessage(L"学号格式不正确，或该学号已经存在。", MB_OK | MB_ICONWARNING);
            return false;
        }
    }

    std::wstring currentName = editing ? utf8ToWide(student.name) : L"";
    if (!promptText(editing ? L"修改学生" : L"添加学生", L"请输入姓名：",
                    input, currentName.c_str())) {
        return false;
    }
    std::string name = wideToUtf8(input.c_str());
    if (!setStudentName(&student, name.c_str())) {
        showMessage(L"姓名不能为空，且 UTF-8 长度不能超过 19 字节。", MB_OK | MB_ICONWARNING);
        return false;
    }

    if (!promptScore(L"请输入 C 语言成绩：", student.cScore,
                     editing ? student.cScore : 0.0f)) return false;
    if (!promptScore(L"请输入数学成绩：", student.mathScore,
                     editing ? student.mathScore : 0.0f)) return false;
    if (!promptScore(L"请输入英语成绩：", student.englishScore,
                     editing ? student.englishScore : 0.0f)) return false;
    calculateScore(&student);
    return true;
}

void drawSidebar(const AppState &app) {
    fillPanel({0, 0, SIDEBAR_WIDTH, LOGICAL_HEIGHT}, COLOR_SIDEBAR);
    drawText(L"GradeSystem", 26, 28, WHITE, 27, FW_BOLD);
    drawText(L"EasyX 图形版", 27, 64, RGB(170, 188, 204), 15);

    const Rect students{18, 125, 192, 171};
    const Rect statistics{18, 181, 192, 227};
    const Rect about{18, 237, 192, 283};
    drawButton(students, L"学生列表",
               app.view == View::Students ? COLOR_PRIMARY : RGB(48, 63, 78));
    drawButton(statistics, L"成绩统计",
               app.view == View::Statistics ? COLOR_PRIMARY : RGB(48, 63, 78));
    drawButton(about, L"关于",
               app.view == View::About ? COLOR_PRIMARY : RGB(48, 63, 78));

    drawText(L"数据文件", 26, 555, RGB(135, 155, 174), 14);
    drawText(L"data.txt", 26, 581, WHITE, 16, FW_SEMIBOLD);
    drawText(L"自动保存修改", 26, 610, RGB(135, 155, 174), 14);
}

void drawHeader(const AppState &app) {
    fillPanel({SIDEBAR_WIDTH, 0, LOGICAL_WIDTH, 80}, COLOR_PANEL);
    const wchar_t *title = L"关于本项目";
    if (app.view == View::Students) title = L"学生成绩管理";
    else if (app.view == View::Statistics) title = L"班级成绩统计";
    drawText(title, 242, 20, COLOR_TEXT, 28, FW_BOLD);
    drawText(app.status, 244, 54, COLOR_MUTED, 14);
}

void drawStudentsView(const AppState &app) {
    /* 当前页面只按需取得筛选后的记录，不复制整条链表。 */
    const int studentCount = countFilteredStudents(app.head, app.filter.c_str());
    const int pageCount = calculatePageCount(studentCount, PAGE_SIZE);
    int page = std::min(app.page, pageCount - 1);

    drawButton({242, 102, 340, 142}, L"添加学生", COLOR_SUCCESS);
    drawButton({352, 102, 450, 142}, L"搜索", COLOR_PRIMARY);
    drawButton({462, 102, 570, 142}, L"清除搜索", RGB(104, 117, 130));
    drawButton({582, 102, 680, 142}, L"排序", RGB(126, 87, 194));
    drawButton({692, 102, 790, 142}, L"保存", COLOR_PRIMARY_DARK);
    drawButton({802, 102, 900, 142}, L"重新加载", RGB(76, 112, 134));
    drawButton({912, 102, 1010, 142}, L"导出报表", RGB(0, 137, 123));
    drawButton({1128, 102, 1246, 142}, L"退出", COLOR_DANGER);

    fillPanel({242, 162, 1246, 590}, COLOR_PANEL);
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
    int end = std::min(start + PAGE_SIZE, studentCount);
    for (int index = start; index < end; ++index) {
        int row = index - start;
        int y = 215 + row * 38;
        Node *node = getFilteredStudentAt(app.head, app.filter.c_str(), index);
        if (node == nullptr) break;
        const Student &student = node->data;
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

    if (studentCount == 0) {
        drawText(app.filter.empty() ? L"暂无学生记录" : L"没有符合搜索条件的记录",
                 650, 380, COLOR_MUTED, 20);
    }

    wchar_t pageText[64];
    swprintf(pageText, 64, L"共 %d 条记录  第 %d / %d 页",
             studentCount, page + 1, pageCount);
    drawText(pageText, 260, 621, COLOR_MUTED, 15);
    drawButton({1035, 611, 1125, 651}, L"上一页", page > 0 ? COLOR_PRIMARY : COLOR_BORDER,
               page > 0 ? WHITE : COLOR_MUTED);
    drawButton({1135, 611, 1225, 651}, L"下一页",
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
    /* 统计计算由 C 模块完成，界面层只负责格式化和绘制。 */
    StudentStats stats{};
    calculateStudentStats(app.head, &stats);

    wchar_t text[64];
    swprintf(text, 64, L"%d", stats.count);
    drawStatCard(242, 112, 230, L"学生人数", text, COLOR_PRIMARY);
    swprintf(text, 64, L"%.2f", stats.classAverage);
    drawStatCard(492, 112, 230, L"班级平均分", text, COLOR_SUCCESS);
    swprintf(text, 64, L"%d", stats.allPass);
    drawStatCard(742, 112, 230, L"三科全部及格", text, RGB(255, 152, 0));
    swprintf(text, 64, L"%d", stats.excellent);
    drawStatCard(992, 112, 230, L"平均分优秀", text, RGB(126, 87, 194));

    fillPanel({242, 258, 1222, 570}, COLOR_PANEL);
    drawText(L"班级概览", 270, 282, COLOR_TEXT, 22, FW_BOLD);
    if (stats.hasBest) {
        std::wstring bestText =
            utf8ToWide(stats.best.num) + L"  " + utf8ToWide(stats.best.name);
        drawText(L"平均分最高学生", 280, 342, COLOR_MUTED, 16);
        drawText(bestText, 280, 378, COLOR_TEXT, 26, FW_BOLD);
        swprintf(text, 64, L"%.2f 分", stats.best.average);
        drawText(text, 280, 420, COLOR_PRIMARY, 22, FW_SEMIBOLD);
    }

    drawText(L"全部及格率", 650, 342, COLOR_MUTED, 16);
    swprintf(text, 64, L"%.1f%%", stats.passRate);
    drawText(text, 650, 378, COLOR_SUCCESS, 28, FW_BOLD);
    drawText(L"优秀率", 900, 342, COLOR_MUTED, 16);
    swprintf(text, 64, L"%.1f%%", stats.excellentRate);
    drawText(text, 900, 378, RGB(126, 87, 194), 28, FW_BOLD);

    drawButton({242, 605, 350, 649}, L"导出报表", RGB(0, 137, 123));
    drawButton({362, 605, 470, 649}, L"保存数据", COLOR_PRIMARY);
    drawButton({1120, 605, 1222, 649}, L"退出", COLOR_DANGER);
}

void drawCreatorCard(int x, int y, int width, int index) {
    const CreatorInfo *creator = getCreatorInfo(index);
    if (creator == nullptr) return;

    fillPanel({x, y, x + width, y + 250}, COLOR_PANEL);
    fillPanel({x, y, x + width, y + 7}, COLOR_PRIMARY);

    wchar_t memberTitle[32];
    swprintf(memberTitle, 32, L"制作者 %d", index + 1);
    drawText(memberTitle, x + 26, y + 28, COLOR_TEXT, 22, FW_BOLD);

    drawText(L"院系", x + 26, y + 82, COLOR_MUTED, 14);
    drawText(utf8ToWide(creator->department), x + 26, y + 106, COLOR_TEXT, 18);
    drawText(L"学号", x + 26, y + 145, COLOR_MUTED, 14);
    drawText(utf8ToWide(creator->studentId), x + 26, y + 169, COLOR_TEXT, 18);
    drawText(L"姓名", x + 26, y + 208, COLOR_MUTED, 14);
    drawText(utf8ToWide(creator->name), x + 86, y + 206, COLOR_TEXT, 18, FW_SEMIBOLD);
}

/* “关于”页面展示课程项目信息及三位制作者。 */
void drawAboutView() {
    drawText(L"学生成绩管理系统", 242, 112, COLOR_TEXT, 26, FW_BOLD);
    drawText(L"基于 C/C++、EasyX 和链表实现的课程设计项目",
             242, 151, COLOR_MUTED, 17);

    const int cardWidth = 300;
    drawCreatorCard(242, 210, cardWidth, 0);
    drawCreatorCard(562, 210, cardWidth, 1);
    drawCreatorCard(882, 210, cardWidth, 2);
}

void render(const AppState &app) {
    /*
     * 先在 1280×680 离屏画布上完成绘制，再高质量拉伸到实际窗口。
     * 这样可以同时支持不同分辨率，并保持控件比例和字体布局一致。
     */
    SetWorkingImage(&logicalCanvas);
    setbkmode(TRANSPARENT);
    setbkcolor(COLOR_BG);
    cleardevice();
    drawSidebar(app);
    drawHeader(app);
    if (app.view == View::Students) drawStudentsView(app);
    else if (app.view == View::Statistics) drawStatisticsView(app);
    else drawAboutView();

    SetWorkingImage();
    HDC target = GetImageHDC();
    HDC source = GetImageHDC(&logicalCanvas);
    SetStretchBltMode(target, HALFTONE);
    SetBrushOrgEx(target, 0, 0, nullptr);
    StretchBlt(target, 0, 0, canvasWidth, canvasHeight,
               source, 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT, SRCCOPY);
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
    /* 删除属于不可逆操作，执行前要求用户再次确认。 */
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
    if (!isSortTypeValid(value)) {
        showMessage(L"排序方式必须是 1 到 6。", MB_OK | MB_ICONWARNING);
        return;
    }
    sortStudents(app.head, static_cast<SortType>(value));
    app.page = 0;
    app.status = L"排序完成";
}

bool handleStudentsClick(AppState &app, int x, int y) {
    /* 按钮和表格行的点击区域均使用逻辑坐标判断。 */
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
    } else if (Rect{1035, 611, 1125, 651}.contains(x, y)) {
        if (app.page > 0) --app.page;
    } else if (Rect{1135, 611, 1225, 651}.contains(x, y)) {
        int studentCount = countFilteredStudents(app.head, app.filter.c_str());
        int pageCount = calculatePageCount(studentCount, PAGE_SIZE);
        if (app.page + 1 < pageCount) ++app.page;
    } else {
        int studentCount = countFilteredStudents(app.head, app.filter.c_str());
        int start = app.page * PAGE_SIZE;
        for (int row = 0; row < PAGE_SIZE; ++row) {
            int index = start + row;
            if (index >= studentCount) break;
            Node *node = getFilteredStudentAt(app.head, app.filter.c_str(), index);
            if (node == nullptr) break;
            int rowY = 215 + row * 38;
            if (Rect{1020, rowY - 3, 1092, rowY + 27}.contains(x, y)) {
                editStudentFromGui(app, node);
                break;
            }
            if (Rect{1102, rowY - 3, 1174, rowY + 27}.contains(x, y)) {
                deleteStudentFromGui(app, node);
                break;
            }
        }
    }
    return true;
}

bool handleClick(AppState &app, int x, int y) {
    /* 先处理全局导航，再将事件分派给当前页面。 */
    if (Rect{18, 125, 192, 171}.contains(x, y)) {
        app.view = View::Students;
        return true;
    }
    if (Rect{18, 181, 192, 227}.contains(x, y)) {
        app.view = View::Statistics;
        return true;
    }
    if (Rect{18, 237, 192, 283}.contains(x, y)) {
        app.view = View::About;
        app.status = L"制作者信息";
        return true;
    }
    if (app.view == View::Students) return handleStudentsClick(app, x, y);
    if (app.view == View::About) return true;
    if (Rect{242, 605, 350, 649}.contains(x, y)) {
        exportReport(app.head, EXPORT_FILE);
        app.status = L"报表已导出到 export.txt";
    } else if (Rect{362, 605, 470, 649}.contains(x, y)) {
        saveToFile(app.head, DATA_FILE);
        app.status = L"数据已保存";
    } else if (Rect{1120, 605, 1222, 649}.contains(x, y)) {
        return false;
    }
    return true;
}

} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    enableHighDpiRendering();

    /* 创建链表头结点并从运行目录加载初始数据。 */
    AppState app;
    app.head = createList();
    if (app.head == nullptr) {
        MessageBoxW(nullptr, L"内存分配失败，程序无法启动。", L"启动失败",
                    MB_OK | MB_ICONERROR);
        return 1;
    }

    loadFromFile(app.head, DATA_FILE);
    /* 计算接近全屏的窗口尺寸，但保留标题栏和任务栏。 */
    RECT workArea;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    calculateCanvasSize(workArea);
    initgraph(canvasWidth, canvasHeight);
    HWND window = GetHWnd();
    SetWindowTextW(window, L"学生成绩管理系统 - EasyX");

    RECT windowRect;
    GetWindowRect(window, &windowRect);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int windowX = workArea.left + (workArea.right - workArea.left - windowWidth) / 2;
    int windowY = workArea.top + (workArea.bottom - workArea.top - windowHeight) / 2;
    SetWindowPos(window, nullptr, windowX, windowY, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    setbkmode(TRANSPARENT);
    BeginBatchDraw();
    render(app);

    /* 使用非阻塞消息循环，保证窗口持续重绘且能及时响应鼠标和键盘。 */
    bool running = true;
    while (running && IsWindow(window)) {
        ExMessage message;
        while (peekmessage(&message, EX_MOUSE | EX_KEY)) {
            if (message.message == WM_LBUTTONDOWN) {
                running = handleClick(app,
                                      toLogicalX(message.x),
                                      toLogicalY(message.y));
            } else if (message.message == WM_KEYDOWN && message.vkcode == VK_ESCAPE) {
                running = false;
            }
        }
        if (running && IsWindow(window)) render(app);
        Sleep(30);
    }

    /* 退出前保存数据，并按创建顺序释放 EasyX 和链表资源。 */
    saveToFile(app.head, DATA_FILE);
    freeList(app.head);
    EndBatchDraw();
    closegraph();
    return 0;
}
