#include "Game.h"
#include "CardDatabase.h"
#include <algorithm>
#include <chrono>
#include <vector>
#include <random>

/**
 * @brief 加载指定时代的卡牌库
 */
std::vector<Card> CardDatabase::loadCardsForAge(int age){
    std::vector<Card> deck;

    // ==================== 时代 I (23张) ====================
    if (age == 1) {
        // --- 原料 (棕色) 6张 ---
        deck.push_back(Card("伐木场", RAW_MATERIAL, {}, 0, 0).setProd({{WOOD, 1}}));
        deck.push_back(Card("采木营地", RAW_MATERIAL, {1, {}}, 0, 0).setProd({{WOOD, 1}}));
        deck.push_back(Card("黏土池", RAW_MATERIAL, {}, 0, 0).setProd({{CLAY, 1}}));
        deck.push_back(Card("黏土坑", RAW_MATERIAL, {1, {}}, 0, 0).setProd({{CLAY, 1}}));
        deck.push_back(Card("采石场", RAW_MATERIAL, {}, 0, 0).setProd({{STONE, 1}}));
        deck.push_back(Card("石坑", RAW_MATERIAL, {1, {}}, 0, 0).setProd({{STONE, 1}}));

        // --- 制品 (灰色) 2张 ---
        deck.push_back(Card("玻璃厂", MANUFACTURED, {1, {}}, 0, 0).setProd({{GLASS, 1}}));
        deck.push_back(Card("压纸机", MANUFACTURED, {1, {}}, 0, 0).setProd({{PAPYRUS, 1}}));

        // --- 军事 (红色) 4张 ---
        deck.push_back(Card("瞭望塔", MILITARY, {0, {}}, 0, 1)); // 免费
        deck.push_back(Card("马厩", MILITARY, {0, {{WOOD, 1}}}, 0, 1).setChain(HORSESHOE));
        deck.push_back(Card("驻军", MILITARY, {0, {{CLAY, 1}}}, 0, 1).setChain(SWORD));
        deck.push_back(Card("栅栏", MILITARY, {0, {{STONE, 1}}}, 0, 1).setChain(TOWER)); // 规则书中费用通常较小，此处设为1石

        // --- 科技 (绿色) 4张 ---
        deck.push_back(Card("工坊", SCIENTIFIC, {0, {{PAPYRUS, 1}}}, 1, 0, GLOBE).setChain(CHAIN_GLOBE)); // 1分
        deck.push_back(Card("药剂师", SCIENTIFIC, {0, {{GLASS, 1}}}, 1, 0, WHEEL).setChain(GEAR));
        deck.push_back(Card("缮写室", SCIENTIFIC, {2, {}}, 0, 0, QUILL).setChain(BOOK)); // 费用2金
        deck.push_back(Card("药师", SCIENTIFIC, {2, {}}, 0, 0, MORTAR).setChain(CHAIN_MORTAR)); // 费用2金

        // --- 商业 (黄色) 4张 ---
        deck.push_back(Card("石头储备", COMMERCIAL, {3, {}}, 0, 0).setTrade(STONE));
        deck.push_back(Card("粘土储备", COMMERCIAL, {3, {}}, 0, 0).setTrade(CLAY));
        deck.push_back(Card("木材储备", COMMERCIAL, {3, {}}, 0, 0).setTrade(WOOD));
        // 酒馆 (Tavern) - 产金币
        deck.push_back(Card("酒馆", COMMERCIAL, {0, {}}, 0, 0).setCoinProd(1)); // 每回合产1金(简化)或进场拿4金

        // --- 市政 (蓝色) 3张 ---
        deck.push_back(Card("剧院", CIVILIAN, {0, {}}, 3).setChain(MASK));
        deck.push_back(Card("祭坛", CIVILIAN, {0, {}}, 3).setChain(SUN));
        deck.push_back(Card("浴场", CIVILIAN, {0, {{STONE, 1}}}, 3).setChain(DROP));
    }
    // ==================== 时代 II (23张) ====================
    else if (age == 2) {
        // --- 原料 (棕色) 3张 ---
        deck.push_back(Card("锯木厂", RAW_MATERIAL, {2, {}}, 0, 0).setProd({{WOOD, 2}}));
        deck.push_back(Card("砖厂", RAW_MATERIAL, {2, {}}, 0, 0).setProd({{CLAY, 2}}));
        deck.push_back(Card("层状采石场", RAW_MATERIAL, {2, {}}, 0, 0).setProd({{STONE, 2}}));

        // --- 制品 (灰色) 2张 ---
        deck.push_back(Card("吹玻璃工", MANUFACTURED, {0, {{WOOD, 1}}}, 0, 0).setProd({{GLASS, 1}})); // 1木->1玻
        deck.push_back(Card("干燥室", MANUFACTURED, {0, {{STONE, 1}}}, 0, 0).setProd({{PAPYRUS, 1}})); // 1石->1纸

        // --- 军事 (红色) 4张 ---
        deck.push_back(Card("城墙", MILITARY, {0, {{STONE, 2}}}, 0, 2));
        deck.push_back(Card("靶场", MILITARY, {0, {{WOOD, 2}, {GLASS, 1}}}, 0, 2).setChain(TARGET));
        deck.push_back(Card("阅兵场", MILITARY, {0, {{CLAY, 2}, {PAPYRUS, 1}}}, 0, 2).setChain(HELMET, HORSESHOE));
        // 新增: 马场 (Horse Breeders)
        deck.push_back(Card("马场", MILITARY, {0, {{WOOD, 1}, {CLAY, 1}}}, 0, 1).setChain(NONE_CHAIN, HORSESHOE));
        // 修正: 补充缺失的军事卡（通常时代II有4张红卡，这里用兵营补位）
        deck.push_back(Card("兵营", MILITARY, {3, {}}, 0, 1).setChain(NONE_CHAIN, SWORD));

        // --- 商业 (黄色) 4张 ---
        deck.push_back(Card("广场", COMMERCIAL, {3, {{CLAY, 1}}}, 0, 0).setProd({{GLASS,1}})); // 产出任意制品(简化为特定或随机)
        deck.push_back(Card("商队旅馆", COMMERCIAL, {2, {{GLASS,1}, {PAPYRUS,1}}}, 0, 0).setProd({{WOOD,1}})); // 产出任意原料
        deck.push_back(Card("酿酒厂", COMMERCIAL, {0, {}}, 0, 0).setChain(BARREL)); // 产6金
        // 新增: 海关 (Customs House)
        deck.push_back(Card("海关", COMMERCIAL, {4, {}}, 0, 0).setTrade(GLASS).setTrade(PAPYRUS)); // 玻璃/纸张交易优惠

        // --- 市政 (蓝色) 5张 ---
        deck.push_back(Card("法庭", CIVILIAN, {0, {{WOOD, 2}, {GLASS, 1}}}, 5));
        deck.push_back(Card("雕像", CIVILIAN, {0, {{CLAY, 2}}}, 4).setChain(PILLAR, MASK)); // 连锁：剧院->雕像
        deck.push_back(Card("神庙", CIVILIAN, {0, {{WOOD, 1}, {PAPYRUS, 1}}}, 4).setChain(NONE_CHAIN, SUN)); // 连锁：祭坛->神庙
        deck.push_back(Card("水渠", CIVILIAN, {0, {{STONE, 3}}}, 5).setChain(NONE_CHAIN, DROP)); // 连锁：浴场->水渠
        deck.push_back(Card("讲坛", CIVILIAN, {0, {{STONE, 1}, {WOOD, 1}}}, 4));

        // --- 科技 (绿色) 4张 ---
        deck.push_back(Card("诊所", SCIENTIFIC, {0, {{CLAY, 2}, {GLASS, 1}}}, 2, 0, MORTAR).setChain(CHAIN_MORTAR, CHAIN_MORTAR)); // 连锁：药师->诊所
        deck.push_back(Card("实验室", SCIENTIFIC, {0, {{WOOD, 2}, {GLASS, 1}}}, 1, 0, GLOBE).setChain(CHAIN_GLOBE, CHAIN_GLOBE)); // 连锁：工坊->实验室
        deck.push_back(Card("图书馆", SCIENTIFIC, {0, {{STONE, 2}, {PAPYRUS, 1}}}, 2, 0, TABLET).setChain(BOOK, BOOK)); // 连锁：缮写室->图书馆
        deck.push_back(Card("学校", SCIENTIFIC, {0, {{WOOD, 1}, {PAPYRUS, 2}}}, 1, 0, WHEEL).setChain(HARP, CHAIN_WHEEL)); // 连锁：药剂师->学校
    }
    // ==================== 时代 III (23张) ====================
    else if (age == 3) {
        // 时代 III 没有原料(棕)和制品(灰)卡牌

        // --- 军事 (红色) 5张 ---
        deck.push_back(Card("兵工厂", MILITARY, {0, {{CLAY, 3}, {WOOD, 2}}}, 0, 3));
        deck.push_back(Card("军械库", MILITARY, {0, {{STONE, 3}, {GLASS, 1}}}, 0, 3).setChain(NONE_CHAIN, HELMET));
        deck.push_back(Card("防御工事", MILITARY, {0, {{STONE, 2}, {CLAY, 2}, {PAPYRUS, 1}}}, 0, 2).setChain(NONE_CHAIN, TOWER)); // 连锁：栅栏/墙->防御工事
        deck.push_back(Card("攻城工坊", MILITARY, {0, {{WOOD, 3}, {GLASS, 1}}}, 0, 2).setChain(NONE_CHAIN, TARGET)); // 连锁：靶场->攻城
        deck.push_back(Card("竞技场(红)", MILITARY, {0, {{STONE, 2}, {CLAY, 2}}}, 0, 2).setChain(NONE_CHAIN, BARREL)); // 连锁：酿酒厂->竞技场

        // --- 市政 (蓝色) 6张 ---
        deck.push_back(Card("法院", CIVILIAN, {0, {{CLAY, 2}, {PAPYRUS, 1}}}, 5)); // 可能是宫殿的别名，这里保留
        deck.push_back(Card("宫殿", CIVILIAN, {0, {{STONE, 1}, {CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 7));
        deck.push_back(Card("市政厅", CIVILIAN, {0, {{STONE, 3}, {WOOD, 2}}}, 6));
        deck.push_back(Card("方尖碑", CIVILIAN, {0, {{STONE, 2}, {GLASS, 1}}}, 5));
        // 新增缺失的蓝卡
        deck.push_back(Card("花园", CIVILIAN, {0, {{WOOD, 2}, {CLAY, 2}}}, 6).setChain(NONE_CHAIN, PILLAR)); // 连锁：雕像->花园
        deck.push_back(Card("万神殿", CIVILIAN, {0, {{CLAY, 1}, {WOOD, 1}, {PAPYRUS, 2}}}, 6).setChain(NONE_CHAIN, SUN)); // 连锁：神庙->万神殿
        deck.push_back(Card("参议院", CIVILIAN, {0, {{WOOD, 2}, {STONE, 1}, {PAPYRUS, 1}}}, 5).setChain(NONE_CHAIN, ROSTRUM)); // 连锁：讲坛->参议院

        // --- 科技 (绿色) 4张 ---
        deck.push_back(Card("学院", SCIENTIFIC, {0, {{STONE, 1}, {GLASS, 2}}}, 3, 0, SCALE)); // 符号需调整为日晷/法律
        deck.push_back(Card("书房", SCIENTIFIC, {0, {{WOOD, 1}, {PAPYRUS, 2}}}, 3, 0, SCALE)); // 连锁：学校->书房
        deck.push_back(Card("大学", SCIENTIFIC, {0, {{CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 2, 0, GLOBE).setChain(NONE_CHAIN, CHAIN_GLOBE)); // 连锁：实验室->大学
        deck.push_back(Card("天文台", SCIENTIFIC, {0, {{STONE, 1}, {PAPYRUS, 2}}}, 2, 0, WHEEL).setChain(NONE_CHAIN, GEAR)); // 连锁：诊所->天文台

        // --- 商业 (黄色) 3张 ---
        deck.push_back(Card("商会", COMMERCIAL, {0, {{PAPYRUS, 2}}}, 3, 0).setChain(NONE_CHAIN, MASK));
        deck.push_back(Card("港口", COMMERCIAL, {0, {{WOOD, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 3, 0).setChain(NONE_CHAIN, BARREL));
        // 灯塔 (Lighthouse) - 灯塔是黄卡，不是奇迹
        deck.push_back(Card("灯塔", COMMERCIAL, {0, {{CLAY, 2}, {GLASS, 1}}}, 3, 0).setChain(NONE_CHAIN, DROP)); // 连锁：水渠->灯塔或是给每张黄卡1金币
        deck.push_back(Card("竞技场(黄)", COMMERCIAL, {0, {{STONE, 1}, {WOOD, 1}}}, 3, 0).setChain(NONE_CHAIN, BARREL)); // 连锁：酿酒厂->竞技场(黄)

        // --- 行会 (紫色) 随机3张 ---
        std::vector<Card> guildPool;
        guildPool.push_back(Card("商人公会", GUILD, {0, {{WOOD, 1}, {CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}).setGuild(G_MERCHANT));
        guildPool.push_back(Card("船东公会", GUILD, {0, {{STONE, 1}, {GLASS, 1}, {PAPYRUS, 1}}}).setGuild(G_SHIPOWNER));
        guildPool.push_back(Card("建筑师公会", GUILD, {0, {{STONE, 2}, {CLAY, 1}, {WOOD, 1}}}).setGuild(G_BUILDER));
        guildPool.push_back(Card("行政官公会", GUILD, {0, {{WOOD, 2}, {CLAY, 1}, {PAPYRUS, 1}}}).setGuild(G_MAGISTRATE));
        guildPool.push_back(Card("科学家公会", GUILD, {0, {{WOOD, 2}, {STONE, 2}}}).setGuild(G_SCIENTIST));
        guildPool.push_back(Card("高利贷公会", GUILD, {0, {{STONE, 2}, {WOOD, 2}}}).setGuild(G_MONEYLENDER));
        guildPool.push_back(Card("策略家公会", GUILD, {0, {{CLAY, 2}, {STONE, 1}, {PAPYRUS, 1}}}).setGuild(G_TACTICIAN));

        unsigned seedG = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(guildPool.begin(), guildPool.end(), std::default_random_engine(seedG));

        for(int i=0; i<3; i++) {
            deck.push_back(guildPool[i]);
        }
    }
    return deck;
}

/**
 * @brief 加载所有奇迹
 */
std::vector<Wonder> CardDatabase::loadWonders() {
    std::vector<Wonder> wonders;

    // 1. 亚壁古道 (The Appian Way)
    // 3分, 3金, 额外回合, 对手扣3金
    Cost costAppian;
    costAppian.resources = {{STONE, 2}, {CLAY, 2}, {PAPYRUS, 1}};
    wonders.push_back(Wonder("亚壁古道", costAppian, 3, 0, 3, false, true, "对手失去3金币，获得额外回合"));

    // 2. 马克西姆斯竞技场 (Circus Maximus)
    // 3分, 1盾, 摧毁灰卡
    Cost costCircus;
    costCircus.resources = {{STONE, 2}, {WOOD, 2}, {GLASS, 1}};
    wonders.push_back(Wonder("馬克西姆斯競技場", costCircus, 3, 1, 0, false, false, "摧毁对手一张灰色卡牌"));

    // 3. 罗德岛太阳神铜像 (The Colossus)
    // 3分, 2盾
    Cost costColossus;
    costColossus.resources = {{CLAY, 3}, {GLASS, 1}};
    wonders.push_back(Wonder("罗德岛太阳神铜像", costColossus, 3, 2, 0, false, false, "获得2个军事盾牌"));

    // 4. 大图书馆 (The Great Library)
    // 4分, 随机科技币
    Cost costLibrary;
    costLibrary.resources = {{WOOD, 3}, {GLASS, 1}, {PAPYRUS, 1}};
    wonders.push_back(Wonder("大图书馆", costLibrary, 4, 0, 0, false, false, "随机获得一个未使用的科技进步标记"));

    // 5. 大灯塔 (The Great Lighthouse)
    // 4分 (当前Wonder结构体不支持资源产出，暂时只给分)
    Cost costLighthouse;
    costLighthouse.resources = {{WOOD, 1}, {STONE, 1}, {PAPYRUS, 2}};
    wonders.push_back(Wonder("大灯塔", costLighthouse, 4, 0, 0, false, false, "生产资源(暂未实现), 4分"));

    // 6. 空中花园 (The Hanging Gardens)
    // 3分, 6金, 额外回合
    Cost costGardens;
    costGardens.resources = {{WOOD, 2}, {PAPYRUS, 2}};
    wonders.push_back(Wonder("空中花园", costGardens, 3, 0, 6, false, true, "获得6金币，获得额外回合"));

    // 7. 摩索拉斯陵墓 (The Mausoleum)
    // 2分, 复活弃牌
    Cost costMausoleum;
    costMausoleum.resources = {{CLAY, 2}, {GLASS, 1}, {PAPYRUS, 2}};
    wonders.push_back(Wonder("哈利卡納斯的摩索拉斯陵墓", costMausoleum, 2, 0, 0, false, false, "从弃牌堆免费建造一张卡牌"));

    // 8. 比雷埃夫斯港 (The Piraeus)
    // 2分, 额外回合 (资源产出暂不支持)
    Cost costPiraeus;
    costPiraeus.resources = {{WOOD, 2}, {STONE, 1}, {CLAY, 1}};
    wonders.push_back(Wonder("比雷埃夫斯港", costPiraeus, 2, 0, 0, false, true, "生产资源(暂未实现), 额外回合"));

    // 9. 金字塔 (The Pyramids)
    // 9分
    Cost costPyramids;
    costPyramids.resources = {{STONE, 3}, {PAPYRUS, 1}};
    wonders.push_back(Wonder("金字塔", costPyramids, 9, 0, 0, false, false, "获得9点胜利分数"));

    // 10. 斯芬克斯 (The Sphinx)
    // 6分, 额外回合
    Cost costSphinx;
    costSphinx.resources = {{STONE, 1}, {CLAY, 1}, {GLASS, 2}};
    wonders.push_back(Wonder("斯芬克斯", costSphinx, 6, 0, 0, false, true, "获得6分，获得额外回合"));

    // 11. 宙斯神像 (Statue of Zeus)
    // 3分, 1盾, 摧毁棕卡
    Cost costZeus;
    costZeus.resources = {{WOOD, 1}, {STONE, 2}, {CLAY, 1}, {PAPYRUS, 1}};
    wonders.push_back(Wonder("奥林匹亞宙斯神像", costZeus, 3, 1, 0, false, false, "摧毁对手一张棕色卡牌"));

    // 12. 阿尔忒弥斯神庙 (The Temple of Artemis)
    // 0分, 12金, 额外回合
    Cost costArtemis;
    costArtemis.resources = {{WOOD, 1}, {STONE, 1}, {GLASS, 1}, {PAPYRUS, 1}};
    wonders.push_back(Wonder("阿尔忒弥斯神庙", costArtemis, 0, 0, 12, false, true, "获得12金币，获得额外回合"));

    return wonders;
}