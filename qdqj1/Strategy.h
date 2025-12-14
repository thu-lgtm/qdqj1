/**
* @file Strategy.h
 * @brief 策略模式定义
 * 定义了玩家（无论是人类还是AI）如何进行决策的接口。
 * 符合项目要求：易于添加新的AI类型。
 */

#ifndef STRATEGY_H
#define STRATEGY_H

#include "Structs.h"
#include <string>

// 前向声明，避免循环依赖
class Game;
struct Player;

/**
 * @struct Action
 * @brief 定义一次操作的决策结果
 */
struct Action {
    // 1:建造卡牌, 2:弃牌换钱, 3:建造奇迹
    int type;
    // 操作的目标卡牌在版图上的ID
    int cardId;
    // 如果是建造奇迹，指定奇迹在玩家列表中的索引
    int wonderIdx = -1;
};

/**
 * @class PlayerStrategy
 * @brief 策略抽象基类 (Abstract Strategy)
 */
class PlayerStrategy {
public:
    virtual ~PlayerStrategy() = default;

    /**
     * @brief 制定决策
     * @param game 游戏实例（用于查询版图和规则）
     * @param me 当前做出决策的玩家
     * @param opp 对手玩家
     * @return Action 具体的决策动作
     */
    virtual Action makeDecision(Game& game, Player& me, Player& opp) = 0;
};

/**
 * @class HumanStrategy
 * @brief 人类玩家策略：通过控制台输入做决定
 */
class HumanStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
};

/**
 * @class GreedyAIStrategy
 * @brief 贪婪AI策略：原有的AI逻辑封装
 */
class GreedyAIStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
};

/**
 * @class RandomAIStrategy
 * @brief 随机AI策略：满足项目“至少有一个简单AI”的保底要求
 */
class RandomAIStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
};

#endif