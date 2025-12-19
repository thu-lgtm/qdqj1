/**
 * @file Game.cpp
 * @brief 游戏核心逻辑的具体实现 (重构版)
 * 包含了所有的规则判定、卡牌数据、经济计算公式以及基于策略模式的流程控制。
 */

#include "Game.h"
#include "Strategy.h" // 引入策略定义
#include "CardDatabase.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

/**
 * @brief 构造函数实现
 * 初始化游戏：
 * 1. 注入玩家名称和策略
 * 2. 初始化科技币堆
 * 3. 发奇迹
 * 4. 摆好第一时代的卡牌
 */
Game::Game(string p1Name, unique_ptr<PlayerStrategy> s1,
           string p2Name, unique_ptr<PlayerStrategy> s2)
    : p1(p1Name), strategyP1(std::move(s1)),
      p2(p2Name), strategyP2(std::move(s2))
{
    initTokens();
    dealWonders();
    setupAge(1);
}

/**
 * @brief 初始化科技币
 */
void Game::initTokens() {
    vector<ProgressToken> all = {P_AGRICULTURE, P_ARCHITECTURE, P_ECONOMY, P_LAW, P_MASONRY, P_MATHEMATICS, P_PHILOSOPHY, P_STRATEGY, P_THEOLOGY, P_URBANISM};
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(all.begin(), all.end(), default_random_engine(seed));

    for(int i=0; i<5; i++) {
        availableTokens.push_back(all[i]);
    }
}

/**
 * @brief 生成指定时代的牌堆
 * 包含所有卡牌数据的硬编码工厂方法。
 */
vector<Card> Game::getDeck(int age) {
    // 从数据库加载基础卡牌
    vector<Card> deck = CardDatabase::loadCardsForAge(age);

    // 洗牌并截取
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(deck.begin(), deck.end(), default_random_engine(seed));
    deck.resize(20);
    return deck;
}

/**
 * @brief 设置时代的版图布局
 */
void Game::setupAge(int age) {
    board.clear();
    vector<Card> deck = getDeck(age);
    int currentId = 0;

    // --- AGE 1: 金字塔结构 ---
    if (age == 1) {
        int rows[] = {2,3,4,5,6};
        vector<vector<int>> rowIds(5);
        for(int r=0; r<5; r++){
            for(int c=0; c<rows[r]; c++){
                BoardSlot s; s.id = currentId; s.card = deck[currentId]; s.row = r; s.col = c;
                s.faceUp = (r%2 == 0);
                board.push_back(s); rowIds[r].push_back(currentId++);
            }
        }
        for(int r=0; r<4; r++){
            for(int i=0; i<rowIds[r].size(); i++){
                board[rowIds[r][i]].coveredBy.push_back(rowIds[r+1][i]);
                board[rowIds[r][i]].coveredBy.push_back(rowIds[r+1][i+1]);
            }
        }
    }
    // --- AGE 2: 倒金字塔结构 ---
    else if (age == 2) {
        int rows[] = {6,5,4,3,2};
        vector<vector<int>> rowIds(5);
        for(int r=0; r<5; r++){
            for(int c=0; c<rows[r]; c++){
                BoardSlot s; s.id = currentId; s.card = deck[currentId]; s.row = r; s.col = c;
                s.faceUp = (r%2 == 0);
                board.push_back(s); rowIds[r].push_back(currentId++);
            }
        }
        for(int r=0; r<4; r++){
            for(int i=0; i<rowIds[r].size(); i++){
                if(i-1 >= 0) board[rowIds[r][i]].coveredBy.push_back(rowIds[r+1][i-1]);
                if(i < rowIds[r+1].size()) board[rowIds[r][i]].coveredBy.push_back(rowIds[r+1][i]);
            }
        }
    }
    // --- AGE 3: 蛇形布局 ---
    else {
        int rows[] = {2, 3, 4, 2, 4, 3, 2};
        vector<vector<int>> age3RowIds(7);
        for(int r=0; r<7; r++){
            for(int c=0; c<rows[r]; c++){
                BoardSlot s; s.id = currentId; s.card = deck[currentId]; s.row = r; s.col = c;
                s.faceUp = (r % 2 == 0);
                board.push_back(s);
                age3RowIds[r].push_back(currentId++);
            }
        }
        auto addCover = [&](int me, int cover) { board[me].coveredBy.push_back(cover); };
        addCover(0, 2); addCover(0, 3); addCover(1, 3); addCover(1, 4);
        addCover(2, 5); addCover(2, 6); addCover(3, 6); addCover(3, 7); addCover(4, 7); addCover(4, 8);
        addCover(5, 9); addCover(6, 9); addCover(7, 10); addCover(8, 10);
        addCover(9, 11); addCover(9, 12); addCover(10, 13); addCover(10, 14);
        addCover(11, 15); addCover(12, 15); addCover(12, 16); addCover(13, 16); addCover(13, 17); addCover(14, 17);
        addCover(15, 18); addCover(16, 18); addCover(16, 19); addCover(17, 19);
    }
}

