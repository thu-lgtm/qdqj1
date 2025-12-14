#include "Strategy.h"
#include "Game.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

// --- 人类策略实现 ---
Action HumanStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    // 获取当前可用的卡牌ID列表
    vector<int> avail = game.getAvailableCards();
    
    if(avail.empty()) return {0, -1}; // 无牌可拿（理论不应发生）

    int choice;
    while(true) {
        cout << "\n>>> " << me.name << " 请选择卡牌ID (输入 -1 查看你的奇迹): ";
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(10000, '\n'); continue;
        }

        if (choice == -1) {
            // 查看奇迹详情
            for(int i=0; i<me.wonders.size(); i++) {
                Wonder& w = me.wonders[i];
                cout << "W" << i << ": " << w.name << " | 费用: " << w.cost.toString() 
                     << " | " << w.desc << (w.built ? " [已建]" : "") << endl;
            }
            continue;
        }

        // 验证选择是否合法
        bool valid = false;
        for(int id : avail) if(id == choice) valid = true;
        if(valid) break;
        cout << "无效选择 (卡牌已被拿走或被遮挡)。" << endl;
    }

    // 获取卡牌引用用于显示信息
    Card& card = game.getCard(choice);

    cout << "已选卡牌: " << card.name << endl;
    cout << "1. 建造 (费用: " << game.calculateCardCost(me, opp, card) << "金)" << endl;
    cout << "2. 弃牌 (获得: " << (2 + me.getYellowCount()) << " 金币)" << endl;
    cout << "3. 建造奇迹" << endl;
    
    int type;
    cin >> type;
    
    Action action;
    action.cardId = choice;
    action.type = type;

    if (type == 3) {
        cout << "请输入要建造的奇迹编号 (0-3): ";
        cin >> action.wonderIdx;
    }

    return action;
}

// --- 贪婪AI策略实现 ---
Action GreedyAIStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    cout << "Bot (" << me.name << ") 正在思考..." << endl;
    
    vector<int> avail = game.getAvailableCards();
    if(avail.empty()) return {2, -1}; // 异常情况

    int bestCardId = avail[0];
    float maxScore = -1000.0;
    int bestType = 2; // 默认弃牌
    int bestWonderIdx = -1;

    for(int id : avail) {
        Card& c = game.getCard(id);
        int cost = game.calculateCardCost(me, opp, c);
        bool canAfford = (me.coins >= cost);

        // 评分逻辑：根据卡牌属性打分
        float score = 0;
        if (canAfford) {
            score += c.points * 1.5;
            score += c.shields * 4.0; // AI偏好军事
            if (c.science != NO_SYMBOL) score += 5.0; // AI偏好科技
            if (c.type == RAW_MATERIAL || c.type == MANUFACTURED) score += 3.0; // 资源重要
            if (c.chainProvide != NONE_CHAIN) score += 1.0;
            
            if (score > maxScore) { 
                maxScore = score; 
                bestCardId = id; 
                bestType = 1; // 建造
            }
        } else {
            // 买不起，尝试造奇迹
            bool foundWonder = false;
            for(int w=0; w<me.wonders.size(); w++) {
                if(!me.wonders[w].built) {
                    int wCost = game.calculateResourceCost(me, opp, me.wonders[w].cost, RAW_MATERIAL, true);
                    if(me.coins >= wCost) {
                        float wScore = me.wonders[w].points + 5.0;
                        if (wScore > maxScore) {
                            maxScore = wScore;
                            bestCardId = id;
                            bestType = 3; // 造奇迹
                            bestWonderIdx = w;
                            foundWonder = true;
                        }
                    }
                }
            }
            
            // 如果既买不起卡也造不了奇迹，比较弃牌价值
            if (!foundWonder) {
                float discardVal = 1.0; // 弃牌保底分
                if (discardVal > maxScore) {
                    maxScore = discardVal;
                    bestCardId = id;
                    bestType = 2; // 弃牌
                }
            }
        }
    }
    
    return {bestType, bestCardId, bestWonderIdx};
}

// --- 随机AI策略实现 ---
Action RandomAIStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    cout << "Random Bot 正在随机行动..." << endl;
    vector<int> avail = game.getAvailableCards();
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(avail.begin(), avail.end(), std::default_random_engine(seed));
    
    int cardId = avail[0];
    Card& c = game.getCard(cardId);
    
    // 优先尝试建造
    int cost = game.calculateCardCost(me, opp, c);
    if(me.coins >= cost) {
        return {1, cardId, -1};
    }
    
    // 其次尝试弃牌 (最稳妥)
    return {2, cardId, -1};
}