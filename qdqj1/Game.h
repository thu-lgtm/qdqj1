/**
 * @file Game.h
 * @brief 游戏主引擎类 (已修复规则缺陷版)
 */

#ifndef GAME_H
#define GAME_H

#include <iostream>
#include "Player.h"
#include "Structs.h"
#include "Enums.h"
#include "Strategy.h"
#include "Extension.h"
#include <vector>
#include <string>
#include <memory>

class Game {
private:
    // --- 核心状态 ---
    Player p1;
    Player p2;
    std::unique_ptr<PlayerStrategy> strategyP1;
    std::unique_ptr<PlayerStrategy> strategyP2;
    std::vector<std::unique_ptr<Extension>> extensions;

    std::vector<Card> discardPile;

    int militaryTrack = 0;
    // 增加军事标记状态位，防止重复触发
    bool milTokenP1_2 = false; // P1进攻达到2格的标记是否已被移除
    bool milTokenP1_5 = false; // P1进攻达到5格的标记是否已被移除
    bool milTokenP2_2 = false; // P2进攻达到2格 (即-2)
    bool milTokenP2_5 = false; // P2进攻达到5格 (即-5)

    int currentAge = 1;
    std::vector<BoardSlot> board;

    std::vector<ProgressToken> availableTokens; // 版图上的5个
    std::vector<ProgressToken> boxTokens;       // 留在盒子里的，供大图书馆使用

    bool gameOver = false;
    std::string winner = "";
    bool p1Turn = true;

    // --- 内部逻辑方法 ---
    void initTokens();
    std::vector<Card> getDeck(int age);
    void setupAge(int age);
    void dealWonders();

    bool isAvailable(int id);
    void executeAction(Player& active, Player& passive, Action action);

    // 具体效果结算
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
    Game(std::string p1Name, std::unique_ptr<PlayerStrategy> s1,
         std::string p2Name, std::unique_ptr<PlayerStrategy> s2);

    std::vector<int> getAvailableCards();
    Card& getCard(int id);
    int calculateCardCost(Player& buyer, Player& opponent, Card& card);
    static int calculateResourceCost(Player& buyer, Player& opponent, const Cost& cost, CardType type, bool isWonder);
    CostBreakdown calculateCostDetails(Player& buyer, Player& opponent, Cost cost);
    void destroyCard(Player& targetPlayer, CardType targetType);
    int getTotalBuiltWonders();
    void run();

    void addExtension(std::unique_ptr<Extension> ext) {
        std::cout << ">>> 激活扩展包: " << ext->getName() << " <<<" << std::endl;
        ext->onGameStart(*this);
        extensions.push_back(std::move(ext));
    }
};

#endif