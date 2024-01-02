#include "Number.h"
#include <cstdio>

Number* Number::create(int number)
{
    auto* ret = new (std::nothrow)Number();
    if (!ret)
    {
        return nullptr;
    }

    if (!ret->init(number))
    {
        delete ret;
        return nullptr;
    }

    ret->autorelease();
    return ret;
}

bool Number::init(int number)
{
    char filename[40];
    snprintf(filename, sizeof(filename), "image/%d.png", number);
    return Sprite::initWithFile(filename);
}

void Number::setImage(int number)
{
    char filename[40];
    snprintf(filename, sizeof(filename), "image/%d.png", number);
    this->setTexture(filename);
}
