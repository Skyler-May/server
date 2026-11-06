// 网狐斗地主逻辑 + NiuMa 风格房间状态机（最小 demo）
// 说明：这里用 standalone 的 LandlordRoom 复刻 NiuMa GameRoom 的状态流转
//       (Waiting -> Dealing -> Playing -> Settled)，验证"逻辑层搬运 + 房间层重写"这套模板可行。
#include "GameLogic.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>

static std::string cardName(BYTE c) {
    if (c == 0x4E) return "小王";
    if (c == 0x4F) return "大王";
    const char* suits[] = { "方块", "梅花", "红桃", "黑桃" };
    int suit = (c & 0xF0) >> 4;
    int val = c & 0x0F;
    if (suit < 0 || suit > 3 || val < 1 || val > 13) return "??";
    static const char* ranks[] = { "?", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    return std::string(suits[suit]) + ranks[val];
}

static std::string cardsText(const BYTE* c, int n) {
    std::string s;
    for (int i = 0; i < n; i++) { if (i) s += " "; s += cardName(c[i]); }
    return s;
}

class LandlordRoom {
public:
    enum class State { Waiting, Dealing, Playing, Settled };

    explicit LandlordRoom(int playerCount) : _playerCount(playerCount) {}

    void onTimer() {
        switch (_state) {
            case State::Waiting:
                beginDeal();
                break;
            case State::Dealing:
                _state = State::Playing;
                _current = _banker;
                _lastCount = 0;
                _lastPlayer = -1;
                printf("[房间] 发牌完成，地主(座位%d)先出\n", _banker);
                break;
            case State::Playing:
                doTurn();
                break;
            default:
                break;
        }
    }

    State state() const { return _state; }
    int winner() const { return _winner; }

private:
    void beginDeal() {
        printf("[房间] 进入发牌阶段\n");
        BYTE all[FULL_COUNT];
        for (int i = 0; i < FULL_COUNT; i++) all[i] = 0;
        _logic.RandCardList(all, FULL_COUNT);

        for (int p = 0; p < _playerCount; p++) {
            _count[p] = NORMAL_COUNT;
            for (int i = 0; i < NORMAL_COUNT; i++)
                _hands[p][i] = all[p * NORMAL_COUNT + i];
            _logic.SortCardList(_hands[p], _count[p], ST_ORDER);
            _logic.SetUserCard((WORD)p, _hands[p], _count[p]);
            printf("  玩家%d: %s\n", p, cardsText(_hands[p], _count[p]).c_str());
        }
        // 底牌给地主（简化：固定 0 号当地主）
        _banker = 0;
        for (int i = 0; i < BACK_COUNT; i++)
            _hands[_banker][_count[_banker]++] = all[_playerCount * NORMAL_COUNT + i];
        _logic.SortCardList(_hands[_banker], _count[_banker], ST_ORDER);
        _logic.SetUserCard((WORD)_banker, _hands[_banker], _count[_banker]);
        _logic.SetBanker((WORD)_banker);
        printf("  地主(座位%d) 拿底牌后共 %d 张\n", _banker, _count[_banker]);
        _state = State::Dealing;
    }

    // 最小 AI：能压就压（单牌/炸弹），否则过牌
    void doTurn() {
        if (_passCount >= _playerCount - 1) {
            // 其余全部过牌，本轮重新自由出牌
            _lastCount = 0;
            _lastPlayer = -1;
            _passCount = 0;
        }

        int seat = _current;
        BYTE pick[4];
        int pickCount = 0;
        bool played = false;

        if (_lastCount == 0) {
            // 自由出牌：出最小的一张
            pick[0] = _hands[seat][_count[seat] - 1];
            pickCount = 1;
            played = true;
        } else {
            // 找一张能压过上一手单牌的最小牌
            for (int i = _count[seat] - 1; i >= 0; i--) {
                BYTE cand[1] = { _hands[seat][i] };
                if (_logic.CompareCard(_lastCards, cand, _lastCount, 1)) {
                    pick[0] = _hands[seat][i];
                    pickCount = 1;
                    played = true;
                    break;
                }
            }
        }

        if (!played) {
            printf("  座位%d 过牌\n", seat);
            _passCount++;
        } else {
            removeCards(seat, pick, pickCount);
            memcpy(_lastCards, pick, pickCount);
            _lastCount = (BYTE)pickCount;
            _lastPlayer = seat;
            _passCount = 0;
            printf("  座位%d 出牌: %s（剩 %d 张）\n", seat, cardsText(pick, pickCount).c_str(), _count[seat]);
            if (_count[seat] == 0) {
                _winner = seat;
                _state = State::Settled;
                printf("[房间] 座位%d 出完牌，本局结束 -> 结算\n", seat);
                settle();
                return;
            }
        }
        _current = (_current + 1) % _playerCount;
    }

    void removeCards(int seat, const BYTE* cards, int n) {
        std::vector<BYTE> left;
        for (int i = 0; i < _count[seat]; i++) {
            bool remove = false;
            for (int j = 0; j < n; j++) {
                if (_hands[seat][i] == cards[j]) { remove = true; cards++; n--; break; }
            }
            if (!remove) left.push_back(_hands[seat][i]);
        }
        _count[seat] = (BYTE)left.size();
        for (size_t i = 0; i < left.size(); i++) _hands[seat][i] = left[i];
        _logic.SetUserCard((WORD)seat, _hands[seat], _count[seat]);
    }

    void settle() {
        printf("[房间] 结算：赢家=座位%d\n", _winner);
        _state = State::Settled;
    }

private:
    CGameLogic _logic;
    int _playerCount = 3;
    State _state = State::Waiting;
    BYTE _hands[GAME_PLAYER][MAX_COUNT];
    BYTE _count[GAME_PLAYER] = { 0, 0, 0 };
    BYTE _lastCards[MAX_COUNT];
    BYTE _lastCount = 0;
    int _lastPlayer = -1;
    int _current = 0;
    int _banker = 0;
    int _winner = -1;
    int _passCount = 0;
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    printf("=== 掼蛋/斗地主房间 demo（网狐 GameLogic + NiuMa 风格状态机）===\n");
    LandlordRoom room(3);
    int tick = 0;
    while (room.state() != LandlordRoom::State::Settled && tick < 200) {
        room.onTimer();
        tick++;
    }
    printf("=== demo 结束（%d 个 tick）===\n", tick);
    return 0;
}
