#include "creator_info.h"

/*
 * 在这里填写三位制作者的具体信息。
 * 字符串使用 UTF-8 编码保存。
 */
static const CreatorInfo CREATORS[] = {
    {"有训书院", "251840079", "薛宸"},
    {"有训书院", "251840138", "施静宇"},
    {"有训书院", "251840141", "蔡钧清"}
};

/* 由数组长度自动得到人数，新增成员时无需手工修改计数。 */
int getCreatorCount(void) {
    return (int)(sizeof(CREATORS) / sizeof(CREATORS[0]));
}

const CreatorInfo *getCreatorInfo(int index) {
    /* 越界时返回空指针，界面层会直接忽略该卡片。 */
    if (index < 0 || index >= getCreatorCount()) return 0;
    return &CREATORS[index];
}