/**
 * @brief 发放奇迹
 */
// 辅助函数：执行单次选择并将奇迹移交给玩家
void performPick(Player& p, PlayerStrategy* strategy, std::vector<Wonder>& pool, Game& game) {
    if (pool.empty()) return;

    int choiceIdx = 0;
    // 如果只剩一张，直接获得，无需选择
    if (pool.size() > 1) {
        choiceIdx = strategy->chooseWonder(pool, game, p);
    } else {
        std::cout << ">>> " << p.name << " 自动获得最后一张奇迹: " << pool[0].name << "\n";
    }

    p.wonders.push_back(pool[choiceIdx]);
    pool.erase(pool.begin() + choiceIdx);
}

void Game::dealWonders() {
    std::cout << "\n========================================\n";
    std::cout << "          奇迹轮抽阶段 (Wonder Draft)     \n";
    std::cout << "========================================\n";

    // 1. 加载并洗混所有 12 个奇迹
    std::vector<Wonder> allWonders = CardDatabase::loadWonders();
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(allWonders.begin(), allWonders.end(), std::default_random_engine(seed));

    // 截取前 8 张用于游戏 (规则：只用 8 张)
    std::vector<Wonder> round1Wonders(allWonders.begin(), allWonders.begin() + 4);
    std::vector<Wonder> round2Wonders(allWonders.begin() + 4, allWonders.begin() + 8);

    // --- 第一轮轮抽 (4张) ---
    // 顺序：P1选1 -> P2选1 -> P2选1 -> P1得1 (逻辑等同于 P1选1, P2选2, P1得余下)
    std::cout << "\n--- 第一轮轮抽 (由 " << p1.name << " 先选) ---\n";
    performPick(p1, strategyP1.get(), round1Wonders, *this);
    performPick(p2, strategyP2.get(), round1Wonders, *this);
    performPick(p2, strategyP2.get(), round1Wonders, *this);
    performPick(p1, strategyP1.get(), round1Wonders, *this);

    // --- 第二轮轮抽 (4张) ---
    // 顺序：P2选1 -> P1选1 -> P1选1 -> P2得1 (由上一轮后选者 P2 先选)
    std::cout << "\n--- 第二轮轮抽 (由 " << p2.name << " 先选) ---\n";
    performPick(p2, strategyP2.get(), round2Wonders, *this);
    performPick(p1, strategyP1.get(), round2Wonders, *this);
    performPick(p1, strategyP1.get(), round2Wonders, *this);
    performPick(p2, strategyP2.get(), round2Wonders, *this);

    std::cout << "\n>>> 奇迹分配完成! <<<\n";
    std::cout << p1.name << " 的奇迹: ";
    for(auto& w : p1.wonders) std::cout << "[" << w.name << "] ";
    std::cout << "\n" << p2.name << " 的奇迹: ";
    for(auto& w : p2.wonders) std::cout << "[" << w.name << "] ";
    std::cout << "\n\n";
}

/**
 * @brief 检查卡牌是否可拿 (辅助方法)
 */
bool Game::isAvailable(int id) {
    if (board[id].taken) return false;
    for (int coverId : board[id].coveredBy) {
        if (!board[coverId].taken) return false;
    }
    return true;
}

// --- 公共查询接口实现 (供 Strategy 使用) ---

