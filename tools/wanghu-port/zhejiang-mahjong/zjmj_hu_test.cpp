// 浙江麻将 胡牌判定 + 番型计分 冒烟测试（独立于 NiuMa 框架）
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
    printf("%-16s 牌: %s\n", name, text.c_str());
    if (hu)
        printf("%-16s => 可胡 | 番:%d 得分:%d | %s\n\n", "", r.fan, r.score, r.desc.c_str());
    else
        printf("%-16s => 不可胡\n\n", "");
}

int main() {
    // 平胡：123万 456万 789万 123条 99筒
    std::vector<BYTE> pingHu = {
        0x01,0x02,0x03, 0x04,0x05,0x06, 0x07,0x08,0x09, 0x11,0x12,0x13, 0x29,0x29
    };
    testCase("平胡-点炮", pingHu, 0x29, false, 0);
    testCase("平胡-自摸", pingHu, 0x29, true, 0);

    // 七对
    std::vector<BYTE> qiDui = {
        0x01,0x01, 0x02,0x02, 0x11,0x11, 0x12,0x12, 0x25,0x25, 0x26,0x26, 0x31,0x31
    };
    testCase("七小对", qiDui, 0x31, false, 0);

    // 清七对：全部万子七对
    std::vector<BYTE> qingQiDui = {
        0x01,0x01, 0x02,0x02, 0x03,0x03, 0x04,0x04, 0x05,0x05, 0x06,0x06, 0x07,0x07
    };
    testCase("清七对", qingQiDui, 0x07, false, 0);

    // 碰碰胡
    std::vector<BYTE> pengPeng = {
        0x01,0x01,0x01, 0x02,0x02,0x02, 0x03,0x03,0x03, 0x04,0x04,0x04, 0x05,0x05
    };
    testCase("碰碰胡-自摸", pengPeng, 0x05, true, 0);

    // 清一色平胡
    std::vector<BYTE> qingYiSe = {
        0x01,0x02,0x03, 0x04,0x05,0x06, 0x07,0x08,0x09, 0x01,0x01,0x01, 0x09,0x09
    };
    testCase("清一色", qingYiSe, 0x09, false, 0);

    // 带 1 张财神（5万）的平胡自摸
    std::vector<BYTE> withKing = {
        0x01,0x02,0x03, 0x04,0x05,0x06, 0x07,0x08,0x09, 0x11,0x12,0x13, 0x29,0x05
    };
    testCase("带财神-自摸", withKing, 0x05, true, 0x05);

    // 不可胡
    std::vector<BYTE> garbage = {
        0x01,0x04,0x07, 0x11,0x14,0x17, 0x21,0x24,0x27, 0x31,0x33,0x35, 0x37,0x02
    };
    testCase("散牌", garbage, 0x02, false, 0);

    return 0;
}
