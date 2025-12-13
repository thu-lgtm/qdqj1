/**
 * @file Game.h
 * @brief 游戏主引擎类
 * 负责管理游戏状态、规则结算和流程控制。
 * 重构说明：移除了硬编码的玩家逻辑，改为持有 PlayerStrategy 指针。
 */

#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Structs.h"
#include "Enums.h"
#include "Strategy.h" // 引入策略定义
#include <vector>
#include <string>
#include <memory>

class Game {
private:
    // --- 核心状态 ---
    Player p1;
    Player p2;
    // 使用 unique_ptr 管理策略对象的生命周期
    std::unique_ptr<PlayerStrategy> strategyP1;
    std::unique_ptr<PlayerStrategy> strategyP2;

    int militaryTrack = 0; // 0在中间, 正数偏向P1进攻(P2劣势), 负数偏向P2进攻
    int currentAge = 1;
    std::vector<BoardSlot> board;
    std::vector<ProgressToken> availableTokens;
    bool gameOver = false;
    std::string winner = "";
    bool p1Turn = true; // true: P1行动, false: P2行动

    // --- 内部逻辑方法 ---
    void initTokens();
    std::vector<Card> getDeck(int age);
    void setupAge(int age);
    void dealWonders();

    // 动作执行方法 (Execute Action)
    void executeAction(Player& active, Player& passive, Action action);

    // 各种具体效果结算
    void applyMilitary(Player& attacker, int shields);
    void checkScienceTokens(Player& p);
    void applyTokenImmediateEffect(Player& p, ProgressToken t);
    void applyCardEffect(Player& p, Card& c);
    void applyWonderEffect(Player& p, Wonder& w);
    void checkFaceUps();
    int calculateGuildPoints(Player& owner, Player& opp, GuildType type);
    void checkInstantWin();
    void calculateFinalScore();
    void printState();

public:
    /**
     * @brief 构造函数
     * @param p1Name 玩家1名字
     * @param s1 玩家1的策略(Human/AI)
     * @param p2Name 玩家2名字
     * @param s2 玩家2的策略
     */
    Game(std::string p1Name, std::unique_ptr<PlayerStrategy> s1,
         std::string p2Name, std::unique_ptr<PlayerStrategy> s2);

    // --- 开放给 Strategy 使用的查询接口 (ReadOnly) ---

    // 获取当前所有可拿取的卡牌ID
    std::vector<int> getAvailableCards();

    // 根据ID获取卡牌信息的引用
    Card& getCard(int id);

    // 计算卡牌建造费用 (公开包装器)
    int calculateCardCost(Player& buyer, Player& opponent, Card& card);

    // 计算资源/奇迹费用 (公开包装器)
    int calculateResourceCost(Player& buyer, Player& opponent, Cost cost, CardType type, bool isWonder);

    // 运行游戏循环
    void run();
};

#endif