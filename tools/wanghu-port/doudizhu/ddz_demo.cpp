// 网狐斗地主 GameLogic 在 Linux 上的冒烟 demo：发牌 + 牌型识别 + 大小比较
#include "GameLogic.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

static std::string cardName(BYTE c) {
    if (c == 0x4E) return "小王";
    if (c == 0x4F) return "大王";
    const char* suits[] = { "方块", "梅花", "红桃", "黑桃", "?", "?" };
    int suit = (c & 0xF0) >> 4;
    int val = c & 0x0F;
    if (suit < 0 || suit > 3) return "??";
    static const char* ranks[] = { "?", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    if (val < 1 || val > 13) return "??";
    return std::string(suits[suit]) + ranks[val];
}

static std::string cardsText(const BYTE* cards, int n) {
    std::string s;
    for (int i = 0; i < n; i++) {
        if (i) s += " ";
        s += cardName(cards[i]);
    }
    return s;
}

static const char* typeName(BYTE t) {
    switch (t) {
        case CT_SINGLE: return "单牌";
        case CT_DOUBLE: return "对子";
        case CT_THREE: return "三条";
        case CT_SINGLE_LINE: return "顺子";
        case CT_DOUBLE_LINE: return "连对";
        case CT_THREE_LINE: return "飞机";
        case CT_THREE_LINE_TAKE_ONE: return "三带一";
        case CT_THREE_LINE_TAKE_TWO: return "三带二";
        case CT_FOUR_LINE_TAKE_ONE: return "四带二";
        case CT_FOUR_LINE_TAKE_TWO: return "四带两对";
        case CT_BOMB_CARD: return "炸弹";
        case CT_MISSILE_CARD: return "火箭";
        default: return "非法";
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    CGameLogic logic;

    // 1) 洗牌
    BYTE all[FULL_COUNT];
    for (int i = 0; i < FULL_COUNT; i++) all[i] = 0;
    logic.RandCardList(all, FULL_COUNT);

    // 2) 发牌：3 家各 17 张 + 3 张底牌
    BYTE hands[GAME_PLAYER][NORMAL_COUNT];
    for (int p = 0; p < GAME_PLAYER; p++)
        for (int i = 0; i < NORMAL_COUNT; i++) hands[p][i] = all[p * NORMAL_COUNT + i];
    BYTE bottom[BACK_COUNT];
    for (int i = 0; i < BACK_COUNT; i++) bottom[i] = all[GAME_PLAYER * NORMAL_COUNT + i];

    printf("=== 发牌结果 ===\n");
    for (int p = 0; p < GAME_PLAYER; p++) {
        logic.SortCardList(hands[p], NORMAL_COUNT, ST_ORDER);
        logic.SetUserCard((WORD)p, hands[p], NORMAL_COUNT);
        printf("玩家%d(%2d张): %s\n", p, NORMAL_COUNT, cardsText(hands[p], NORMAL_COUNT).c_str());
    }
    logic.SetBackCard(3, bottom, BACK_COUNT);
    printf("底牌: %s\n\n", cardsText(bottom, BACK_COUNT).c_str());

    // 3) 牌型识别
    printf("=== 牌型识别 ===\n");
    BYTE single[1] = { 0x03 };
    BYTE pair[2] = { 0x03, 0x13 };
    BYTE three[3] = { 0x03, 0x13, 0x23 };
    BYTE straight[5] = { 0x03, 0x04, 0x05, 0x06, 0x07 };
    BYTE bomb[4] = { 0x03, 0x13, 0x23, 0x33 };
    BYTE rocket[2] = { 0x4E, 0x4F };
    struct { const char* label; BYTE* cards; BYTE n; } cases[] = {
        { "单牌 3", single, 1 },
        { "对子 33", pair, 2 },
        { "三条 333", three, 3 },
        { "顺子 34567", straight, 5 },
        { "炸弹 3333", bomb, 4 },
        { "火箭 王炸", rocket, 2 },
    };
    for (auto& c : cases) {
        logic.SortCardList(c.cards, c.n, ST_ORDER);
        BYTE t = logic.GetCardType(c.cards, c.n);
        printf("  %-12s -> %s (type=%d)\n", c.label, typeName(t), (int)t);
    }

    // 4) 大小比较
    printf("\n=== 大小比较 ===\n");
    BYTE s4[1] = { 0x04 };
    BYTE sA[1] = { 0x01 };
    printf("  CompareCard(4, 3) = %d\n", (int)logic.CompareCard(s4, single, 1, 1));
    printf("  CompareCard(3, 4) = %d\n", (int)logic.CompareCard(single, s4, 1, 1));
    printf("  CompareCard(A, 4) = %d\n", (int)logic.CompareCard(sA, s4, 1, 1));
    printf("  CompareCard(bomb, 单A) = %d\n", (int)logic.CompareCard(bomb, sA, 4, 1));
    printf("  CompareCard(rocket, bomb) = %d\n", (int)logic.CompareCard(rocket, bomb, 2, 4));

    printf("\n=== demo 结束 ===\n");
    return 0;
}
