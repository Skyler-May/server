// 浙江麻将胡牌判定冒烟测试（独立于 NiuMa 框架）
#include "StdAfx.h"
#include "ZhejiangHuChecker.h"

#include <cstdio>
#include <vector>

using namespace NiuMa;

static void testCase(const char* name, const std::vector<BYTE>& cards, BYTE current, bool zimo, BYTE king) {
    ZhejiangHuChecker checker;
    checker.setKingCard(king);
    ZhejiangHuResult r;
    bool hu = checker.checkHu(cards, current, zimo, r);
    std::string text;
    for (size_t i = 0; i < cards.size(); i++) {
        if (i) text += " ";
        text += ZhejiangHuChecker::cardToText(cards[i]);
    }
    printf("%-24s 牌: %s\n", name, text.c_str());
    printf("%-24s 结果: %s  (牌型:%s 财神:%d)\n\n", "",
        hu ? "可胡" : "不可胡", r.desc.empty() ? "-" : r.desc.c_str(), (int)king);
}

int main() {
    // 1) 平胡：123万 456万 789万 123条 99筒（14张，单钓）
    std::vector<BYTE> t1 = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x11, 0x12, 0x13, 0x29, 0x29
    };
    testCase("平胡(14张)", t1, 0x29, false, 0);
    std::vector<BYTE> t1b(t1.begin(), t1.end() - 1);
    testCase("平胡(13张+摸牌)", t1b, 0x29, true, 0);

    // 2) 七对：1万 1万 2万 2万 3条 3条 4条 4条 5筒 5筒 6筒 6筒 东 东
    std::vector<BYTE> t2 = {
        0x01, 0x01, 0x02, 0x02, 0x11, 0x11, 0x12, 0x12,
        0x25, 0x25, 0x26, 0x26, 0x31, 0x31
    };
    testCase("七对", t2, 0x31, false, 0);

    // 3) 清一色：全是万 11 234 567 789 99
    std::vector<BYTE> t3 = {
        0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x01, 0x09, 0x01, 0x09
    };
    testCase("清一色(乱序)", t3, 0x09, false, 0);

    // 4) 带财神：把一张牌当作财神（0x05），看是否能胡
    std::vector<BYTE> t4 = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x11, 0x12, 0x13, 0x29, 0x05
    };
    testCase("带财神", t4, 0x05, true, 0x05);

    // 5) 不能胡：全部散牌
    std::vector<BYTE> t5 = {
        0x01, 0x04, 0x07, 0x11, 0x14, 0x17, 0x21, 0x24, 0x27,
        0x31, 0x33, 0x35, 0x37, 0x02
    };
    testCase("散牌", t5, 0x02, false, 0);

    // 6) 碰碰胡：1万*3 2万*3 3万*3 4万*3 5万*2
    std::vector<BYTE> t6 = {
        0x01,0x01,0x01, 0x02,0x02,0x02, 0x03,0x03,0x03, 0x04,0x04,0x04, 0x05,0x05
    };
    testCase("碰碰胡", t6, 0x05, false, 0);

    // 7) 平胡(条)：123条 456条 789条 111万 99筒
    std::vector<BYTE> t7 = {
        0x11,0x12,0x13, 0x14,0x15,0x16, 0x17,0x18,0x19, 0x01,0x01,0x01, 0x29,0x29
    };
    testCase("平胡(条)", t7, 0x29, false, 0);

    // 8) 碰碰胡 13 张 + 摸 5万（bInsertCurrentCard 自适应为 true）
    std::vector<BYTE> t8 = {
        0x01,0x01,0x01, 0x02,0x02,0x02, 0x03,0x03,0x03, 0x04,0x04,0x04, 0x05
    };
    testCase("碰碰胡(13+摸)", t8, 0x05, true, 0);

    // 9) 最简单：111万 222万 333万 444万 55万，但财神设为白板(0x37)避免误判
    testCase("碰碰胡(财神=白板)", t6, 0x05, false, 0x37);

    return 0;
}
