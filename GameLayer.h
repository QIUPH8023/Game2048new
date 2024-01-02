#ifndef GAMELAYER_H
#define GAMELAYER_H

#include <vector>
#include "cocos2d.h"
#include "Number.h"

enum Dir { left, right, up, down };

struct Position { int i; int j; };

#define BOARD_SIZE 4

#define FOR(i, j) for (int i = 0; i < 4; i++) { for (int j = 0; j < 4; j++) {
#define END_FOR }}

class GameLayer : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void update(float dt);
    void removeZero(bool inverted, bool again);
    void getDataForColorRow(int arr[][BOARD_SIZE], int getNum);
    void merge(bool inverted);
    void reduction(int arr[][BOARD_SIZE], int reductionNum);
    void setTouchEnable(bool enable);
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void setKeyboardEnable(bool enable);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event*);
    void arrClear();
    void randomCreateNum();
    void exit(cocos2d::Ref* sender);
    GameLayer();
    ~GameLayer();

    CREATE_FUNC(GameLayer);

private:
    bool isFull();
    bool canMerge();
    void onStop();
    void updateScore(int s);
    const char* name() { return " Jack"; }
    void toggleRankingVisible();
    void createRanking();
    cocos2d::Vec2 touchStartPos;
    bool touchMoved;
    Vec2 touch_began_point;

    Number *number[BOARD_SIZE][BOARD_SIZE]{};
    int map[BOARD_SIZE][BOARD_SIZE];
    int temp[4];
    Dir dir;
    bool needUpdate;
    cocos2d::Vec2 firstCell;
    int score;
    Label* scoreLabel;
    Label* rankLabel;
    std::vector<Node*> rankingNodes;
    bool rankingShow;

};

#endif
