/**
 * @file Game.cpp
 * @brief 游戏核心逻辑 (已修复缺陷版)
 */

#include "Game.h"
#include "Strategy.h"
#include "CardDatabase.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

Game::Game(string p1Name, unique_ptr<PlayerStrategy> s1,
           string p2Name, unique_ptr<PlayerStrategy> s2)
    : p1(p1Name), strategyP1(std::move(s1)),
      p2(p2Name), strategyP2(std::move(s2))
{
    initTokens();
    dealWonders();
    setupAge(1);
}

void Game::initTokens() {
    vector<ProgressToken> all = {P_AGRICULTURE, P_ARCHITECTURE, P_ECONOMY, P_LAW, P_MASONRY, P_MATHEMATICS, P_PHILOSOPHY, P_STRATEGY, P_THEOLOGY, P_URBANISM};
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(all.begin(), all.end(), default_random_engine(seed));

    availableTokens.clear();
    boxTokens.clear();

    // 版图上放 5 个
    for(int i=0; i<5; i++) {
        availableTokens.push_back(all[i]);
    }
    // 剩下的放入盒子，供大图书馆使用
    for(int i=5; i<all.size(); i++) {
        boxTokens.push_back(all[i]);
    }
}

vector<Card> Game::getDeck(int age) {
    vector<Card> deck = CardDatabase::loadCardsForAge(age);
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(deck.begin(), deck.end(), default_random_engine(seed));
    deck.resize(20);
    return deck;
}

