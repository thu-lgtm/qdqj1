/**
* @file Enums.cpp
 * @brief 枚举辅助函数的具体实现
 */

#include "Enums.h"

/**
 * @brief 实现 getChainName 函数
 * 通过 switch-case 将枚举值转换为汉化的字符串
 */
std::string getChainName(ChainSymbol c) {
    switch(c) {
    case JUG: return "水壶"; case MASK: return "面具"; case TELESCOPE: return "望远镜";
    case SUN: return "太阳"; case DROP: return "水滴"; case PILLAR: return "柱子";
    case TARGET: return "靶子"; case HELMET: return "头盔"; case HORSESHOE: return "马蹄铁";
    case SWORD: return "剑"; case TOWER: return "塔楼"; case BOOK: return "书";
    case GEAR: return "齿轮"; case HARP: return "竖琴"; case LAMP: return "神灯";
    case BARREL: return "木桶";
    case CHAIN_GLOBE: return "天文仪链"; case CHAIN_MORTAR: return "研钵链";
    case CHAIN_WHEEL: return "轮子链"; case CHAIN_QUILL: return "羽毛笔链";
    default: return "";
    }
}

/**
 * @brief 实现 getTokenName 函数
 * 返回科技币名称及其效果简述，方便在控制台显示
 */
std::string getTokenName(ProgressToken t) {
    switch(t) {
    case P_AGRICULTURE: return "农业(金币+6,胜利点+4)";
    case P_ARCHITECTURE: return "建筑学(建造奇迹省2份材料)";
    case P_ECONOMY: return "经济学(对手交易费归你)";
    case P_LAW: return "法律(科技符号+1)";
    case P_MASONRY: return "砌体结构(建造蓝卡省2份材料)";
    case P_MATHEMATICS: return "数学(结算时每个发展标记得3个胜利点)";
    case P_PHILOSOPHY: return "哲学(胜利点+7)";
    case P_STRATEGY: return "战略(使用红卡多1个额外军事标记)";
    case P_THEOLOGY: return "神学(建造任意奇迹均可增加一回合)";
    case P_URBANISM: return "城市规划(通过连锁建造时获得4金币)";
    default: return "未知";
    }
}