std::vector<int> Game::getAvailableCards() {
    std::vector<int> avail;
    for(auto& slot : board) {
        if(isAvailable(slot.id)) avail.push_back(slot.id);
    }
    return avail;
}

Card& Game::getCard(int id) {
    return board[id].card;
}


/**
 * @brief 计算资源费用 (核心算法)
 */
int Game::calculateResourceCost(Player& buyer, Player& opponent, const Cost& cost, CardType type, bool isWonder) {
    if (cost.coins > 0) return cost.coins;
    int discount = 0;
    if (isWonder && buyer.hasToken(P_ARCHITECTURE)) discount = 2;
    if (!isWonder && type == CIVILIAN && buyer.hasToken(P_MASONRY)) discount = 2;

    int totalGoldNeeded = cost.coins;
    vector<int> missingCosts;

    for (auto const& [res, needed] : cost.resources) {
        int produced = buyer.production[res];
        if (produced < needed) {
            int missing = needed - produced;
            int oppProd = opponent.production[res];
            int pricePerUnit = 2 + oppProd;
            if (buyer.tradeFixed.count(res) && buyer.tradeFixed.at(res)) pricePerUnit = 1;
            for(int k=0; k<missing; k++) missingCosts.push_back(pricePerUnit);
        }
    }
    sort(missingCosts.rbegin(), missingCosts.rend());
    for(int i = 0; i < missingCosts.size(); i++) {
        if (i < discount) continue;
        totalGoldNeeded += missingCosts[i];
    }
    return totalGoldNeeded;
}

/**
 * @brief 计算卡牌最终费用 (包含连锁)
 */
int Game::calculateCardCost(Player& buyer, Player& opponent, Card& card) {
    if (card.chainCost != NONE_CHAIN) {
        if (buyer.chainIcons.count(card.chainCost)) return 0;
    }
    return calculateResourceCost(buyer, opponent, card.cost, card.type, false);
}

/**
 * @brief 应用科技币即时奖励
 */
void Game::applyTokenImmediateEffect(Player& p, ProgressToken t) {
    if (t == P_AGRICULTURE || t == P_URBANISM) {
        p.coins += 6;
        cout << ">>> 科技奖励：获得 6 金币！" << endl;
    }
}

/**
 * @brief 检查科技配对
 */
void Game::checkScienceTokens(Player& p) {
    map<ScienceSymbol, int> counts = p.scienceSymbols;
    for(auto const& [sym, count] : counts) {
        if(count == 2) {
            if(availableTokens.empty()) return;
            // 简单处理：如果是AI或人类都简单处理，或者可以优化为策略选择。
            // 这里为了简化，默认拿第一个。在更复杂的实现中，这也可以通过 Strategy 来询问用户。
            ProgressToken t = availableTokens.back();
            availableTokens.pop_back();
            p.tokens.push_back(t);
            cout << ">>> " << p.name << " 收集一对科技符号，获得: " << getTokenName(t) << endl;
            applyTokenImmediateEffect(p, t);
            p.scienceSymbols[sym] = 3; // 防止重复触发
        }
    }
}

/**
 * @brief 应用军事效果
 */
void Game::applyMilitary(Player& attacker, int shields) {
    Player& defender = (&attacker == &p1) ? p2 : p1;
    int oldTrack = militaryTrack;

    if (&attacker == &p1) militaryTrack += shields;
    else militaryTrack -= shields;

    int loss = 0;
    if (&attacker == &p1) {
        if (oldTrack < 2 && militaryTrack >= 2) loss = 2;
        if (oldTrack < 5 && militaryTrack >= 5) loss = 5;
    }
    else {
        if (oldTrack > -2 && militaryTrack <= -2) loss = 2;
        if (oldTrack > -5 && militaryTrack <= -5) loss = 5;
    }

    if (loss > 0) {
        int actualLoss = min(defender.coins, loss);
        defender.coins -= actualLoss;
        cout << "\n>>> [军事掠夺!] " << defender.name << " 失去了 " << actualLoss << " 金币! <<<\n" << endl;
    }
}

/**
 * @brief 应用建造卡牌后的效果
 */
