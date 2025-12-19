/**
* @file Strategy.h
 * @brief 策略模式定义
 * 定义了玩家（无论是人类还是AI）如何进行决策的接口。
 */

#ifndef STRATEGY_H
#define STRATEGY_H

#include "Structs.h"
#include <string>
#include <vector>

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
     */
    virtual Action makeDecision(Game& game, Player& me, Player& opp) = 0;

    /**
     * @brief 奇迹轮抽选择
     * @param options 当前可选的奇迹列表
     * @param game 游戏实例
     * @param me 当前玩家
     * @return 选中的奇迹在 options 列表中的索引 (0 ~ size-1)
     */
    virtual int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) = 0;

    // 从弃牌堆中选择一张卡牌的索引
    virtual int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) = 0;

    // 从对手的可摧毁卡牌列表中选择一张的索引
    virtual int chooseCardToDestroy(const std::vector<Card>& targets, Game& game) = 0;
};

/**
 * @class HumanStrategy
 * @brief 人类玩家策略
 */
class HumanStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
    int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) override;
    int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game)override;
    int chooseCardToDestroy(const std::vector<Card>& targets, Game& game)override;
};

/**
 * @class GreedyAIStrategy
 * @brief 贪婪AI策略
 */
class GreedyAIStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
    int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) override;
    int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game)override;
    int chooseCardToDestroy(const std::vector<Card>& targets, Game& game)override;
};

/**
 * @class RandomAIStrategy
 * @brief 随机AI策略
 */
class RandomAIStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
    int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) override;
    int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game)override;
    int chooseCardToDestroy(const std::vector<Card>& targets, Game& game)override;
};

#endif