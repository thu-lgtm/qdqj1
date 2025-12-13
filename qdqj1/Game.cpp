/**
 * @file Game.cpp
 * @brief 游戏核心逻辑的具体实现 (重构版)
 * 包含了所有的规则判定、卡牌数据、经济计算公式以及基于策略模式的流程控制。
 */

#include "Game.h"
#include "Strategy.h" // 引入策略定义
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
    vector<Card> deck;

    // --- 时代 I (Age 1) ---
    if (age == 1) {
        // 原料卡 (棕色)
        deck.push_back(Card("伐木场", RAW_MATERIAL, {}, 0, 0).setProd({{WOOD, 1}}));
        deck.push_back(Card("采木营地", RAW_MATERIAL, {1, {}}, 0, 0).setProd({{WOOD, 1}}));
        deck.push_back(Card("黏土池", RAW_MATERIAL, {}, 0, 0).setProd({{CLAY, 1}}));
        deck.push_back(Card("黏土坑", RAW_MATERIAL, {1, {}}, 0, 0).setProd({{CLAY, 1}}));
        deck.push_back(Card("采石场", RAW_MATERIAL, {}, 0, 0).setProd({{STONE, 1}}));
        deck.push_back(Card("石坑", RAW_MATERIAL, {1, {}}, 0, 0).setProd({{STONE, 1}}));

        // 制品卡 (灰色)
        deck.push_back(Card("玻璃厂", MANUFACTURED, {1, {}}, 0, 0).setProd({{GLASS, 1}}));
        deck.push_back(Card("压纸机", MANUFACTURED, {1, {}}, 0, 0).setProd({{PAPYRUS, 1}}));

        // 军事卡 (红色)
        deck.push_back(Card("瞭望塔", MILITARY, {0, {}}, 0, 1));
        deck.push_back(Card("马厩", MILITARY, {0, {{WOOD, 1}}}, 0, 1).setChain(HORSESHOE));
        deck.push_back(Card("驻军", MILITARY, {0, {{CLAY, 1}}}, 0, 1).setChain(SWORD));
        deck.push_back(Card("栅栏", MILITARY, {0, {{STONE, 1}}}, 0, 1).setChain(TOWER));

        // 科技卡 (绿色)
        deck.push_back(Card("工坊", SCIENTIFIC, {0, {{PAPYRUS, 1}}}, 1, 0, GLOBE).setChain(CHAIN_GLOBE));
        deck.push_back(Card("药剂师", SCIENTIFIC, {0, {{GLASS, 1}}}, 1, 0, WHEEL).setChain(GEAR));
        deck.push_back(Card("缮写室", SCIENTIFIC, {0, {{PAPYRUS, 1}}}, 0, 0, QUILL).setChain(BOOK));
        deck.push_back(Card("药师", SCIENTIFIC, {0, {{GLASS, 1}}}, 0, 0, MORTAR).setChain(CHAIN_MORTAR));

        // 商业卡 (黄色)
        deck.push_back(Card("石头储备", COMMERCIAL, {3, {}}, 0, 0).setTrade(STONE));
        deck.push_back(Card("粘土储备", COMMERCIAL, {3, {}}, 0, 0).setTrade(CLAY));
        deck.push_back(Card("木材储备", COMMERCIAL, {3, {}}, 0, 0).setTrade(WOOD));

        // 市政卡 (蓝色)
        deck.push_back(Card("剧院", CIVILIAN, {0, {}}, 3).setChain(MASK));
        deck.push_back(Card("祭坛", CIVILIAN, {0, {}}, 3).setChain(SUN));
        deck.push_back(Card("浴场", CIVILIAN, {0, {{STONE, 1}}}, 3).setChain(DROP));

        // 填充至23张
        while(deck.size() < 23) deck.push_back(Card("雇佣兵", MILITARY, {2, {}}, 0, 1));
    }
    // --- 时代 II (Age 2) ---
    else if (age == 2) {
        deck.push_back(Card("锯木厂", RAW_MATERIAL, {2, {}}, 0, 0).setProd({{WOOD, 2}}));
        deck.push_back(Card("砖厂", RAW_MATERIAL, {2, {}}, 0, 0).setProd({{CLAY, 2}}));
        deck.push_back(Card("层状采石场", RAW_MATERIAL, {2, {}}, 0, 0).setProd({{STONE, 2}}));
        deck.push_back(Card("吹玻璃工", MANUFACTURED, {0, {{WOOD, 1}}}, 0, 0).setProd({{GLASS, 1}}));
        deck.push_back(Card("干燥室", MANUFACTURED, {0, {{STONE, 1}}}, 0, 0).setProd({{PAPYRUS, 1}}));

        deck.push_back(Card("城墙", MILITARY, {0, {{STONE, 2}}}, 0, 2));
        deck.push_back(Card("靶场", MILITARY, {0, {{WOOD, 2}, {GLASS, 1}}}, 0, 2).setChain(TARGET));
        deck.push_back(Card("阅兵场", MILITARY, {0, {{CLAY, 2}, {PAPYRUS, 1}}}, 0, 2).setChain(HELMET, HORSESHOE));

        deck.push_back(Card("广场", COMMERCIAL, {3, {{CLAY, 1}}}, 0, 0).setProd({{GLASS,1}}));
        deck.push_back(Card("商队旅馆", COMMERCIAL, {2, {{GLASS,1}, {PAPYRUS,1}}}, 0, 0).setProd({{WOOD,1}}));
        deck.push_back(Card("酿酒厂", COMMERCIAL, {0, {}}, 0, 0).setChain(BARREL));

        deck.push_back(Card("法庭", CIVILIAN, {0, {{WOOD, 2}, {GLASS, 1}}}, 5));
        deck.push_back(Card("雕像", CIVILIAN, {0, {{CLAY, 2}}}, 4).setChain(PILLAR, MASK));
        deck.push_back(Card("神庙", CIVILIAN, {0, {{WOOD, 1}, {PAPYRUS, 1}}}, 4).setChain(NONE_CHAIN, SUN));
        deck.push_back(Card("水渠", CIVILIAN, {0, {{STONE, 3}}}, 5).setChain(NONE_CHAIN, DROP));
        deck.push_back(Card("讲坛", CIVILIAN, {0, {{STONE, 1}, {WOOD, 1}}}, 4));

        deck.push_back(Card("诊所", SCIENTIFIC, {0, {{CLAY, 2}, {GLASS, 1}}}, 2, 0, MORTAR).setChain(CHAIN_MORTAR, CHAIN_MORTAR));
        deck.push_back(Card("实验室", SCIENTIFIC, {0, {{WOOD, 2}, {GLASS, 1}}}, 1, 0, GLOBE).setChain(CHAIN_GLOBE, CHAIN_GLOBE));
        deck.push_back(Card("图书馆", SCIENTIFIC, {0, {{STONE, 2}, {PAPYRUS, 1}}}, 2, 0, TABLET).setChain(BOOK, BOOK));
        deck.push_back(Card("学校", SCIENTIFIC, {0, {{WOOD, 1}, {PAPYRUS, 2}}}, 1, 0, WHEEL).setChain(HARP, CHAIN_WHEEL));

        while(deck.size() < 23) deck.push_back(Card("雇佣兵II", MILITARY, {3, {}}, 0, 2));
    }
    // --- 时代 III (Age 3) ---
    else {
        deck.push_back(Card("兵工厂", MILITARY, {0, {{CLAY, 3}, {WOOD, 2}}}, 0, 3));
        deck.push_back(Card("法院", CIVILIAN, {0, {{CLAY, 2}, {PAPYRUS, 1}}}, 5));
        deck.push_back(Card("学院", SCIENTIFIC, {0, {{STONE, 1}, {GLASS, 2}}}, 3, 0, SCALE));
        deck.push_back(Card("书房", SCIENTIFIC, {0, {{WOOD, 1}, {PAPYRUS, 2}}}, 3, 0, SCALE));
        deck.push_back(Card("商会", COMMERCIAL, {0, {{PAPYRUS, 2}}}, 3, 0).setChain(NONE_CHAIN, MASK));
        deck.push_back(Card("港口", COMMERCIAL, {0, {{WOOD, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 3, 0).setChain(NONE_CHAIN, BARREL));
        deck.push_back(Card("军械库", MILITARY, {0, {{STONE, 3}, {GLASS, 1}}}, 0, 3));
        deck.push_back(Card("宫殿", CIVILIAN, {0, {{STONE, 1}, {CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 7));
        deck.push_back(Card("市政厅", CIVILIAN, {0, {{STONE, 3}, {WOOD, 2}}}, 6));
        deck.push_back(Card("方尖碑", CIVILIAN, {0, {{STONE, 2}, {GLASS, 1}}}, 5));
        deck.push_back(Card("防御工事", MILITARY, {0, {{STONE, 2}, {CLAY, 2}, {PAPYRUS, 1}}}, 0, 2).setChain(NONE_CHAIN, TOWER));
        deck.push_back(Card("攻城工坊", MILITARY, {0, {{WOOD, 3}, {GLASS, 1}}}, 0, 2).setChain(NONE_CHAIN, TARGET));
        deck.push_back(Card("竞技场", MILITARY, {0, {{STONE, 2}, {CLAY, 2}}}, 0, 2).setChain(NONE_CHAIN, BARREL));

        deck.push_back(Card("大学", SCIENTIFIC, {0, {{CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 2, 0, GLOBE).setChain(NONE_CHAIN, CHAIN_GLOBE));
        deck.push_back(Card("天文台", SCIENTIFIC, {0, {{STONE, 1}, {PAPYRUS, 2}}}, 2, 0, WHEEL).setChain(NONE_CHAIN, GEAR));

        // 公会卡
        deck.push_back(Card("商人公会", GUILD, {0, {{WOOD, 1}, {CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}).setGuild(G_MERCHANT));
        deck.push_back(Card("船东公会", GUILD, {0, {{STONE, 1}, {GLASS, 1}, {PAPYRUS, 1}}}).setGuild(G_SHIPOWNER));
        deck.push_back(Card("建筑师公会", GUILD, {0, {{STONE, 2}, {CLAY, 1}, {WOOD, 1}}}).setGuild(G_BUILDER));
        while(deck.size() < 23) deck.push_back(Card("老兵", MILITARY, {0, {{CLAY,1}}}, 0, 2));
    }

    // 洗牌并截取20张
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
void Game::dealWonders() {
    vector<Wonder> all;
    all.push_back(Wonder("亚壁古道", {0, {{STONE, 2}, {CLAY, 2}, {PAPYRUS, 1}}}, 3, 0, 3, false, true, "对手丢3金,再来一回合"));
    all.push_back(Wonder("大竞技场", {0, {{STONE, 2}, {WOOD, 1}, {GLASS, 1}}}, 3, 1, 0, false, false, "1盾,3分"));
    all.push_back(Wonder("罗德岛巨像", {0, {{CLAY, 3}, {GLASS, 1}}}, 3, 2, 0, false, false, "2盾,3分"));
    all.push_back(Wonder("大图书馆", {0, {{WOOD, 3}, {PAPYRUS, 1}, {GLASS, 1}}}, 4, 0, 0, false, false, "4分,随机科技币(自动)"));
    all.push_back(Wonder("斯芬克斯像", {0, {{STONE, 3}, {GLASS, 2}}}, 6, 0, 0, false, true, "6分,再来一回合"));
    all.push_back(Wonder("比雷埃夫斯港", {0, {{WOOD, 2}, {CLAY, 1}, {PAPYRUS, 1}}}, 2, 0, 0, false, true, "2分,再来一回合"));
    all.push_back(Wonder("金字塔", {0, {{STONE, 3}, {PAPYRUS, 1}}}, 9, 0, 0, false, false, "9分"));
    all.push_back(Wonder("阿尔忒弥斯神庙", {0, {{WOOD, 1}, {STONE, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 0, 0, 12, false, true, "12金,再来一回合"));

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(all.begin(), all.end(), default_random_engine(seed));

    for(int i=0; i<4; i++) p1.wonders.push_back(all[i]);
    for(int i=4; i<8; i++) p2.wonders.push_back(all[i]);
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
int Game::calculateResourceCost(Player& buyer, Player& opponent, Cost cost, CardType type, bool isWonder) {
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

    if (action.type == 1) { // 建造
        int cost = calculateCardCost(active, passive, slot.card);
        if (active.coins >= cost) {
            active.coins -= cost;
            // 简单处理：所有费用视为流向对手触发经济学(简化逻辑)
            applyEconomy(active, passive, cost);
            applyCardEffect(active, slot.card);
            cout << active.name << " 建造了 " << slot.card.name << endl;
            p1Turn = !p1Turn;
        } else {
            cout << "错误：金币不足，自动转为弃牌。" << endl;
            action.type = 2;
        }
    }

    if (action.type == 2) { // 弃牌
        int gain = 2 + active.getYellowCount();
        active.coins += gain;
        cout << active.name << " 弃掉了 " << slot.card.name << " 获得 " << gain << " 金币" << endl;
        p1Turn = !p1Turn;
    }
    else if (action.type == 3) { // 奇迹
        if(action.wonderIdx >= 0 && action.wonderIdx < active.wonders.size()) {
            Wonder& w = active.wonders[action.wonderIdx];
            int wCost = calculateResourceCost(active, passive, w.cost, RAW_MATERIAL, true);
            if (!w.built && active.coins >= wCost) {
                active.coins -= wCost;
                applyEconomy(active, passive, wCost);
                applyWonderEffect(active, w);
                cout << active.name << " 建造了奇迹: " << w.name << endl;
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