void Game::applyCardEffect(Player& p, Card& c) {
    p.victoryPoints += c.points;
    if (c.shields > 0) {
        int bonus = (p.hasToken(P_STRATEGY)) ? 1 : 0;
        applyMilitary(p, c.shields + bonus);
    }
    if (c.science != NO_SYMBOL) {
        p.scienceSymbols[c.science]++;
        checkScienceTokens(p);
    }
    for(auto const& [res, count] : c.production) p.production[res] += count;

    bool chained = (c.chainCost != NONE_CHAIN && p.chainIcons.count(c.chainCost));
    if(chained && p.hasToken(P_URBANISM)) {
        p.coins += 4;
        cout << ">>> 城市规划奖励：获得 4 金币！" << endl;
    }
    p.coins += c.coinProduction;
    if (c.chainProvide != NONE_CHAIN) p.chainIcons.insert(c.chainProvide);
    if (c.tradeDiscountRes != NO_RES) p.tradeFixed[c.tradeDiscountRes] = true;
    p.builtCards.push_back(c);
}

/**
 * @brief 应用建造奇迹后的效果
 */
void Game::applyWonderEffect(Player& p, Wonder& w) {
    p.victoryPoints += w.points;
    if (w.shields > 0) applyMilitary(p, w.shields);
    p.coins += w.coins;
    w.built = true;
    PlayerStrategy* strat = (&p == &p1) ? strategyP1.get() : strategyP2.get();
    Player& opp = (&p == &p1) ? p2 : p1;

    //摩索拉斯陵墓 (从弃牌堆免费建造)
    if (w.name == "哈利卡納斯的摩索拉斯陵墓" || w.name == "Mausoleum") { // 名字需匹配数据库
        if (!discardPile.empty()) {
            int idx = strat->chooseCardFromDiscard(discardPile, *this);
            if (idx >= 0 && idx < discardPile.size()) {
                Card picked = discardPile[idx];
                // 从弃牌堆移除
                discardPile.erase(discardPile.begin() + idx);
                std::cout << ">>> 摩索拉斯陵墓复活了: " << picked.name << "\n";
                // 免费建造
                applyCardEffect(p, picked);
            }
        } else {
            std::cout << ">>> 弃牌堆为空，无法复活卡牌。\n";
        }
    }

    //摧毁对手卡牌 (宙斯神像/大竞技场)
    if (w.name == "奥林匹亞宙斯神像" || w.name == "Statue of Zeus") {
        destroyCard(opp, RAW_MATERIAL); // 摧毁棕卡
    }
    if (w.name == "馬克西姆斯競技場" || w.name == "Circus Maximus") {
        destroyCard(opp, MANUFACTURED); // 摧毁灰卡
    }

    if (p.hasToken(P_THEOLOGY)) w.extraTurn = true;

    if(w.name == "亚壁古道") {
        Player& opp = (&p == &p1) ? p2 : p1;
        opp.coins = max(0, opp.coins - 3);
    }
    if(w.name == "大图书馆" && !availableTokens.empty()) {
        ProgressToken t = availableTokens.back();
        availableTokens.pop_back();
        p.tokens.push_back(t);
        cout << ">>> 大图书馆奖励: " << getTokenName(t) << endl;
        applyTokenImmediateEffect(p, t);
    }
}

// 辅助函数：执行摧毁逻辑
void Game::destroyCard(Player& targetPlayer, CardType targetType) {
    std::vector<Card> targets;
    std::vector<int> originalIndices; // 记录在 builtCards 中的原始下标

    // 筛选目标卡牌
    for(size_t i=0; i<targetPlayer.builtCards.size(); i++) {
        if (targetPlayer.builtCards[i].type == targetType) {
            targets.push_back(targetPlayer.builtCards[i]);
            originalIndices.push_back(i);
        }
    }

    if (targets.empty()) {
        std::cout << ">>> 对手没有可摧毁的卡牌。\n";
        return;
    }

    // 获取当前回合发起者的策略
    PlayerStrategy* strat = (&targetPlayer == &p1) ? strategyP2.get() : strategyP1.get();

    int choice = strat->chooseCardToDestroy(targets, *this);
    if (choice >= 0 && choice < targets.size()) {
        int removeIdx = originalIndices[choice];
        Card removedCard = targetPlayer.builtCards[removeIdx];

        std::cout << ">>> " << removedCard.name << " 被摧毁并移入弃牌堆！\n";

        // 1. 移入弃牌堆
        discardPile.push_back(removedCard);

        // 2. 从玩家城市移除
        targetPlayer.builtCards.erase(targetPlayer.builtCards.begin() + removeIdx);

        // 3. 回滚资源产量！
        for(auto const& [res, count] : removedCard.production) {
            targetPlayer.production[res] -= count;
        }
    }
}

