#include "Strategy.h"
#include "Game.h"
#include <iostream>
#include <limits>
#include <random>

using namespace std;

// --- HumanStrategy ---

Action HumanStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    int choice;
    cout << "请选择操作 (1:建造, 2:弃牌, 3:建造奇迹): ";
    while(!(cin >> choice) || choice < 1 || choice > 3) {
        cin.clear(); cin.ignore(10000, '\n'); cout << "无效输入，重试: ";
    }

    int cardId;
    cout << "输入卡牌ID: ";
    cin >> cardId;

    int wIdx = -1;
    if (choice == 3) {
        cout << "输入要建造的奇迹序号 (0-" << me.wonders.size()-1 << "): ";
        cin >> wIdx;
    }

    return {choice, cardId, wIdx};
}

int HumanStrategy::chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) {
    cout << me.name << " 请选择奇迹 (0-" << options.size()-1 << "): \n";
    for(int i=0; i<options.size(); i++) {
        cout << i << ": " << options[i].name << " (" << options[i].desc << ")\n";
    }
    int idx;
    while(!(cin >> idx) || idx < 0 || idx >= options.size()) {
        cin.clear(); cin.ignore(10000, '\n'); cout << "无效，重选: ";
    }
    return idx;
}

int HumanStrategy::chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) {
    if(pile.empty()) return -1;
    cout << "请选择要复活的卡牌 (弃牌堆): \n";
    for(int i=0; i<pile.size(); i++) {
        cout << i << ": " << pile[i].name << " (" << pile[i].getTypeColor() << ")\n";
    }
    int idx;
    cin >> idx;
    return idx;
}

int HumanStrategy::chooseCardToDestroy(const std::vector<Card>& targets, Game& game) {
    if(targets.empty()) return -1;
    cout << "请选择要摧毁的对手卡牌: \n";
    for(int i=0; i<targets.size(); i++) {
        cout << i << ": " << targets[i].name << "\n";
    }
    int idx;
    cin >> idx;
    return idx;
}

// [修复3] Human实现
int HumanStrategy::chooseToken(const std::vector<ProgressToken>& options, Game& game) {
    if(options.empty()) return -1;
    cout << "大图书馆生效！请从以下科技币中选择一个: \n";
    for(int i=0; i<options.size(); i++) {
        cout << i << ": " << getTokenName(options[i]) << "\n";
    }
    int idx;
    while(!(cin >> idx) || idx < 0 || idx >= options.size()) {
        cin.clear(); cin.ignore(10000, '\n'); cout << "无效，重选: ";
    }
    return idx;
}


// --- GreedyAIStrategy ---

Action GreedyAIStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    // 简单贪婪：优先买能买得起的、分最高的卡
    vector<int> avail = game.getAvailableCards();
    for(int id : avail) {
        Card& c = game.getCard(id);
        int cost = game.calculateCardCost(me, opp, c);
        if (me.coins >= cost) {
            return {1, id, -1};
        }
    }
    // 买不起就弃掉第一张
    if(!avail.empty()) return {2, avail[0], -1};
    return {2, 0, -1}; // fallback
}

int GreedyAIStrategy::chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) {
    return 0; // 总是选第一个
}
int GreedyAIStrategy::chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) {
    return pile.size() - 1; // 选刚弃的那张
}
int GreedyAIStrategy::chooseCardToDestroy(const std::vector<Card>& targets, Game& game) {
    return 0;
}
int GreedyAIStrategy::chooseToken(const std::vector<ProgressToken>& options, Game& game) {
    return 0; // 总是选第一个
}


// --- RandomAIStrategy ---
Action RandomAIStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    vector<int> avail = game.getAvailableCards();
    if(avail.empty()) return {2, 0, -1};
    int id = avail[rand() % avail.size()];
    return {1, id, -1}; // 尝试购买，买不起逻辑在Game::executeAction里会转为弃牌
}
int RandomAIStrategy::chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) {
    return rand() % options.size();
}
int RandomAIStrategy::chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) {
    return rand() % pile.size();
}
int RandomAIStrategy::chooseCardToDestroy(const std::vector<Card>& targets, Game& game) {
    return rand() % targets.size();
}
int RandomAIStrategy::chooseToken(const std::vector<ProgressToken>& options, Game& game) {
    return rand() % options.size();
}