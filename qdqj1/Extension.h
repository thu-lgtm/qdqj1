/**
* @file Extension.h
 * @brief 扩展包接口定义
 * 允许在不修改 Game 核心代码的情况下改变游戏规则。
 */
#ifndef EXTENSION_H
#define EXTENSION_H

#include "Structs.h"
#include "Player.h"

// 前向声明
class Game;

class Extension {
public:
    virtual ~Extension() = default;

    // 获取扩展名称
    virtual std::string getName() const = 0;

    // 钩子：当游戏初始化时调用
    virtual void onGameStart(Game& game) {}

    // 钩子：当计算卡牌费用时调用 (允许扩展修改费用)
    virtual void onCalculateCost(const Player& buyer, const Card& card, int& currentCost) {}

    // 钩子：当一个回合结束时调用
    virtual void onTurnEnd(Game& game) {}
};

// 示例：简单的“万神殿”扩展桩代码
class PantheonExtension : public Extension {
public:
    std::string getName() const override { return "Pantheon (Demo)"; }

    void onGameStart(Game& game) override {
        // 在这里可以初始化神灵卡牌等
        // std::cout << "[扩展已加载] 万神殿扩展已激活！神灵正在注视..." << std::endl;
    }
};

#endif