/**
 * @brief 检查并翻开解锁的卡牌
 */
void Game::checkFaceUps() {
    for (auto& slot : board) {
        if (!slot.taken && !slot.faceUp && isAvailable(slot.id)) slot.faceUp = true;
    }
}

/**
 * @brief 计算公会得分
 */
int Game::calculateGuildPoints(Player& owner, Player& opp, GuildType type) {
    switch(type) {
        case G_MERCHANT: return max(owner.getYellowCount(), opp.getYellowCount()) * 1;
        case G_SHIPOWNER: return (owner.production[WOOD]+owner.production[CLAY]+owner.production[STONE]+owner.production[GLASS]+owner.production[PAPYRUS]);
        case G_BUILDER: return max(owner.getWonderCount(), opp.getWonderCount()) * 2;
        case G_SCIENTIST: return 1;
        default: return 0;
    }
}

/**
 * @brief 打印游戏状态 (UI)
 */
void Game::printState() {
    cout << string(50, '\n');
    cout << "=== 七大奇迹：对决 | 时代 " << currentAge << " ===" << endl;

    cout << "科技: [P2] ";
    for(int i=1; i<=6; i++) cout << (p2.countScienceDistinct() >= i ? "O" : ".");
    cout << " | [P1] ";
    for(int i=1; i<=6; i++) cout << (p1.countScienceDistinct() >= i ? "O" : ".");

    cout << "\n军事: [P2] ";
    for(int i=-9; i<0; i++) cout << (militaryTrack == -i ? "X" : ".");
    cout << "|";
    for(int i=1; i<=9; i++) cout << (militaryTrack == -i ? "X" : ".");
    cout << " [P1]" << endl;

    auto printPlayer = [&](Player& p) {
        cout << "--- " << p.name << " ---" << endl;
        cout << "金币: " << p.coins << " | VP: " << p.victoryPoints << endl;
        cout << "资源: ";
        for(auto const& [r, c] : p.production) if(c>0) cout << c << (r==WOOD?"木 ":r==CLAY?"土 ":r==STONE?"石 ":r==GLASS?"玻 ":"纸 ");
        cout << "\n连锁: "; for(auto c : p.chainIcons) cout << getChainName(c) << " ";
        cout << "\n奇迹: "; for(auto& w : p.wonders) if(w.built) cout << "[" << w.name << "] ";
        cout << endl;
    };

    printPlayer(p2);
    cout << "\n--- 桌面卡牌 ---" << endl;
    vector<int> avail = getAvailableCards();
    if (avail.empty()) cout << "(本时代已无卡牌)" << endl;
    for(int id : avail) {
        BoardSlot& s = board[id];
        int cost = calculateCardCost(p1Turn ? p1 : p2, p1Turn ? p2 : p1, s.card);
        cout << "ID[" << (id<10?"0":"") << id << "] " << s.card.getTypeColor() << " " << s.card.name
             << "\t| 费:" << cost << "\t| 效:" << s.card.getEffect() << endl;
    }
    cout << endl;
    printPlayer(p1);
}

// 实现统计函数
int Game::getTotalBuiltWonders() {
    return p1.getWonderCount() + p2.getWonderCount();
}

/**
 * @brief 执行策略传来的动作
 */
