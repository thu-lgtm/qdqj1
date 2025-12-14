#include <iostream>
#include <memory>
#include "Game.h"
#include "Strategy.h"
#include "Extension.h"

using namespace std;

// 辅助函数：选择策略类型
std::unique_ptr<PlayerStrategy> chooseStrategy(string playerName) {
    int choice;
    cout << "为 " << playerName << " 选择控制者类型:" << endl;
    cout << "1. 人类玩家" << endl;
    cout << "2. 智能 AI (贪婪策略)" << endl;
    cout << "3. 随机 AI (简单测试)" << endl;
    cout << "请输入选项 (1-3): ";
    cin >> choice;

    // 清除输入缓冲，防止后续读取名字出错
    cin.clear();
    cin.ignore(10000, '\n');

    switch(choice) {
    case 1: return std::make_unique<HumanStrategy>();
    case 2: return std::make_unique<GreedyAIStrategy>();
    case 3: return std::make_unique<RandomAIStrategy>(); // 至少一种简单AI
    default: return std::make_unique<RandomAIStrategy>();
    }
}

int main() {
    cout << "========================================" << endl;
    cout << "    七大奇迹：对决 (7 Wonders Duel)     " << endl;
    cout << "       LO02 项目 - 秋季 2025           " << endl;
    cout << "========================================" << endl;

    // 1. 配置玩家
    string p1Name, p2Name;
    cout << "请输入玩家 1 名字: ";
    getline(cin, p1Name);
    auto s1 = chooseStrategy(p1Name);

    cout << "请输入玩家 2 名字: ";
    getline(cin, p2Name);
    auto s2 = chooseStrategy(p2Name);

    // 2. 配置扩展 (展示架构对扩展的支持)
    bool enableExpansion = false;
    cout << "是否启用扩展包 (功能预留)? (0:否, 1:是): ";
    cin >> enableExpansion;

    // 3. 初始化并运行游戏
    // 使用 std::move 将策略的所有权转移给 Game 对象
    Game game(p1Name, std::move(s1), p2Name, std::move(s2));

    // 动态添加扩展
    if (enableExpansion) {
        game.addExtension(std::make_unique<PantheonExtension>());
    }

    game.run();

    return 0;
}