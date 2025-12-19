/**
* @file Strategy.h
 * @brief 策略模式定义 (已修复接口版)
 */

#ifndef STRATEGY_H
#define STRATEGY_H

#include "Structs.h"
#include <string>
#include <vector>

class Game;
struct Player;

struct Action {
    int type; // 1:建造, 2:弃牌, 3:奇迹
    int cardId;
    int wonderIdx = -1;
};

class PlayerStrategy {
public:
    virtual ~PlayerStrategy() = default;

    virtual Action makeDecision(Game& game, Player& me, Player& opp) = 0;
    virtual int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) = 0;
    virtual int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) = 0;
    virtual int chooseCardToDestroy(const std::vector<Card>& targets, Game& game) = 0;

    // 新增接口：从给定的科技币列表中选择一个（用于大图书馆）
    virtual int chooseToken(const std::vector<ProgressToken>& options, Game& game) = 0;
};

class HumanStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
    int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) override;
    int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) override;
    int chooseCardToDestroy(const std::vector<Card>& targets, Game& game) override;
    int chooseToken(const std::vector<ProgressToken>& options, Game& game) override;
};

class GreedyAIStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
    int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) override;
    int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) override;
    int chooseCardToDestroy(const std::vector<Card>& targets, Game& game) override;
    int chooseToken(const std::vector<ProgressToken>& options, Game& game) override;
};

class RandomAIStrategy : public PlayerStrategy {
public:
    Action makeDecision(Game& game, Player& me, Player& opp) override;
    int chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) override;
    int chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) override;
    int chooseCardToDestroy(const std::vector<Card>& targets, Game& game) override;
    int chooseToken(const std::vector<ProgressToken>& options, Game& game) override;
};

#endif