void Game::executeAction(Player& active, Player& passive, Action action) {
    BoardSlot& slot = board[action.cardId];

    auto applyEconomy = [&](Player& spender, Player& earner, int amount) {
        if(amount > 0 && earner.hasToken(P_ECONOMY)) {
            earner.coins += 1;
            cout << ">>> 经济学触发：" << earner.name << " 获得 1 金币税收！" << endl;
        }
    };

    if (action.type == 1) { // 建造卡牌
        CostBreakdown cost = calculateCostDetails(active, passive, slot.card.cost);

        // 检查连锁（免费）
        bool isFreeChain = (slot.card.chainCost != NONE_CHAIN && active.chainIcons.count(slot.card.chainCost));
        if (isFreeChain) {
            cost = {0, 0, 0};
            // 城市规划奖励
            if (active.hasToken(P_URBANISM)) {
                active.coins += 4;
                cout << ">>> [城市规划] 连锁建造获得 4 金币！\n";
            }
        }
        if (active.coins >= cost.totalCost) {
            active.coins -= cost.totalCost;
            passive.coins += cost.coinsToOpponent; // 经济学：钱给对手 [cite: 276]

            if(cost.coinsToOpponent > 0)
                cout << ">>> [经济学] " << passive.name << " 获得了 " << cost.coinsToOpponent << " 贸易金币！\n";
            applyCardEffect(active, slot.card);
            cout << active.name << " 建造了 " << slot.card.name << endl;
            p1Turn = !p1Turn;
        } else {
            cout << "错误：金币不足，自动转为弃牌。" << endl;
            action.type = 2;
        }
    }

    if (action.type == 2) { // 弃牌
        discardPile.push_back(slot.card);// 加入弃牌堆
        int gain = 2 + active.getYellowCount();
        active.coins += gain;
        cout << active.name << " 弃掉了 " << slot.card.name << " 获得 " << gain << " 金币" << endl;
        p1Turn = !p1Turn;
    }
    else if (action.type == 3) { // 建造奇迹
        // 检查是否已经达到 7 个奇迹上限
        if (getTotalBuiltWonders() >= 7) {
            cout << ">>> [规则限制] 全场已建成 7 个奇迹，无法再建造！操作自动转为弃牌。 <<<" << endl;
            // 强制转为弃牌逻辑
            int gain = 2 + active.getYellowCount();
            active.coins += gain;
            cout << active.name << " 被迫弃掉了 " << slot.card.name << " 获得 " << gain << " 金币" << endl;
            p1Turn = !p1Turn;
        }
        else if(action.wonderIdx >= 0 && action.wonderIdx < active.wonders.size()) {
            Wonder& w = active.wonders[action.wonderIdx];
            int wCost = calculateResourceCost(active, passive, w.cost, RAW_MATERIAL, true);

            if (!w.built && active.coins >= wCost) {
                active.coins -= wCost;
                applyEconomy(active, passive, wCost);
                applyWonderEffect(active, w);
                cout << active.name << " 建造了奇迹: " << w.name << endl;

                // [新增规则] 检查是否刚刚触发 7 奇迹上限，若是则销毁剩余奇迹
                if (getTotalBuiltWonders() >= 7) {
                    cout << "\n========================================================" << endl;
                    cout << ">>> [规则触发] 第 7 个奇迹已建成！场上剩余的奇迹已被移除游戏！ <<<" << endl;
                    cout << "========================================================" << endl;

                    // 定义 Lambda 函数移除未建成的奇迹
                    auto removeUnbuilt = [](Player& p) {
                        for (auto it = p.wonders.begin(); it != p.wonders.end(); ) {
                            if (!it->built) {
                                cout << "--- " << p.name << " 的未建成奇迹 [" << it->name << "] 被移除。" << endl;
                                it = p.wonders.erase(it); // 从列表中彻底删除
                            } else {
                                ++it;
                            }
                        }
                    };
                    removeUnbuilt(p1);
                    removeUnbuilt(p2);
                }

                if(w.extraTurn) cout << ">>> " << active.name << " 获得额外回合！" << endl;
                else p1Turn = !p1Turn;

            } else {
                 cout << "错误：无法建造奇迹(钱不够或已建)，自动转为弃牌。" << endl;
                 int gain = 2 + active.getYellowCount();
                 active.coins += gain;
                 p1Turn = !p1Turn;
            }
        }
    }

    slot.taken = true;
    checkFaceUps();
}

