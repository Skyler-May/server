-- ============================================================
-- 比鸡逐局明细表
--
-- 背景：
--   比鸡原先只在内存算金币，最终只把「押金余额」写到 cash_pledge，
--   没有逐局输赢明细 → 聚合平台拿不到流水（返水/打码/风控全部失效）。
--
--   本表由 NiuMa C++ 在每局结算时写入，供 web-server 的
--   BiJiSettleProvider 扫描并推送给聚合平台。
--
-- 设计要点：
--   · id 自增，同时作为增量扫描水位（provider 按 id > lastId 拉取）
--   · venue_id + round_no 标识一局；player_id 为 NiuMa 内部玩家 ID
--   · win_gold 为扣除抽水后的实际输赢（与玩家金币一致）
--   · tax 为平台抽水（1%），便于对账
--   · 与 cash_pledge 的关系：cash_pledge 是押金余额快照，
--     本表是逐局流水，两者互不覆盖
-- ============================================================

CREATE TABLE IF NOT EXISTS `game_bi_ji_round` (
    `id`         BIGINT       NOT NULL AUTO_INCREMENT COMMENT '自增ID（扫描水位）',
    `venue_id`   VARCHAR(32)  NOT NULL COMMENT '场地ID',
    `round_no`   INT          NOT NULL COMMENT '局号（场地内递增）',
    `player_id`  VARCHAR(32)  NOT NULL COMMENT '玩家ID',
    `seat`       INT          NOT NULL DEFAULT 0 COMMENT '座位号',
    `score`      INT          NOT NULL DEFAULT 0 COMMENT '牌型得分',
    `di_zhu`     INT          NOT NULL DEFAULT 0 COMMENT '底注',
    `win_gold`   BIGINT       NOT NULL DEFAULT 0 COMMENT '输赢金币（已扣抽水）',
    `tax`        BIGINT       NOT NULL DEFAULT 0 COMMENT '平台抽水',
    `time`       DATETIME     NOT NULL COMMENT '结算时间',
    PRIMARY KEY (`id`),
    KEY `idx_venue_round` (`venue_id`, `round_no`),
    KEY `idx_player` (`player_id`),
    KEY `idx_time` (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='比鸡逐局输赢明细';
