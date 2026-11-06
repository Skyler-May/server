# 网狐子游戏 → NiuMa 移植模板（斗地主为例）

## 结论

网狐子游戏的 **规则引擎 `GameLogic.cpp/h` 与网狐框架零耦合**（实测 `ITableFrame`/`IServerUserItem`/`CMD_` 依赖数 = 0），
可以直接搬到 Linux，只需要一个很小的垫片；真正要重写的是**牌桌/网络层**（`TableFrameSink.cpp`，网狐专属）。

## 编译验证

```bash
g++ -std=c++14 -o ddz_demo ddz_demo.cpp GameLogic.cpp
g++ -std=c++14 -o ddz_room_demo ddz_room_demo.cpp GameLogic.cpp
./ddz_demo        # 发牌 + 牌型识别 + 大小比较
./ddz_room_demo   # NiuMa 风格房间状态机跑一整局
```

结果：**0 error**，两个 demo 均可运行。

## 改动清单（每款游戏通用）

1. `StdAfx.h` 换成 3 行（`#include "port_shim.h"`）；
2. `port_shim.h` 提供：`BYTE/WORD/DWORD/INT_PTR/LONG`、`CString=std::string`、`ZeroMemory/CopyMemory`、
   `CountArray`、`GAME_PLAYER`、`ASSERT`，以及基于 `std::vector` 的 `CArrayTemplate`；
3. `GameLogic.cpp` 里 **4 处** MSVC 成员函数指针赋值需要显式 cast：
   ```cpp
   GetAllCardFunArray[0] = static_cast<pGetAllCardFun>(&CGameLogic::GetAllBomCard);
   ```

## 两个必须知道的 API 语义

- `GetCardType(cards, count)` **要求先把牌排序**（`SortCardList(cards,count,ST_ORDER)`），否则顺子/王炸会被判成非法；
- `CompareCard(first, next, firstCount, nextCount)` 返回 **"next 是否压过 first"**（true = 后一手更大）。

## 牌型识别实测

```
单牌 3     -> 单牌   (type=1)
对子 33    -> 对子   (type=2)
三条 333   -> 三条   (type=3)
顺子 34567 -> 顺子   (type=4)
炸弹 3333  -> 炸弹   (type=11)
火箭 王炸  -> 火箭   (type=12)
```

## 房间层怎么对应 NiuMa GameRoom

demo 里的 `LandlordRoom` 状态机（`Waiting → Dealing → Playing → Settled`）就是接入 NiuMa 时要实现的骨架：

| demo 方法 | 对应 NiuMa `GameRoom` |
|---|---|
| `onTimer()` | `onTimer()` / `update(nowTick)` |
| `beginDeal()` | 发牌逻辑（`beginDeal` + `notify*` 消息） |
| `doTurn()` | `onMessage(MsgPlayCard)` + 超时托管 `autoExecute` |
| `settle()` | 结算 + `MsgGameOver`/积分落库 |
| 机器人 | 网狐的 `AndroidUserItemSink.cpp`（可搬的 AI） |

## 每款游戏的预估工作量

| 环节 | 人天 |
|---|---|
| 逻辑层搬运 + 编译通过 | 0.5 |
| 房间状态机 / 消息 / 超时托管 | 2~3 |
| 结算与积分、战绩落库 | 0.5~1 |
| 客户端 UI（Unity，两条路径都一样） | 5~10 |
