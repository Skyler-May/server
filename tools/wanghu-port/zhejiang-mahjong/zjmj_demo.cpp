// 浙江麻将 GameLogic 在 Linux 上的冒烟 demo：牌索引构造 + 胡牌/听牌判定
#include "StdAfx.h"
#include "GameLogic.h"
#include <cstdio>
#include <cstring>

static void dump(const char* label, const BYTE idx[MAX_INDEX]) {
    int total = 0;
    printf("%s: ", label);
    for (int i = 0; i < MAX_INDEX; i++) {
        if (idx[i]) {
            printf("[%d]x%d ", i, idx[i]);
            total += idx[i];
        }
    }
    printf("(共 %d 张)\n", total);
}

int main() {
    CGameLogic logic;

    // 1) 标准胡牌：111 222 333 444 + 55（14 张）
    BYTE idx[MAX_INDEX];
    memset(idx, 0, sizeof(idx));
    idx[0] = 3; idx[1] = 3; idx[2] = 3; idx[3] = 3; idx[4] = 2;
    dump("标准胡 111222333444+55", idx);
    printf("  IsNeat3 = %s\n", logic.IsNeat3(idx) ? "true(可胡)" : "false");

    // 2) 未成牌：111 222 334 445 + 5（13 张听牌形）
    memset(idx, 0, sizeof(idx));
    idx[0] = 3; idx[1] = 3; idx[2] = 2; idx[3] = 2; idx[4] = 2;
    dump("听牌形 111222334455", idx);
    printf("  IsNeat3 = %s\n", logic.IsNeat3(idx) ? "true(可胡)" : "false");

    // 3) 七对形：11 22 33 44 55 66 77
    memset(idx, 0, sizeof(idx));
    for (int i = 0; i < 7; i++) idx[i] = 2;
    dump("七对 11223344556677", idx);
    printf("  IsNeat2 = %s\n", logic.IsNeat2(idx) ? "true(七对)" : "false");
    printf("  IsNeat3 = %s\n", logic.IsNeat3(idx) ? "true" : "false(非平胡)");

    // 4) 财神（King Card）机制
    logic.SetKingCardData(0x01);
    printf("\n财神牌 = 0x%02X, IsKingCardData(0x01) = %s, IsValidCommonCard(0x01) = %s\n",
        logic.GetKingCardData(),
        logic.IsKingCardData(0x01) ? "true" : "false",
        logic.IsValidCommonCard(0x01) ? "true" : "false");

    // 5) 洗牌：4 人各 13 张 + 剩余
    BYTE wall[136];
    memset(wall, 0, sizeof(wall));
    logic.RandCardData(wall, 136);
    int valid = 0;
    for (int i = 0; i < 136; i++)
        if (logic.IsValidCommonCard(wall[i]))
            valid++;
    printf("\n洗牌后有效牌数 = %d / 136\n", valid);

    printf("\n=== demo 结束 ===\n");
    return 0;
}