CostBreakdown Game::calculateCostDetails(Player& buyer, Player& opponent, Cost cost) {
    CostBreakdown cb;
    cb.coinsToBank = cost.coins; // 卡牌自带的金币费用总是给银行

    for (auto const& [res, needed] : cost.resources) {
        int produced = buyer.production[res];
        if (produced < needed) {
            int missing = needed - produced;
            int oppProd = opponent.production[res];

            // 计算单价：如果有黄色贸易卡，固定为1，否则为 2 + 对手产量
            int pricePerUnit = 2 + oppProd;
            if (buyer.tradeFixed.count(res) && buyer.tradeFixed.at(res)) {
                pricePerUnit = 1;
            }

            int tradeCost = missing * pricePerUnit;

            // 核心修正：如果有经济学标记，这笔钱给对手；否则给银行
            if (opponent.hasToken(P_ECONOMY)) {
                cb.coinsToOpponent += tradeCost;
            } else {
                cb.coinsToBank += tradeCost;
            }
        }
    }
    cb.totalCost = cb.coinsToBank + cb.coinsToOpponent;
    return cb;
}

/**
 * @brief 检查即时胜利条件
 */
void Game::checkInstantWin() {
    if (militaryTrack >= 9) {
        gameOver = true; winner = p1.name + " (军事压制)";
    } else if (militaryTrack <= -9) {
        gameOver = true; winner = p2.name + " (军事压制)";
    }
    if (p1.countScienceDistinct() >= 6) {
        gameOver = true; winner = p1.name + " (科技压制)";
    } else if (p2.countScienceDistinct() >= 6) {
        gameOver = true; winner = p2.name + " (科技压制)";
    }
}

/**
 * @brief 计算最终得分
 */
void Game::calculateFinalScore() {
    int p1Score = p1.victoryPoints + p1.coins/3 + (militaryTrack > 0 ? militaryTrack : 0);
    int p2Score = p2.victoryPoints + p2.coins/3 + (militaryTrack < 0 ? abs(militaryTrack) : 0);

    for(auto& c : p1.builtCards) if(c.type == GUILD) p1Score += calculateGuildPoints(p1, p2, c.guildType);
    for(auto& c : p2.builtCards) if(c.type == GUILD) p2Score += calculateGuildPoints(p2, p1, c.guildType);

    auto addTokenPoints = [&](Player& p, int& score) {
        for(auto t : p.tokens) {
            if(t == P_AGRICULTURE) score += 4;
            if(t == P_PHILOSOPHY) score += 7;
            if(t == P_MATHEMATICS) score += (3 * p.tokens.size());
        }
    };
    addTokenPoints(p1, p1Score);
    addTokenPoints(p2, p2Score);

    cout << "\n=== 游戏结束 ===" << endl;
    cout << p1.name << " 总分: " << p1Score << endl;
    cout << p2.name << " 总分: " << p2Score << endl;
    winner = (p1Score > p2Score) ? p1.name : p2.name;
}

/**
 * @brief 游戏主循环
 */
void Game::run() {
    while (!gameOver) {
        bool allTaken = true;
        for(auto& s : board) if(!s.taken) allTaken = false;

        if (allTaken) {
            if (currentAge == 3) {
                gameOver = true;
                calculateFinalScore();
                break;
            }
            currentAge++;
            setupAge(currentAge);
            cout << "\n>>> 进入时代 " << currentAge << " <<<\n";
            if(militaryTrack < 0) p1Turn = true;
            else if(militaryTrack > 0) p1Turn = false;
            continue;
        }

        printState();
        checkInstantWin();
        if (gameOver) break;

        Player& active = p1Turn ? p1 : p2;
        Player& passive = p1Turn ? p2 : p1;
        PlayerStrategy* strat = p1Turn ? strategyP1.get() : strategyP2.get();

        cout << "\n>>> 轮到 " << active.name << " 行动 <<<" << endl;
        Action action = strat->makeDecision(*this, active, passive);
        executeAction(active, passive, action);

        if(!gameOver) {
            cout << "按回车继续...";
            cin.ignore(10000, '\n');
            if(cin.peek() == '\n') cin.get();
        }
    }
    cout << "最终胜者: " << winner << endl;
}