void Game::setupAge(int age) {
    board.clear();
    vector<Card> deck = getDeck(age);
    int currentId = 0;
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
    } else if (age == 2) {
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
    } else {
        int rows[] = {2, 3, 4, 2, 4, 3, 2};
        for(int r=0; r<7; r++){
            for(int c=0; c<rows[r]; c++){
                BoardSlot s; s.id = currentId; s.card = deck[currentId]; s.row = r; s.col = c;
                s.faceUp = (r % 2 == 0);
                board.push_back(s);
                currentId++;
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
void performPick(Player& p, PlayerStrategy* strategy, std::vector<Wonder>& pool, Game& game) {
    if (pool.empty()) return;
    int choiceIdx = 0;
    if (pool.size() > 1) {
        choiceIdx = strategy->chooseWonder(pool, game, p);
    } else {
        std::cout << ">>> " << p.name << " 自动获得最后一张奇迹: " << pool[0].name << "\n";
    }
    p.wonders.push_back(pool[choiceIdx]);
    pool.erase(pool.begin() + choiceIdx);
}
void Game::dealWonders() {
    std::vector<Wonder> allWonders = CardDatabase::loadWonders();
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(allWonders.begin(), allWonders.end(), std::default_random_engine(seed));
    std::vector<Wonder> round1Wonders(allWonders.begin(), allWonders.begin() + 4);
    std::vector<Wonder> round2Wonders(allWonders.begin() + 4, allWonders.begin() + 8);
    performPick(p1, strategyP1.get(), round1Wonders, *this);
    performPick(p2, strategyP2.get(), round1Wonders, *this);
    performPick(p2, strategyP2.get(), round1Wonders, *this);
    performPick(p1, strategyP1.get(), round1Wonders, *this);
    performPick(p2, strategyP2.get(), round2Wonders, *this);
    performPick(p1, strategyP1.get(), round2Wonders, *this);
    performPick(p1, strategyP1.get(), round2Wonders, *this);
    performPick(p2, strategyP2.get(), round2Wonders, *this);
}
bool Game::isAvailable(int id) {
    if (board[id].taken) return false;
    for (int coverId : board[id].coveredBy) {
        if (!board[coverId].taken) return false;
    }
    return true;
}
std::vector<int> Game::getAvailableCards() {
    std::vector<int> avail;
    for(auto& slot : board) if(isAvailable(slot.id)) avail.push_back(slot.id);
    return avail;
}
Card& Game::getCard(int id) { return board[id].card; }
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
int Game::calculateCardCost(Player& buyer, Player& opponent, Card& card) {
    if (card.chainCost != NONE_CHAIN) {
        if (buyer.chainIcons.count(card.chainCost)) return 0;
    }
    return calculateResourceCost(buyer, opponent, card.cost, card.type, false);
}
void Game::applyTokenImmediateEffect(Player& p, ProgressToken t) {
    if (t == P_AGRICULTURE || t == P_URBANISM) {
        p.coins += 6;
        cout << ">>> 科技奖励：获得 6 金币！" << endl;
    }
}
void Game::checkScienceTokens(Player& p) {
    map<ScienceSymbol, int> counts = p.scienceSymbols;
    for(auto const& [sym, count] : counts) {
        if(count == 2) {
            if(availableTokens.empty()) return;
            ProgressToken t = availableTokens.back();
            availableTokens.pop_back();
            p.tokens.push_back(t);
            cout << ">>> " << p.name << " 收集一对科技符号，获得: " << getTokenName(t) << endl;
            applyTokenImmediateEffect(p, t);
            p.scienceSymbols[sym] = 3;
        }
    }
}


/**
 * @brief 应用军事效果
 * 增加标记状态检查，确保每个区间的扣钱只触发一次。
 */
void Game::applyMilitary(Player& attacker, int shields) {
    Player& defender = (&attacker == &p1) ? p2 : p1;
    int oldTrack = militaryTrack;

    if (&attacker == &p1) militaryTrack += shields;
    else militaryTrack -= shields;

    int loss = 0;

    // P1 进攻 (Track 变大)
    if (&attacker == &p1) {
        // 检查 2分线 (范围: 2~4)
        if (oldTrack < 2 && militaryTrack >= 2) {
            if (!milTokenP1_2) {
                loss += 2;
                milTokenP1_2 = true; // 移除标记
                cout << ">>> [军事] P1 突破第1防线，移除 2金 惩罚标记。" << endl;
            }
        }
        // 检查 5分线 (范围: 5~8)
        if (oldTrack < 5 && militaryTrack >= 5) {
            if (!milTokenP1_5) {
                loss += 5;
                milTokenP1_5 = true; // 移除标记
                cout << ">>> [军事] P1 突破第2防线，移除 5金 惩罚标记。" << endl;
            }
        }
    }
    // P2 进攻 (Track 变小)
    else {
        // 检查 2分线 (范围: -2~-4)
        if (oldTrack > -2 && militaryTrack <= -2) {
            if (!milTokenP2_2) {
                loss += 2;
                milTokenP2_2 = true;
                cout << ">>> [军事] P2 突破第1防线，移除 2金 惩罚标记。" << endl;
            }
        }
        // 检查 5分线 (范围: -5~-8)
        if (oldTrack > -5 && militaryTrack <= -5) {
            if (!milTokenP2_5) {
                loss += 5;
                milTokenP2_5 = true;
                cout << ">>> [军事] P2 突破第2防线，移除 5金 惩罚标记。" << endl;
            }
        }
    }

    if (loss > 0) {
        int actualLoss = min(defender.coins, loss);
        defender.coins -= actualLoss;
        cout << "\n>>> [军事掠夺!] " << defender.name << " 失去了 " << actualLoss << " 金币! <<<\n" << endl;
    }
}

/**
 * @brief 应用卡牌效果
 * 修正了 Strategy 科技币只对 MILITARY 卡牌生效的逻辑。
 */
void Game::applyCardEffect(Player& p, Card& c) {
    p.victoryPoints += c.points;
    if (c.shields > 0) {
        // 只有当卡牌类型是 MILITARY 时，Strategy 科技币才加盾
        int bonus = 0;
        if (c.type == MILITARY && p.hasToken(P_STRATEGY)) {
            bonus = 1;
            cout << ">>> [战略] 科技币生效，额外获得 1 盾牌！" << endl;
        }
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
        cout << ">>> [城市规划] 奖励：获得 4 金币！" << endl;
    }
    p.coins += c.coinProduction;
    if (c.chainProvide != NONE_CHAIN) p.chainIcons.insert(c.chainProvide);
    if (c.tradeDiscountRes != NO_RES) p.tradeFixed[c.tradeDiscountRes] = true;
    p.builtCards.push_back(c);
}

/**
 * @brief 应用奇迹效果
 */
void Game::applyWonderEffect(Player& p, Wonder& w) {
    p.victoryPoints += w.points;
    if (w.shields > 0) applyMilitary(p, w.shields);
    p.coins += w.coins;
    w.built = true;
    PlayerStrategy* strat = (&p == &p1) ? strategyP1.get() : strategyP2.get();
    Player& opp = (&p == &p1) ? p2 : p1;

    // 摩索拉斯陵墓
    if (w.name == "哈利卡納斯的摩索拉斯陵墓" || w.name == "Mausoleum") {
        if (!discardPile.empty()) {
            int idx = strat->chooseCardFromDiscard(discardPile, *this);
            if (idx >= 0 && idx < discardPile.size()) {
                Card picked = discardPile[idx];
                discardPile.erase(discardPile.begin() + idx);
                std::cout << ">>> 摩索拉斯陵墓复活了: " << picked.name << "\n";
                applyCardEffect(p, picked);
            }
        } else {
            std::cout << ">>> 弃牌堆为空，无法复活卡牌。\n";
        }
    }

    if (w.name == "奥林匹亞宙斯神像" || w.name == "Statue of Zeus") destroyCard(opp, RAW_MATERIAL);
    if (w.name == "馬克西姆斯競技場" || w.name == "Circus Maximus") destroyCard(opp, MANUFACTURED);

    if (p.hasToken(P_THEOLOGY)) w.extraTurn = true;

    if(w.name == "亚壁古道") {
        opp.coins = max(0, opp.coins - 3);
    }

    // 大图书馆：从盒子中随机抽3个，选1个
    if((w.name == "大图书馆" || w.name == "The Great Library") && !boxTokens.empty()) {
        std::vector<ProgressToken> options;
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(boxTokens.begin(), boxTokens.end(), std::default_random_engine(seed));

        // 抽取最多3个
        int count = min((int)boxTokens.size(), 3);
        for(int i=0; i<count; i++) options.push_back(boxTokens[i]);

        // 让玩家选择
        int choice = strat->chooseToken(options, *this);
        if(choice >= 0 && choice < options.size()) {
            ProgressToken t = options[choice];
            p.tokens.push_back(t);
            cout << ">>> 大图书馆奖励: " << getTokenName(t) << endl;
            applyTokenImmediateEffect(p, t);

            // 按照规则，剩下的应该放回盒子（这里boxTokens里还是乱序的，不需要特别处理，只是没被选中的还在里面）
            // 从 boxTokens 里移除被选中的那个。
            // 找到被选中的 token 在 boxTokens 中的位置并移除
            // options是副本，从 boxTokens 移除
             for(auto it = boxTokens.begin(); it != boxTokens.end(); ++it) {
                 if(*it == t) {
                     boxTokens.erase(it);
                     break;
                 }
             }
        }
    } else if (w.name == "大图书馆" && boxTokens.empty()) {
        cout << ">>> 盒子中没有科技币了，大图书馆无法发动。" << endl;
    }
}

void Game::destroyCard(Player& targetPlayer, CardType targetType) {
    std::vector<Card> targets;
    std::vector<int> originalIndices;
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
    PlayerStrategy* strat = (&targetPlayer == &p1) ? strategyP2.get() : strategyP1.get();
    int choice = strat->chooseCardToDestroy(targets, *this);
    if (choice >= 0 && choice < targets.size()) {
        int removeIdx = originalIndices[choice];
        Card removedCard = targetPlayer.builtCards[removeIdx];
        std::cout << ">>> " << removedCard.name << " 被摧毁并移入弃牌堆！\n";
        discardPile.push_back(removedCard);
        targetPlayer.builtCards.erase(targetPlayer.builtCards.begin() + removeIdx);
        for(auto const& [res, count] : removedCard.production) {
            targetPlayer.production[res] -= count;
        }
    }
}
void Game::checkFaceUps() {
    for (auto& slot : board) {
        if (!slot.taken && !slot.faceUp && isAvailable(slot.id)) slot.faceUp = true;
    }
}
int Game::calculateGuildPoints(Player& owner, Player& opp, GuildType type) {
    switch(type) {
        case G_MERCHANT: return max(owner.getYellowCount(), opp.getYellowCount()) * 1;
        case G_SHIPOWNER: return (owner.production[WOOD]+owner.production[CLAY]+owner.production[STONE]+owner.production[GLASS]+owner.production[PAPYRUS]);
        case G_BUILDER: return max(owner.getWonderCount(), opp.getWonderCount()) * 2;
        case G_SCIENTIST: return 1;
        default: return 0;
    }
}
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
int Game::getTotalBuiltWonders() {
    return p1.getWonderCount() + p2.getWonderCount();
}
void Game::executeAction(Player& active, Player& passive, Action action) {
    BoardSlot& slot = board[action.cardId];
    auto applyEconomy = [&](Player& spender, Player& earner, int amount) {
        if(amount > 0 && earner.hasToken(P_ECONOMY)) {
            earner.coins += 1;
            cout << ">>> 经济学触发：" << earner.name << " 获得 1 金币税收！" << endl;
        }
    };
    if (action.type == 1) {
        CostBreakdown cost = calculateCostDetails(active, passive, slot.card.cost);
        bool isFreeChain = (slot.card.chainCost != NONE_CHAIN && active.chainIcons.count(slot.card.chainCost));
        if (isFreeChain) {
            cost = {0, 0, 0};
            if (active.hasToken(P_URBANISM)) {
                active.coins += 4;
                cout << ">>> [城市规划] 连锁建造获得 4 金币！\n";
            }
        }
        if (active.coins >= cost.totalCost) {
            active.coins -= cost.totalCost;
            passive.coins += cost.coinsToOpponent;
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
    if (action.type == 2) {
        discardPile.push_back(slot.card);
        int gain = 2 + active.getYellowCount();
        active.coins += gain;
        cout << active.name << " 弃掉了 " << slot.card.name << " 获得 " << gain << " 金币" << endl;
        p1Turn = !p1Turn;
    }
    else if (action.type == 3) {
        if (getTotalBuiltWonders() >= 7) {
            cout << ">>> [规则限制] 全场已建成 7 个奇迹，无法再建造！操作自动转为弃牌。 <<<" << endl;
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
                if (getTotalBuiltWonders() >= 7) {
                    cout << "\n========================================================" << endl;
                    cout << ">>> [规则触发] 第 7 个奇迹已建成！场上剩余的奇迹已被移除游戏！ <<<" << endl;
                    cout << "========================================================" << endl;
                    auto removeUnbuilt = [](Player& p) {
                        for (auto it = p.wonders.begin(); it != p.wonders.end(); ) {
                            if (!it->built) {
                                cout << "--- " << p.name << " 的未建成奇迹 [" << it->name << "] 被移除。" << endl;
                                it = p.wonders.erase(it);
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
    cb.coinsToBank = cost.coins;
    for (auto const& [res, needed] : cost.resources) {
        int produced = buyer.production[res];
        if (produced < needed) {
            int missing = needed - produced;
            int oppProd = opponent.production[res];
            int pricePerUnit = 2 + oppProd;
            if (buyer.tradeFixed.count(res) && buyer.tradeFixed.at(res)) {
                pricePerUnit = 1;
            }
            int tradeCost = missing * pricePerUnit;
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