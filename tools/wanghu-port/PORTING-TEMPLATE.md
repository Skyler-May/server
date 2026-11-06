# 网狐子游戏 → NiuMa 移植模板（v1，斗地主已验证）

> 适用范围：把网狐（WangHu）C++ 子游戏的**规则引擎**搬到 NiuMa 服务端，其余按 NiuMa 架构重写。
> 已用「斗地主」完整验证：进房 → 补机器人 → 发牌 → 出牌 → 结算，端到端跑通。

---

## 一、先判断能不能搬

| 网狐文件 | 能否搬 | 说明 |
|---|---|---|
| `GameLogic.cpp/h` | ✅ **直接搬** | 实测 `ITableFrame`/`IServerUserItem`/`CMD_` 依赖为 **0** |
| `TableFrameSink.cpp` | ❌ 重写 | 网狐牌桌胶水层（43 处 ITableFrame） |
| `AndroidUserItemSink.cpp` | 🔶 借鉴 | 机器人 AI 算法，可移植到 `<X>Avatar::candidateCombinationsImpl` |
| 客户端工程 | ❌ 不用 | 网狐客户端是 Win32/MFC，与 Unity 无关 |

判断命令（在网狐游戏服务器目录执行）：

```bash
grep -c "ITableFrame\|IServerUserItem\|CMD_" GameLogic.cpp
# 0 表示逻辑层可整体搬走
```

---

## 二、移植步骤（以斗地主为例，每款约 0.5~1 人天）

### 步骤 1：抽逻辑层

```bash
mkdir -p vendor/server/Server/<Game>
cp 网狐/<游戏>/游戏服务器/GameLogic.{h,cpp} vendor/server/Server/<Game>/
```

### 步骤 2：写垫片 `port_shim.h`

必需内容（缺一个就编译不过）：

```cpp
typedef unsigned char BYTE;  typedef unsigned short WORD;
typedef int INT_PTR;         typedef long LONG;
typedef std::string CString;
#define ZeroMemory(p,n) memset((p),0,(n))
#define CopyMemory(d,s,n) memcpy((d),(s),(n))
#define CountArray(x) (sizeof(x)/sizeof((x)[0]))
#define ASSERT(x) do { (void)sizeof(x); } while (0)
#ifndef GAME_PLAYER
#define GAME_PLAYER 3      // 按游戏人数改
#endif
// 网狐的 CArrayTemplate 用 std::vector 实现（GetCount/GetAt/Add/RemoveAt/operator[]）
template <class TYPE, class ARG_TYPE = const TYPE&> class CArrayTemplate { ... };
```

并把 `StdAfx.h` 替换为：

```cpp
#pragma once
#include "port_shim.h"
```

编译验证：

```bash
g++ -std=c++14 -c GameLogic.cpp -o /tmp/gl.o
```

### 步骤 3：修 MSVC 特有写法

已知必改点（斗地主 4 处）：成员函数指针赋值需要显式 cast

```cpp
GetAllCardFunArray[0] = static_cast<pGetAllCardFun>(&CGameLogic::GetAllBomCard);
```

### 步骤 4：NiuMa 规则层 `<Game>Rule`（继承 `DouDiZhuRule`）

```cpp
class LandlordRule : public DouDiZhuRule {
    LandlordRule() {
        if (_orderTable == nullptr) _orderTable = new CardOrderTable();
        _orderTable->setPointOrder((int)PokerPoint::Three, 0);   // 3最小
        ...
        _orderTable->setPointOrder((int)PokerPoint::Joker, 13);  // 王最大
    }
    int getBombOrder(int genre) const override;   // Bomb=1, Rocket=2
    int getBombByOrder(int order) const override;
    int predicateCardGenre(PokerGenre& pcg) const override;  // 牌型判定
};
```

**两个 API 语义坑（务必记住）**：
- `predicateCardGenre` 内部做判定时，牌**先排序**再判顺子/王炸，否则误判；
- `DouDiZhuRule::compareGenre(a,b)` 返回 **1 表示 a 大、2 表示 b 大、0 不可比**。

### 步骤 5：消息层 `<Game>Messages`

每个 `MsgBase` 派生消息必须同时有：

```cpp
MSG_PACK_IMPL
MSGPACK_DEFINE_MAP(字段1, 字段2, ...);   // 少了它，编译期报 no member named msgpack_pack
```

然后写 `registMessages()`，并在 `main.cpp` 注册。

### 步骤 6：房间层 `<Game>Room`（继承 `GameRoom`）

必须实现的纯虚函数只有 4 个：

```cpp
GameAvatar::Ptr createAvatar(playerId, seat, robot) const override;
bool checkEnter(playerId, errMsg, robot) const override;
int  checkLeave(playerId, errMsg) const override;
bool beforeDestroy(bool silence) override;
```

推荐再实现：`initialize/onTimer/onMessage/clean/canReplaceRobot`。

状态机（斗地主为例）：`Waiting → Dealing → Playing → Settled`。

关键点：
- 发牌用 NiuMa 的 `PokerDealer`（`shuffle()` + `handOutCards(cards, n)`）；
- 机器人补位复用 `GameRoom::joinRobot`（配合 `PlayerManager::loadRobot`）；
- 出牌校验：`predicateCardGenre` + `compareGenre`；
- 超时托管：`onTimer` 里 `deltaTicks >= PLAY_TIMEOUT` 自动出牌/过牌。

