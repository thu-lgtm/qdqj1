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

int HumanStrategy::chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) {
    std::cout << "\n>>> 轮到 " << me.name << " 选择奇迹:\n";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << i << ". [" << options[i].name << "] "
                  << "费用:" << options[i].cost.toString()
                  << " | 效果:" << options[i].desc << "\n";
    }

    int choice;
    while (true) {
        std::cout << "请输入编号 (0-" << options.size() - 1 << "): ";
        if (std::cin >> choice && choice >= 0 && choice < options.size()) {
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "无效的选择，请重试。\n";
    }
}

int HumanStrategy::chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) {
    if (pile.empty()) return -1;
    std::cout << "\n>>> [摩索拉斯陵墓] 请从弃牌堆选择一张卡牌复活:\n";
    for(size_t i=0; i<pile.size(); i++) {
        std::cout << i << ". " << pile[i].getTypeColor() << " " << pile[i].name
                  << " (" << pile[i].getEffect() << ")\n";
    }
    int choice;
    std::cout << "输入编号: ";
    std::cin >> choice; // 简化版，实际应加边界检查
    return choice;
}

int HumanStrategy::chooseCardToDestroy(const std::vector<Card>& targets, Game& game) {
    if (targets.empty()) return -1;
    std::cout << "\n>>> [摧毁效果] 选择对手的一张卡牌摧毁:\n";
    for(size_t i=0; i<targets.size(); i++) {
        std::cout << i << ". " << targets[i].getTypeColor() << " " << targets[i].name << "\n";
    }
    int choice;
    std::cout << "输入编号: ";
    std::cin >> choice;
    return choice;
}

// --- 贪婪AI策略实现 ---
Action GreedyAIStrategy::makeDecision(Game& game, Player& me, Player& opp) {
    cout << "Bot (" << me.name << ") 正在思考..." << endl;

    vector<int> avail = game.getAvailableCards();
    if(avail.empty()) return {2, -1};

    int bestCardId = avail[0];
    float maxScore = -1000.0;
    int bestType = 2; // 默认弃牌
    int bestWonderIdx = -1;

    // [新增] 获取当前已建奇迹数
    int builtWonders = game.getTotalBuiltWonders();

    for(int id : avail) {
        Card& c = game.getCard(id);
        int cost = game.calculateCardCost(me, opp, c);
        bool canAfford = (me.coins >= cost);

        float score = 0;
        if (canAfford) {
            // ... (原有的卡牌评分逻辑不变) ...
            score += c.points * 1.5;
            score += c.shields * 4.0;
            if (c.science != NO_SYMBOL) score += 5.0;
            if (c.type == RAW_MATERIAL || c.type == MANUFACTURED) score += 3.0;
            if (c.chainProvide != NONE_CHAIN) score += 1.0;

            if (score > maxScore) {
                maxScore = score;
                bestCardId = id;
                bestType = 1;
            }
        } else {
            // [修改] 只有当全场奇迹数 < 7 时才尝试计算建造奇迹的收益
            if (builtWonders < 7) {
                bool foundWonder = false;
                for(int w=0; w<me.wonders.size(); w++) {
                    if(!me.wonders[w].built) {
                        int wCost = game.calculateResourceCost(me, opp, me.wonders[w].cost, RAW_MATERIAL, true);
                        if(me.coins >= wCost) {
                            float wScore = me.wonders[w].points + 5.0;

                            // [新增] 如果这是第 7 个奇迹，稍微提高优先级以进行抢占
                            if (builtWonders == 6) wScore += 10.0;

                            if (wScore > maxScore) {
                                maxScore = wScore;
                                bestCardId = id;
                                bestType = 3;
                                bestWonderIdx = w;
                                foundWonder = true;
                            }
                        }
                    }
                }
            }

            // 弃牌逻辑...
            if (maxScore < 0) { // 如果还没有找到好的操作
                 float discardVal = 1.0;
                 if (discardVal > maxScore) {
                    maxScore = discardVal;
                    bestCardId = id;
                    bestType = 2;
                }
            }
        }
    }
    
    return {bestType, bestCardId, bestWonderIdx};
}

int GreedyAIStrategy::chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) {
    // 简单的贪婪评估：优先选有点数或额外回合的
    int bestIdx = 0;
    int maxScore = -1;

    for (size_t i = 0; i < options.size(); ++i) {
        int score = 0;
        const Wonder& w = options[i];

        score += w.points * 2;              // 分数权重
        if (w.extraTurn) score += 5;        // 额外回合很强
        if (w.shields > 0) score += 3;      // 军事
        // 简单的资源平衡检查略过，AI 优先抢高分卡

        if (score > maxScore) {
            maxScore = score;
            bestIdx = i;
        }
    }
    std::cout << "Bot " << me.name << " 选择了 " << options[bestIdx].name << "\n";
    return bestIdx;
}

int GreedyAIStrategy::chooseCardFromDiscard(const std::vector<Card>& pile, Game& game) {
    if (pile.empty()) return -1;
    // 简单贪婪：选分最高的
    int bestIdx = 0;
    int maxPoints = -1;
    for(size_t i=0; i<pile.size(); i++) {
        if (pile[i].points > maxPoints) {
            maxPoints = pile[i].points;
            bestIdx = i;
        }
    }
    std::cout << "Bot 选择了复活: " << pile[bestIdx].name << "\n";
    return bestIdx;
}

int GreedyAIStrategy::chooseCardToDestroy(const std::vector<Card>& targets, Game& game) {
    if (targets.empty()) return -1;
    // 简单贪婪：优先摧毁产出资源多的，或者是高分的（虽然棕/灰卡分低，但有的可能有分）
    std::cout << "Bot 选择了摧毁对手的: " << targets[0].name << "\n";
    return 0;
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

int RandomAIStrategy::chooseWonder(const std::vector<Wonder>& options, Game& game, Player& me) {
    // 随机选择
    int idx = rand() % options.size();
    std::cout << "RandomBot " << me.name << " 随机选择了 " << options[idx].name << "\n";
    return idx;
}
