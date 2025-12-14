/**
 * @file Player.h
 * @brief 玩家类声明
 * 定义玩家的状态数据和基本查询方法。
 * 作用：存储玩家的所有信息，如持有的资源、金币、卡牌、奇迹等。
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Structs.h"
#include <string>
#include <vector>
#include <map>
#include <set>

/**
 * @class Player
 * @brief 玩家类
 * 代表游戏中的一方（人类玩家或电脑 Bot）。
 */
struct Player {
    std::string name;           // 玩家姓名 (例如 "玩家" 或 "Bot")
    int coins = 7;              // 当前持有的金币数量 (初始为7)
    int victoryPoints = 0;      // 当前直接获得的胜利点数 (通常来自蓝色市政卡)

    /**
     * @brief 资源产量统计
     * 键(Key): 资源类型 (WOOD, CLAY...)
     * 值(Value): 每回合自动产出的数量
     * 作用：用于计算购买卡牌时的基础资源是否足够。
     */
    std::map<Resource, int> production;

    /**
     * @brief 科技符号统计
     * 键(Key): 科技符号类型 (GLOBE, TABLET...)
     * 值(Value): 拥有的数量
     * 作用：用于判断科技胜利(集齐6种)和获得科技币(集齐一对)。
     */
    std::map<ScienceSymbol, int> scienceSymbols;

    /**
     * @brief 连锁符号集合
     * 存储玩家目前拥有的所有连锁符号。
     * 使用 set 是为了快速查找是否存在某个符号 (O(logN) 复杂度)。
     * 作用：判断是否满足下一张卡的连锁免费条件。
     */
    std::set<ChainSymbol> chainIcons;

    /**
     * @brief 拥有的奇迹列表
     * 存储分配给该玩家的 4 个奇迹。
     */
    std::vector<Wonder> wonders;

    /**
     * @brief 已建造的所有卡牌
     * 存储玩家已经建造好的卡牌对象。
     * 作用：用于游戏结束时的公会算分，或某些奇迹(摧毁卡牌)的目标查找。
     */
    std::vector<Card> builtCards;

    /**
     * @brief 拥有的科技进步指示物
     * 存储玩家获得的绿色科技币。
     */
    std::vector<ProgressToken> tokens;

    /**
     * @brief 贸易固定价格开关
     * 键(Key): 资源类型
     * 值(Value): 是否固定为 1 金币
     * 作用：如果有黄色贸易卡(如木材储备)，对应的资源买入价锁定为 1，无视对手产量。
     */
    std::map<Resource, bool> tradeFixed;

    /**
     * @brief 构造函数
     * 初始化玩家状态。
     * @param n 玩家姓名
     */
    Player(std::string n);

    /**
     * @brief 获取已建造的黄色商业卡数量
     * 作用：用于计算弃牌收益 (收益 = 2 + 黄卡数)。
     * @return 黄色卡牌的数量
     */
    int getYellowCount();

    /**
     * @brief 获取已建成的奇迹数量
     * 作用：用于某些公会卡(如建筑师公会)的计分。
     * @return 已建成(built=true)的奇迹数量
     */
    int getWonderCount();

    /**
     * @brief 统计拥有的不同科技符号种类数
     * 作用：用于判断科技胜利 (集齐6种)。
     * 特殊逻辑：如果拥有“法律”(P_LAW)科技币，返回值会在实际种类上 +1。
     * @return 不同符号的种类数量
     */
    int countScienceDistinct();

    /**
     * @brief 检查是否拥有特定的科技币
     * @param t 要检查的科技币类型
     * @return 如果拥有则返回 true，否则返回 false
     */
    bool hasToken(ProgressToken t);
};

#endif