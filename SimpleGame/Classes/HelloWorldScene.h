#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
	HelloWorld();
	void updateCustom(float dt);
	void updateCustomMonsterMove(float dt);

    virtual bool init();
    // implement the "static create()" method manually
	void addMonster();
	void hitByMonster(float dt);
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* scoreLabel;
	cocos2d::Menu* menu;
	cocos2d::UserDefault* db;
	char lastDirection;
	int flag;
	int dtime;
	int score;
	cocos2d::ProgressTimer* pT;
};
