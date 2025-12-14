#ifndef CARDDATABASE_H
#define CARDDATABASE_H

#include "Structs.h"
#include <vector>

class CardDatabase {
public:
    // 静态方法：根据时代获取原始卡牌列表
    static std::vector<Card> loadCardsForAge(int age);
    
    // 静态方法：获取所有奇迹
    static std::vector<Wonder> loadWonders();
};

#endif