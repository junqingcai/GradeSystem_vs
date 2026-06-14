#ifndef CREATOR_INFO_H
#define CREATOR_INFO_H

/* “关于”页面使用的制作者信息接口。 */

#ifdef __cplusplus
extern "C" {
#endif

/* 每名制作者需要在界面中展示的三项信息。 */
typedef struct CreatorInfo {
    const char *department;
    const char *studentId;
    const char *name;
} CreatorInfo;

int getCreatorCount(void);
const CreatorInfo *getCreatorInfo(int index);

#ifdef __cplusplus
}
#endif

#endif
