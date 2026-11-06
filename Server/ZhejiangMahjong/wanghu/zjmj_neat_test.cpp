// 直接测试 IsNeat3 / IsNeat2 / GetHuInfo 的输入约定
#include "StdAfx.h"
#include "GameLogic.h"
#include <cstdio>

static void show(const char* label, BYTE idx[MAX_INDEX]) {
    int total = 0;
    printf("%-22s: ", label);
    for (int i = 0; i < MAX_INDEX; i++) { if (idx[i]) { printf("[%d]x%d ", i, idx[i]); total += idx[i]; } }
    printf("(共%d张)\n", total);
}

int main() {
    CGameLogic logic;
    BYTE idx[MAX_INDEX];

    // 12 张：111 222 333 444（4 个刻子，无将）
    ZeroMemory(idx, sizeof(idx));
    idx[0] = 3; idx[1] = 3; idx[2] = 3; idx[3] = 3;
    show("111222333444 (12张)", idx);
    printf("  IsNeat3 = %s\n", logic.IsNeat3(idx) ? "true" : "false");

    // 12 张：123 456 789 万 + 111 万（顺子+刻子）
    ZeroMemory(idx, sizeof(idx));
    for (int i = 0; i < 9; i++) idx[i] = 1;
    idx[0] += 2; // 1万 3 张
    show("123456789万 + 11万 (12张)", idx);
    printf("  IsNeat3 = %s\n", logic.IsNeat3(idx) ? "true" : "false");

    // 14 张：111 222 333 444 + 55
    ZeroMemory(idx, sizeof(idx));
    idx[0] = 3; idx[1] = 3; idx[2] = 3; idx[3] = 3; idx[4] = 2;
    show("111222333444+55 (14张)", idx);
    printf("  IsNeat3 = %s   IsNeat2 = %s\n", logic.IsNeat3(idx) ? "true" : "false", logic.IsNeat2(idx) ? "true" : "false");

    // 直接走 GetHuInfo（14 张）
    tagWeaveItem weave[4];
    ZeroMemory(weave, sizeof(weave));
    tagHuInput in;
    in.pCardIndexSeq = idx;
    in.pWeaveItemSeq = weave;
    in.cbWeaveItemCount = 0;
    in.cbCurrentCard = 0x05;
    in.bInsertCurrentCard = false;
    in.bJudgeHuOnly = true;
    tagHuCourt court;
    court.Reset();
    bool hu = logic.GetHuInfo(&in, court);
    printf("  GetHuInfo(14张) = %s  QueueType=%d\n", hu ? "true" : "false", (int)court.wHuCardQueueType);

    // 直接走 GetHuInfo（12 张，无将）
    ZeroMemory(idx, sizeof(idx));
    idx[0] = 3; idx[1] = 3; idx[2] = 3; idx[3] = 3;
    in.pCardIndexSeq = idx;
    court.Reset();
    hu = logic.GetHuInfo(&in, court);
    printf("  GetHuInfo(12张) = %s  QueueType=%d\n", hu ? "true" : "false", (int)court.wHuCardQueueType);

    return 0;
}
