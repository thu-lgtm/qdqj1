/**
 * @file Player.cpp
 * @brief 玩家类方法的具体实现
 * 包含了玩家状态查询、资源统计、科技符号统计等核心逻辑的实现代码。
 */

#include "Player.h"

/**
 * @brief 构造函数实现
 * 初始化玩家的基本状态。
 * - 初始资源产量全部设为 0。
 * - 初始贸易优惠状态全部设为 false（关闭）。
 * * @param n 玩家的姓名字符串
 */
Player::Player(std::string n) : name(n) {
    // 初始化所有资源类型的贸易优惠为关闭
    tradeFixed[WOOD] = false;
    tradeFixed[CLAY] = false;
    tradeFixed[STONE] = false;
    tradeFixed[GLASS] = false;
    tradeFixed[PAPYRUS] = false;

    // 初始化所有资源类型的产量为 0
    production[WOOD]=0;
    production[CLAY]=0;
    production[STONE]=0;
    production[GLASS]=0;
    production[PAPYRUS]=0;
}

/**
 * @brief 获取已建造的黄色商业卡数量
 * 遍历 builtCards 列表，统计类型为 COMMERCIAL 的卡牌。
 * 作用：主要用于计算弃牌时的收益。规则是：弃牌获得的金币 = 2 + 拥有的黄色卡牌数量。
 * * @return 黄色卡牌的数量
 */
int Player::getYellowCount() {
    int c = 0;
    for(auto& card : builtCards) {
        if(card.type == COMMERCIAL) {
            c++;
        }
    }
    return c;
}

/**
 * @brief 获取已建成的奇迹数量
 * 遍历 wonders 列表，统计 built 属性为 true 的奇迹。
 * 作用：主要用于某些公会卡（如建筑师公会）的终局计分。
 * * @return 已建成奇迹的数量
 */
int Player::getWonderCount() {
    int c = 0;
    for(auto& w : wonders) {
        if(w.built) {
            c++;
        }
    }
    return c;
}

/**
 * @brief 统计拥有的不同科技符号种类数
 * 遍历 scienceSymbols 映射表，统计拥有数量大于 0 且不是占位符(NO_SYMBOL)的符号种类。
 * * 特殊逻辑：
 * 如果玩家拥有“法律”(P_LAW) 科技进步指示物，则视为额外拥有一种虚拟的科技符号。
 * 这有助于更容易达成科技胜利（集齐 6 种不同符号）。
 * * @return 不同符号的种类数量（含法律加成）
 */
int Player::countScienceDistinct() {
    int c = 0;
    for(auto const& [sym, count] : scienceSymbols) {
        // 只有当该符号的数量大于 0，且不是无效符号时，才计入种类
        if(count > 0 && sym != NO_SYMBOL) {
            c++;
        }
    }

    // 检查是否有“法律”科技币
    if(hasToken(P_LAW)) {
        c++; // 如果有，种类数 +1
    }

    return c;
}

/**
 * @brief 检查是否拥有特定的科技币
 * 遍历 tokens 向量，查找是否存在指定的科技币类型。
 * * @param t 要检查的目标科技币类型
 * @return true 如果玩家拥有该科技币
 * @return false 如果玩家没有该科技币
 */
bool Player::hasToken(ProgressToken t) {
    for(auto token : tokens) {
        if(token == t) {
            return true;
        }
    }
    return false;
}