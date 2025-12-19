/**
 * @file Enums.h
 * @brief 存放项目中所有的枚举类型定义
 * 作用：作为全局“字典”，统一定义游戏中的各种常量标识，如资源类型、卡牌颜色等。
 */

#ifndef ENUMS_H
#define ENUMS_H

#include <string>

/**
 * @enum Resource
 * @brief 资源类型枚举
 * 用于表示游戏中的基础资源。
 */
enum Resource {
    WOOD,       // 木材 (棕色卡产出)
    CLAY,       // 粘土 (棕色卡产出)
    STONE,      // 石头 (棕色卡产出)
    GLASS,      // 玻璃 (灰色卡产出)
    PAPYRUS,    // 纸莎草 (灰色卡产出)
    NO_RES      // 无资源 (占位符)
};

/**
 * @enum CardType
 * @brief 卡牌类型枚举
 * 用于区分卡牌的颜色和功能分类。
 */
enum CardType {
    RAW_MATERIAL,   // 原料卡 (棕色) - 产出基础资源
    MANUFACTURED,   // 制品卡 (灰色) - 产出高级资源
    CIVILIAN,       // 市政卡 (蓝色) - 提供胜利点数
    SCIENTIFIC,     // 科技卡 (绿色) - 提供科技符号
    COMMERCIAL,     // 商业卡 (黄色) - 提供金币、贸易优惠
    MILITARY,       // 军事卡 (红色) - 提供盾牌，推进军事条
    GUILD           // 公会卡 (紫色) - 第三时代出现，提供特殊计分
};

/**
 * @enum ScienceSymbol
 * @brief 科技符号枚举
 * 用于绿色科技卡上的符号标识。收集6种不同符号可获胜，收集2个相同符号可获得科技币。
 */
enum ScienceSymbol {
    GLOBE,      // 天文仪
    TABLET,     // 石板
    MORTAR,     // 研钵
    SCALE,      // 天平
    WHEEL,      // 轮子
    QUILL,      // 羽毛笔
    NO_SYMBOL   // 无符号
};

/**
 * @enum ChainSymbol
 * @brief 连锁符号枚举
 * 用于实现“免费建造”机制。卡牌右上角的白色符号。
 * 如果玩家拥有某个符号（ChainProvide），就能免费建造需要该符号（ChainCost）的卡。
 */
enum ChainSymbol {
    NONE_CHAIN, // 无连锁
    // 基础连锁符号
    JUG, MASK, TELESCOPE, SUN, DROP, PILLAR, TARGET, HELMET, HORSESHOE, SWORD, TOWER,
    BOOK, GEAR, HARP, LAMP, BARREL,ROSTRUM,
    // 科技卡专属连锁符号 (为了逻辑统一添加)
    CHAIN_GLOBE, CHAIN_TABLET, CHAIN_MORTAR, CHAIN_SCALE, CHAIN_WHEEL, CHAIN_QUILL
};

/**
 * @enum GuildType
 * @brief 公会类型枚举
 * 用于区分第三时代紫色公会卡的特殊计分规则。
 */
enum GuildType {
    NO_GUILD,       // 非公会卡
    G_MERCHANT,     // 商人公会
    G_SHIPOWNER,    // 船东公会
    G_BUILDER,      // 建筑师公会
    G_MAGISTRATE,   // 行政官公会
    G_SCIENTIST,    // 科学家公会
    G_TACTICIAN,    // 策略家公会
    G_MONEYLENDER   // 高利贷公会
};

/**
 * @enum ProgressToken
 * @brief 科技进步指示物枚举
 * 玩家收集一对相同科技符号时获得的特殊奖励标记。
 */
enum ProgressToken {
    P_AGRICULTURE,  // 农业
    P_ARCHITECTURE, // 建筑学
    P_ECONOMY,      // 经济学
    P_LAW,          // 法律
    P_MASONRY,      // 砌体结构
    P_MATHEMATICS,  // 数学
    P_PHILOSOPHY,   // 哲学
    P_STRATEGY,     // 战略
    P_THEOLOGY,     // 神学
    P_URBANISM      // 城市规划
};

// --- 辅助函数声明 ---

/**
 * @brief 获取连锁符号的中文名称
 * @param c 连锁符号枚举值
 * @return 对应的中文名称字符串
 */
std::string getChainName(ChainSymbol c);

/**
 * @brief 获取科技币的中文名称及简短效果描述
 * @param t 科技币枚举值
 * @return 对应的中文名称及效果字符串
 */
std::string getTokenName(ProgressToken t);

#endif