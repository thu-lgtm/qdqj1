#include "Game.h"
#include "CardDatabase.h"
#include <algorithm>
#include <chrono>
#include <vector>
#include <random>

std::vector<Card> CardDatabase::loadCardsForAge(int age){
    std::vector<Card> deck;
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
    else if (age == 3) {
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

        // --- 【改进】：随机公会卡池 ---
        std::vector<Card> guildPool;
        guildPool.push_back(Card("商人公会", GUILD, {0, {{WOOD, 1}, {CLAY, 1}, {GLASS, 1}, {PAPYRUS, 1}}}).setGuild(G_MERCHANT));
        guildPool.push_back(Card("船东公会", GUILD, {0, {{STONE, 1}, {GLASS, 1}, {PAPYRUS, 1}}}).setGuild(G_SHIPOWNER));
        guildPool.push_back(Card("建筑师公会", GUILD, {0, {{STONE, 2}, {CLAY, 1}, {WOOD, 1}}}).setGuild(G_BUILDER));
        guildPool.push_back(Card("行政官公会", GUILD, {0, {{WOOD, 2}, {CLAY, 1}, {PAPYRUS, 1}}}).setGuild(G_MAGISTRATE));
        guildPool.push_back(Card("科学家公会", GUILD, {0, {{WOOD, 2}, {STONE, 2}}}).setGuild(G_SCIENTIST));
        guildPool.push_back(Card("高利贷公会", GUILD, {0, {{STONE, 2}, {WOOD, 2}}}).setGuild(G_MONEYLENDER));
        guildPool.push_back(Card("策略家公会", GUILD, {0, {{CLAY, 2}, {STONE, 1}, {PAPYRUS, 1}}}).setGuild(G_TACTICIAN));

        // 洗牌公会卡
        unsigned seedG = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(guildPool.begin(), guildPool.end(),std::default_random_engine(seedG));

        // 只取前 3 张加入牌堆
        for(int i=0; i<3; i++) {
            deck.push_back(guildPool[i]);
        }

        // 补齐剩下的军事卡
        while(deck.size() < 23) deck.push_back(Card("老兵", MILITARY, {0, {{CLAY,1}}}, 0, 2));
    }
    return deck;
}

std::vector<Wonder> CardDatabase::loadWonders() {
    std::vector<Wonder> all;
    all.push_back(Wonder("亚壁古道", {0, {{STONE, 2}, {CLAY, 2}, {PAPYRUS, 1}}}, 3, 0, 3, false, true, "对手丢3金,再来一回合"));
    all.push_back(Wonder("大竞技场", {0, {{STONE, 2}, {WOOD, 1}, {GLASS, 1}}}, 3, 1, 0, false, false, "1盾,3分"));
    all.push_back(Wonder("罗德岛巨像", {0, {{CLAY, 3}, {GLASS, 1}}}, 3, 2, 0, false, false, "2盾,3分"));
    all.push_back(Wonder("大图书馆", {0, {{WOOD, 3}, {PAPYRUS, 1}, {GLASS, 1}}}, 4, 0, 0, false, false, "4分,随机科技币(自动)"));
    all.push_back(Wonder("斯芬克斯像", {0, {{STONE, 3}, {GLASS, 2}}}, 6, 0, 0, false, true, "6分,再来一回合"));
    all.push_back(Wonder("比雷埃夫斯港", {0, {{WOOD, 2}, {CLAY, 1}, {PAPYRUS, 1}}}, 2, 0, 0, false, true, "2分,再来一回合"));
    all.push_back(Wonder("金字塔", {0, {{STONE, 3}, {PAPYRUS, 1}}}, 9, 0, 0, false, false, "9分"));
    all.push_back(Wonder("阿尔忒弥斯神庙", {0, {{WOOD, 1}, {STONE, 1}, {GLASS, 1}, {PAPYRUS, 1}}}, 0, 0, 12, false, true, "12金,再来一回合"));
    return all;
}