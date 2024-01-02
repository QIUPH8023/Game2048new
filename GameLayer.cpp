#include "GameLayer.h"
#include "ui/CocosGUI.h"
#include <cstdlib>
#include <ctime>
#include "AudioEngine.h"
#include "audio/include/AudioEngine.h"

using namespace cocos2d;

GameLayer::GameLayer() : dir(up) {}

GameLayer::~GameLayer() {}

Scene* GameLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = GameLayer::create();
    scene->addChild(layer);
    return scene;
}

bool GameLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    AudioEngine::play2d("background.mp3",true);

    srand((int)time(0));

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto nameLabel = Label::createWithTTF(name(), "fonts/arial.ttf", 20);
    nameLabel->setPosition(origin + nameLabel->getContentSize() / 2);
    this->addChild(nameLabel, 1);

    createRanking();

    // 退出按钮放在右上角
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(GameLayer::exit, this)
    );
    closeItem->setScale(0.5);
    auto csize = closeItem->getContentSize() / 2;
    closeItem->setPosition(origin + visibleSize - csize);

    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    rankLabel = Label::createWithTTF("Ranking", "fonts/arial.ttf", 20);
    csize = rankLabel->getContentSize() / 2;
    rankLabel->setPosition(origin + Vec2(visibleSize.width, 0) + Vec2(-csize.width, csize.height));
    this->addChild(rankLabel, 2);

    // 屏幕中心坐标
    float x0 = visibleSize.width / 2 + origin.x;
    float y0 = visibleSize.height / 2 + origin.y;

    Sprite* bg = Sprite::create("image/map.png");
    bg->setScale(0.5);
    bg->setPosition(Vec2(x0,  y0));
    this->addChild(bg, -1);

    // 第一个方块的中心坐标
    float xf = x0 - 28.0 - 56.5;
    float yf = y0 + 28.0 + 56.5;

    FOR(i, j)
        auto num = Number::create(0);
        num->setScale(0.5);
        num->setPosition(Vec2(xf + j * 56.5, yf - i * 56.5));
        this->addChild(num, 1);

        map[i][j] = 0;
        number[i][j] = num;
    END_FOR

    score = 0;
    scoreLabel = Label::createWithTTF("0", "fonts/arial.ttf", 20);
    this->addChild(scoreLabel, 1);
    updateScore(0);

    randomCreateNum();
    randomCreateNum();
    needUpdate = true;

    setTouchEnable(true);
    scheduleUpdate();

    //开启键盘监听
    setKeyboardEnable(true);

    return true;
}

void GameLayer::randomCreateNum()
{
    int x, y;

    do {
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
    } while (map[x][y] != 0);

    map[x][y] = (rand() % 2) == 0 ? 2 : 4;
}

void GameLayer::update(float dt)
{
    if (!needUpdate)
    {
        return;
    }

    bool inverted = (dir == left || dir == up);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        getDataForColorRow(map, i);
        merge(inverted);
        reduction(map, i);
        arrClear();
    }

    if (isFull())
    {
        needUpdate = false;
        return;
    }

    randomCreateNum();

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            number[i][j]->setImage(map[i][j]);
        }
    }

    if (isFull() && !canMerge())
    {
        onStop();
        needUpdate = false;
        return;
    }

    needUpdate = false;
}

void GameLayer::getDataForColorRow(int arr[][BOARD_SIZE], int getNum)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        switch (dir)
        {
        case left:
            temp[i] = arr[getNum][i];
            break;
        case right:
            temp[i] = arr[getNum][BOARD_SIZE - i - 1];
            break;
        case down:
            temp[i] = arr[BOARD_SIZE - i - 1][getNum];
            break;
        case up:
            temp[i] = arr[i][getNum];
            break;
        }
    }
}

void GameLayer::merge(bool inverted)
{
    removeZero(inverted, false);

    for (int i = 0; i < BOARD_SIZE - 1; i++)
    {
        if (temp[i] == temp[i + 1])
        {
            temp[i] += temp[i + 1];
            temp[i + 1] = 0;
            updateScore(temp[i]);
        }
    }

    removeZero(inverted, true);
}

void GameLayer::removeZero(bool inverted, bool again)
{
    int arr[BOARD_SIZE]{0 };
    int j = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (temp[i] != 0)
        {
            arr[j++] = temp[i];
            temp[i] = 0;
        }
    }

    for (int i = 0; i < j; i++)
    {
        if (inverted)
        {
            temp[i] = arr[i];
        }
        else
        {
            if (!again)
            {
                temp[i] = arr[i];
            }
            else
            {
                temp[BOARD_SIZE - i - 1] = arr[i];
            }
        }
    }
}

void GameLayer::reduction(int arr[][BOARD_SIZE], int reductionNum)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        switch(dir)
        {
        case left:
        case right:
            arr[reductionNum][i] = temp[i];
            break;
        case down:
        case up:
            arr[i][reductionNum] = temp[i];
            break;
        }
    }
}

void GameLayer::setTouchEnable(bool enable)
{
    if (!enable)
    {
        _eventDispatcher->removeEventListenersForTarget(this);
        return;
    }

    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    //listener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool GameLayer::onTouchBegan(Touch* touch, Event* event) {
    auto location = touch->getLocation();
    touch_began_point = location;
    auto rect = rankLabel->getBoundingBox();
    if (rect.containsPoint(location)) {
        toggleRankingVisible();
        touchMoved = false; // 当新触摸开始时重置标志
        return false;
    }
    touchMoved = false; // 当新触摸开始时重置标志
    return true;
}

