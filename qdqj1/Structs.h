/**
 * @file Structs.h
 * @brief 核心数据结构定义
 * 定义了游戏中最基本的“物体”：费用、卡牌、奇迹、版图插槽。
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include "Enums.h"
#include <map>
#include <string>
#include <vector>

/**
 * @struct Cost
 * @brief 费用结构体
 * 描述建造一个物品需要支付的代价。
 */
struct Cost {
    int coins = 0;                      // 需要支付的金币数量
    std::map<Resource, int> resources;  // 需要支付的资源清单 (资源类型 -> 数量)

    // --- 构造函数 ---
    Cost() : coins(0) {}
    Cost(int c, std::map<Resource, int> r = {}) : coins(c), resources(r) {}

    /**
     * @brief 将费用转换为字符串描述
     * 例如："2金 1木"
     */
    std::string toString() const {
        if (coins == 0 && resources.empty()) return "免费";
        std::string s = "";
        if (coins > 0) s += std::to_string(coins) + "金 ";
        for (auto const& [res, count] : resources) {
            if (count > 0) {
                std::string rName;
                switch(res) {
                    case WOOD: rName="木"; break; case CLAY: rName="土"; break;
                    case STONE: rName="石"; break; case GLASS: rName="玻"; break;
                    case PAPYRUS: rName="纸"; break; default: break;
                }
                s += std::to_string(count) + rName + " ";
            }
        }
        return s;
    }
};

/**
 * @struct Card
 * @brief 卡牌结构体
 * 描述一张卡牌的所有属性。
 */
struct Card {
    std::string name;           // 卡牌名称
    CardType type;              // 卡牌类型 (红/蓝/绿等)
    Cost cost;                  // 建造费用
    int points = 0;             // 提供的胜利点数 (VP)
    int shields = 0;            // 提供的军事盾牌数
    ScienceSymbol science = NO_SYMBOL; // 提供的科技符号
    std::map<Resource, int> production; // 提供的资源产量
    int coinProduction = 0;     // 建造时一次性给予的金币

    // 连锁机制相关
    ChainSymbol chainProvide = NONE_CHAIN; // 该卡牌提供的连锁符号 (供未来免费建造使用)
    ChainSymbol chainCost = NONE_CHAIN;    // 该卡牌需要的连锁符号 (如果玩家有此符号，可免费建造)

    Resource tradeDiscountRes = NO_RES; // (仅黄色卡) 提供的贸易优惠资源类型
    GuildType guildType = NO_GUILD;     // (仅紫色卡) 公会类型

    // 构造函数
    Card() {}
    Card(std::string n, CardType t, Cost c, int p=0, int s=0, ScienceSymbol sci=NO_SYMBOL)
         : name(n), type(t), cost(c), points(p), shields(s), science(sci) {}

    // --- 链式设置方法 (Builder Pattern) ---
    Card& setProd(std::map<Resource, int> prod) { production = prod; return *this; }
    Card& setChain(ChainSymbol provide, ChainSymbol costSym = NONE_CHAIN) { chainProvide = provide; chainCost = costSym; return *this; }
    Card& setTrade(Resource res) { tradeDiscountRes = res; return *this; }
    Card& setCoinProd(int c) { coinProduction = c; return *this; }
    Card& setGuild(GuildType g) { guildType = g; return *this; }

    std::string getEffect() const {
        std::string s = "";
        if (points > 0) s += std::to_string(points) + "分 ";
        if (shields > 0) s += std::to_string(shields) + "盾 ";
        if (science != NO_SYMBOL) s += "科技 ";
        for (auto const& [res, count] : production) {
             std::string rName = (res==WOOD?"木":res==CLAY?"土":res==STONE?"石":res==GLASS?"玻":"纸");
             s += "+" + std::to_string(count) + rName + " ";
        }
        if (chainProvide != NONE_CHAIN) s += "[" + getChainName(chainProvide) + "] ";
        if (chainCost != NONE_CHAIN) s += "连锁:(" + getChainName(chainCost) + ") ";
        if (tradeDiscountRes != NO_RES) s += "交易优惠 ";
        if (guildType != NO_GUILD) s += "公会计分 ";
        return s;
    }

    std::string getTypeColor() const {
        switch(type) {
            case MILITARY: return "红";
            case SCIENTIFIC: return "绿";
            case CIVILIAN: return "蓝";
            case COMMERCIAL: return "黄";
            case GUILD: return "紫";
            case RAW_MATERIAL: return "棕";
            case MANUFACTURED: return "灰";
            default: return "白";
        }
    }
};

/**
 * @struct Wonder
 * @brief 奇迹结构体
 */
struct Wonder {
    std::string name;   // 奇迹名称
    Cost cost;          // 建造费用
    int points = 0;     // 胜利点数
    int shields = 0;    // 军事盾牌
    int coins = 0;      // 获得的金币
    bool built = false; // 状态：是否已建造
    bool extraTurn = false; // 是否提供额外回合
    std::string desc;   // 效果描述文本

    Wonder() {}
    Wonder(std::string n, Cost c, int p, int s, int coin, bool b, bool extra, std::string d)
        : name(n), cost(c), points(p), shields(s), coins(coin), built(b), extraTurn(extra), desc(d) {}
};

/**
 * @struct BoardSlot
 * @brief 版图插槽结构体
 */
struct BoardSlot {
    int id;             // 唯一编号
    Card card;          // 该位置存放的卡牌对象
    bool faceUp;        // 是否正面朝上
    bool taken = false; // 是否已被拿走
    std::vector<int> coveredBy; // 覆盖列表
    int row;
    int col;
};

struct CostBreakdown {
    int totalCost = 0;
    int coinsToBank = 0;     // 基础卡费 + 给银行的资源费
    int coinsToOpponent = 0; // 给对手的资源贸易费（受经济学标记影响）
};

#endif