### 步骤 7：Handler + Loader

与 `LackeyRoomHandler`/`LackeyLoader` 完全同构：

```cpp
LandlordRoomHandler::LandlordRoomHandler(queue) : GameRoomHandler(queue) {
    addMessage(MsgLandlordSync::TYPE);
    addMessage(MsgLandlordPlayCard::TYPE);
    addGameType((int)GameType::Landlord);
}
```

Loader 读自己的表 `game_<game>(venue_id, number, ...)`。

### 步骤 8：接线 + 建表 + 后台

1. `Server/GameDefines.h`：`Landlord = 1031`；
2. `main.cpp`：`#include` 三个头文件 + `registLoader` + `registHandler` + `registMessages`；
3. **重跑 cmake**（`Server/CMakeLists.txt` 是 `GLOB Server/*/*.cpp`，新目录不重跑 configure 会链接不到）；
4. 建表：

```sql
CREATE TABLE `game_landlord` (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `venue_id` varchar(16) NOT NULL,
  `number` varchar(16) NOT NULL,
  `di_zhu` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`), UNIQUE KEY `uk_venue` (`venue_id`), KEY `idx_number` (`number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

5. Java 侧：`NiuMaConstants.GAME_TYPE_X`、`Game<X>` 实体、`Game<X>Mapper` + XML、`GameServiceImpl.enterNumber` 分支；
6. 建场地：`venue(id, owner_id, game_type=1031, status=0)` + `game_landlord(venue_id, number, di_zhu)`。

---

## 三、验收清单（用 `tools/niuma-bot`）

```bash
node tools/niuma-bot/bot.mjs p1001 0 1031 <房号>
```

期望服务端日志：

```
<X>补位：机器人(Id:xxx)进入房间，当前人数:2
<X>补位：机器人(Id:yyy)进入房间，当前人数:3
<X>房间(Id:...)人满且全部准备，开始发牌
<X>牌桌(ID:...)发牌，玩家:...
<X>房间(Id:...)开始出牌，地主座位:1
<X>发手牌：座位0 玩家xxx 张数17 session=ok
...
<X>房间(Id:...)结算，赢家座位:2，地主负
```

客户端应能收到：`DealCard / HandCard / Turn / PlayCardResp / GameOver`。

---

## 四、踩坑清单（都踩过，直接照做）

| 坑 | 现象 | 解决 |
|---|---|---|
| CMake GLOB | 链接报 `undefined reference to <X>Loader::<X>Loader()` | 重跑 `cmake .` |
| msgpack 宏顺序 | 编译报 `has no member named msgpack_pack` | 每个消息补 `MSGPACK_DEFINE_MAP(...)` |
| 逻辑层 Windows 类型 | `ZeroMemory/CopyMemory/CountArray/CArrayTemplate` 未定义 | 用垫片 |
| 成员函数指针 | `cannot convert ... to type pGetAllCardFun` | 显式 `static_cast` |
| 牌型判定 | 顺子/王炸被判非法 | 判定前先 `sort` |
| **session 时序** | 发牌/轮转消息客户端收不到 | 发送前用 `PlayerManager` 里的**当前 session** 刷新替身 |
| **同账号抢占** | 客户端反复 reconnect/disconnect，牌局卡住 | 测试时只保留一个客户端（关掉残留的 H5 页面/浏览器） |
| 进房授权 | `MsgEnterVenue` 直接进 venueId 报 `not authorized` | 必须走 `enter/district` 或 `enter/number` |
| **复制模块后 include guard 重名** | 编译报 `PaoDeKuaiLoader is not a member of NiuMa` | 复制模块后先批量替换 `_NIU_MA_<OLD>_` → `_NIU_MA_<NEW>_` |
| **MSVC for 作用域泄漏** | `i was not declared in this scope`（循环内 `for(i=0;...)`，函数后面继续用 `i`） | 在函数体开头补 `int i = 0;`（脚本见 `tools/wanghu-port/zhejiang-mahjong/fix-zjmj-scope.mjs`） |
| **厂商源码拼写错误** | `const LONG_SCORE_XXX`（LONG 与变量名黏在一起，MSVC 也不该过） | 改为 `const LONG SCORE_XXX` |
| 场地未激活 | `enter/number` 报 `Venue not exist` | `venue.status = 0` |

---

## 五、工作量参考（每款）

| 环节 | 人天 |
|---|---|
| 抽逻辑 + 垫片 + 编译通过 | 0.5 |
| 规则层（牌型/炸弹顺序） | 0.5 |
| 房间状态机 + 消息 + 托管 | 2~3 |
| 结算与战绩落库 | 0.5~1 |
| 后台（表 + Java 分支） | 0.5 |
| 客户端 UI（Unity） | 5~10 |

**服务端合计约 4~5 人天/款**，远低于自研（8~15 人天/款）。

---

## 六、下一步可搬的游戏（网狐已有规则）

按难度从低到高：

1. **牛牛**（`GameLogic` 仅 8KB）
2. **跑得快2**（21KB）
3. **韶关麻将鸡胡**（21KB）
4. **宁海麻将 / 浙江麻将**（45KB / 37KB）

搬法完全相同，只需换 `<Game>` 名字、人数、牌型定义与结算规则。
