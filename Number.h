#ifndef NUMBER_H
#define NUMBER_H

#include "cocos2d.h"

using namespace cocos2d;

class Number : public Sprite
{
public:
    static Number* create(int number);
    bool init(int number);
    void setImage(int number);
};

#endif