void GameLayer::onTouchMoved(Touch* touch, Event* event)
{
    Vec2 end = touch->getLocation();

    float deltaX = end.x - touchStartPos.x;
    float deltaY = end.y - touchStartPos.y;

    // 减小触摸点击灵敏度的阈值
    float threshold = 10.0;

    if (std::abs(deltaX) > std::abs(deltaY) && std::abs(deltaX) > threshold) {
        // 水平滑动
        if (deltaX > 0) {
            // 向右滑动
            dir = right;
        }
        else {
            // 向左滑动
            dir = left;
        }
    }
    else if (std::abs(deltaY) > threshold) {
        // 垂直滑动
        if (deltaY > 0) {
            // 向上滑动
            dir = up;
        }
        else {
            // 向下滑动
            dir = down;
        }
    }

    needUpdate = true;
}

void GameLayer::onTouchEnded(Touch* touch, Event* event)
{
    Vec2 end = touch->getLocation();

    float deltaX = end.x - touch_began_point.x;
    float deltaY = end.y - touch_began_point.y;

    // 减小触摸点击灵敏度的阈值
    float threshold = 1.0;

    if (std::abs(deltaX) > std::abs(deltaY) && std::abs(deltaX) > threshold) {
        // 水平滑动
        if (deltaX > 0) {
            // 向右滑动
            dir = right;
        }
        else {
            // 向左滑动
            dir = left;
        }
    }
    else if (std::abs(deltaY) > threshold) {
        // 垂直滑动
        if (deltaY > 0) {
            // 向上滑动
            dir = up;
        }
        else {
            // 向下滑动
            dir = down;
        }
    }

    needUpdate = true;
}

//是否开启键盘监听
void GameLayer::setKeyboardEnable(bool enable) 
{
    if (enable) {//开启
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed = CC_CALLBACK_2(GameLayer::onKeyPressed, this);
        //事件分发
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
    else {//关闭
        _eventDispatcher->removeEventListenersForTarget(this);
    }
}

// 键盘监听
void GameLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event*) 
{
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
    case EventKeyboard::KeyCode::KEY_W:
        dir = up;
        needUpdate = true;
        break;

    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case EventKeyboard::KeyCode::KEY_S:
        dir = down;
        needUpdate = true;
        break;

    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case EventKeyboard::KeyCode::KEY_A:
        dir = left;
        needUpdate = true;
        break;

    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case EventKeyboard::KeyCode::KEY_D:
        dir = right;
        needUpdate = true;
        break;

    default:
        needUpdate = false;
    }
}

void GameLayer::arrClear()
{
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        temp[i] = 0;
    }
}

void GameLayer::exit(Ref*)
{
    Director::getInstance()->end();
}

bool GameLayer::isFull() {
    FOR(i, j)
        if (map[i][j] == 0)
        {
            return false;
        }
    END_FOR

    return true;
}

bool GameLayer::canMerge() {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (map[i][j] == map[i][j + 1] || map[j][i] == map[j + 1][i])
            {
                return true;
            }
        }

    return false;
}

void GameLayer::onStop()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto gameoverLabel = Label::createWithTTF("GAMEOVER", "fonts/arial.ttf", 20);
    gameoverLabel->setPosition(origin + visibleSize / 2);
    this->addChild(gameoverLabel, 10);
}

void GameLayer::updateScore(int s) {
    score += s;

    char sstr[20];
    snprintf(sstr, sizeof(sstr),"Score: %d", score);
    scoreLabel->setString(sstr);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto csize = scoreLabel->getContentSize() / 2;
    auto arrow = Vec2(csize.width, -csize.height - 5);
    scoreLabel->setPosition(origin + Vec2(0, visibleSize.height) + arrow);
}

void GameLayer::createRanking()
{

    DrawNode* drawNode = DrawNode::create();

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    float x0 = visibleSize.width / 2 + origin.x;
    float y0 = visibleSize.height / 2 + origin.y;

    Vec2 vertices[] = {
        Vec2(x0 - 120.0, y0 - 120.0),
        Vec2(x0 + 120.0, y0 - 120.0),
        Vec2(x0 + 120.0, y0 + 120.0),
        Vec2(x0 - 120.0, y0 + 120.0)
    };

    Color4F color(0, 0, 0, 1);

    drawNode->drawSolidPoly(vertices, 4, color);

    this->addChild(drawNode, 10);

    auto label1 = Label::createWithTTF("Jack: 888", "fonts/arial.ttf", 20);
    auto label2 = Label::createWithTTF("Mary: 644", "fonts/arial.ttf", 20);
    auto label3 = Label::createWithTTF("Lucy: 368", "fonts/arial.ttf", 20);
    auto label4 = Label::createWithTTF("Pano: 228", "fonts/arial.ttf", 20);

    label1->setPosition(Vec2(x0, y0 + 45));
    label2->setPosition(Vec2(x0, y0 + 15));
    label3->setPosition(Vec2(x0, y0 - 15));
    label4->setPosition(Vec2(x0, y0 - 45));

    this->addChild(label1, 11);
    this->addChild(label2, 11);
    this->addChild(label3, 11);
    this->addChild(label4, 11);

    rankingNodes.push_back(drawNode);
    rankingNodes.push_back(label1);
    rankingNodes.push_back(label2);
    rankingNodes.push_back(label3);
    rankingNodes.push_back(label4);

    rankingShow = true;
    toggleRankingVisible();
}

void GameLayer::toggleRankingVisible()
{
    rankingShow = !rankingShow;
    for (auto node : rankingNodes)
    {
        node->setVisible(rankingShow);
    }
}

