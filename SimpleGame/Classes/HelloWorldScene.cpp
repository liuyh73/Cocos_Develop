#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"
#include <cstring>
#include <string>
#pragma execution_character_set("utf-8")
USING_NS_CC;

HelloWorld::HelloWorld() {
	dtime = 180;
	score = 0;
	flag = 1;
	lastDirection = 'D';
	db = UserDefault::getInstance();
	if (!db->getBoolForKey("isExist")) {
		db->setBoolForKey("isExist", true);
		db->setIntegerForKey("score", score);
		//db->setIntegerForKey("dtime", dtime);
	}
	else {
		score = db->getIntegerForKey("score", score);
		//dtime = db->getIntegerForKey("dtime", dtime);
	}
}

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

void HelloWorld::updateCustom(float dt) 
{
	dtime--;
	log(dtime);
	time->setString(std::to_string(dtime));
	if (dtime < 10)
		time->setColor(Color3B(255,80,80));
	if (dtime == 0)
		unschedule(schedule_selector(HelloWorld::updateCustom));
	db->setIntegerForKey("dtime", dtime);
}

void HelloWorld::updateCustomMonsterMove(float dt) {
	Factory::getInstance()->moveMonster(player->getPosition(), 1);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	//定时器
	schedule(schedule_selector(HelloWorld::updateCustom), 1.0f, kRepeatForever, 0);

	//怪物移动定时器
	schedule(schedule_selector(HelloWorld::hitByMonster), 0.1f, kRepeatForever, 0);

	schedule(schedule_selector(HelloWorld::updateCustomMonsterMove), 1.0f, kRepeatForever, 0);
	TMXTiledMap *SimpleGameTMX = TMXTiledMap::create("SimpleGame.tmx");
	SimpleGameTMX->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	SimpleGameTMX->setAnchorPoint(Vec2(0.5, 0.5));
	SimpleGameTMX->setScale(Director::getInstance()->getContentScaleFactor());
	addChild(SimpleGameTMX, 0);

    time = Label::createWithTTF(std::to_string(dtime), "fonts/arial.ttf", 36);
	time->setColor(Color3B(255,255,255));
	time->setAnchorPoint(Vec2(0.5, 0.5));
	time->setPosition(Vec2(origin.x+visibleSize.width/2, origin.y+visibleSize.height-50));
	addChild(time, 5);

	auto scoreNameLabel=Label::createWithTTF("Score: ", "fonts/arial.ttf", 40);
	scoreLabel = Label::createWithTTF(std::to_string(score), "fonts/arial.ttf", 36);
	scoreNameLabel->setPosition(Vec2(origin.x + visibleSize.width - 150, origin.y + visibleSize.height - 50));
	scoreLabel->setPosition(Vec2(origin.x + visibleSize.width - 70, origin.y + visibleSize.height - 50));
	addChild(scoreLabel, 5);
	addChild(scoreNameLabel, 5);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (auto i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (auto i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	
	//按钮X、Y
	auto X = Label::createWithTTF("X","fonts/arial.ttf", 36);
	auto XButton = MenuItemLabel::create(X, [&](cocos2d::Ref* sender) {
		/*if (flag) {
			flag = 0;
			auto decrease = ProgressFromTo::create(0.4, pT->getPercentage(), pT->getPercentage() - 20);
			pT->runAction(decrease);
			auto deadAnimation = Animation::createWithSpriteFrames(dead);
			deadAnimation->setDelayPerUnit(0.12f);
			deadAnimation->setRestoreOriginalFrame(true);
			auto deadAction = Animate::create(deadAnimation);
			player->runAction(Sequence::create(deadAction, CallFunc::create([&]{
				flag = 1;
			}),NULL));
		}*/
	});
	XButton->setPosition(Vec2(origin.x+visibleSize.width-15, origin.y+50));
	
	auto Y = Label::createWithTTF("Y", "fonts/arial.ttf", 36);
	auto YButton = MenuItemLabel::create(Y, [&](cocos2d::Ref* sender) {
		if (flag) {
			flag = 0;
			auto attackAnimation = Animation::createWithSpriteFrames(attack);
			attackAnimation->setDelayPerUnit(0.12f);
			attackAnimation->setRestoreOriginalFrame(true);
			auto attackAction = Animate::create(attackAnimation);
			player->runAction(Sequence::create(attackAction, CallFunc::create([&]{
				flag = 1;
			}), NULL));

			Rect playerRect = player->getBoundingBox();
			Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(), playerRect.getMaxX() - playerRect.getMinX() + 80, playerRect.getMaxY() - playerRect.getMinY());
			auto collision = Factory::getInstance()->collider(attackRect);
			if (collision != NULL) {
				auto increase = ProgressFromTo::create(0.4, pT->getPercentage(), pT->getPercentage() + 20);
				pT->runAction(increase);
				score++;
				scoreLabel->setString(std::to_string(score));
				db->setIntegerForKey("score", score);
				Factory::getInstance()->removeMonster(collision);
				addMonster();
			}
		}
	});
	YButton->setPosition(Vec2(origin.x + visibleSize.width - 50, origin.y+15));
	
	//按钮W,S,A,D
	auto W = Label::createWithTTF("W", "fonts/arial.ttf", 36);
	auto WButton = MenuItemLabel::create(W, [&](cocos2d::Ref* sender) {
		auto runAnimation = Animation::createWithSpriteFrames(run);
		runAnimation->setDelayPerUnit(0.12f);
		runAnimation->setRestoreOriginalFrame(true);
		auto runAction = Animate::create(runAnimation);
		if (player->getPosition().y + 30 < visibleSize.height - player->getContentSize().height) {
			auto moveTo = MoveTo::create(0.5f, Vec2(player->getPosition().x, player->getPosition().y + 30));
			player->runAction(Spawn::create(runAction, moveTo, NULL));
		}
		else
			player->runAction(runAction);
	});
	WButton->setPosition(Vec2(origin.x + 55, origin.y + 50));

	auto S = Label::createWithTTF("S", "fonts/arial.ttf", 36);
	auto SButton = MenuItemLabel::create(S, [&](cocos2d::Ref* sender) {
		auto runAnimation = Animation::createWithSpriteFrames(run);
		runAnimation->setDelayPerUnit(0.12f);
		runAnimation->setRestoreOriginalFrame(true);
		auto runAction = Animate::create(runAnimation);
		if (player->getPosition().y - 30 > player->getContentSize().height) {
			auto moveTo = MoveTo::create(0.5f, Vec2(player->getPosition().x, player->getPosition().y - 30));
			player->runAction(Spawn::create(runAction, moveTo, NULL));
		}
		else
			player->runAction(runAction);
	});
	SButton->setPosition(Vec2(origin.x + 55, origin.y + 15));
	
	auto A = Label::createWithTTF("A", "fonts/arial.ttf", 36);
	auto AButton = MenuItemLabel::create(A, [&](cocos2d::Ref* sender) {
		auto runAnimation = Animation::createWithSpriteFrames(run);
		runAnimation->setDelayPerUnit(0.12f);
		runAnimation->setRestoreOriginalFrame(true);
		auto runAction = Animate::create(runAnimation);
		if (lastDirection != 'A') {
			player->setFlipX(true);
		}
		if (player->getPosition().x - 30 > player->getContentSize().width) {
			auto moveTo = MoveTo::create(0.5f, Vec2(player->getPosition().x - 30, player->getPosition().y));
			player->runAction(Spawn::create(runAction, moveTo, NULL));
		}
		else
			player->runAction(runAction);
		lastDirection = 'A';
	});
	AButton->setPosition(Vec2(origin.x + 15, origin.y + 15));

	auto D = Label::createWithTTF("D", "fonts/arial.ttf", 36);
	auto DButton = MenuItemLabel::create(D, [&](cocos2d::Ref* sender) {
		auto runAnimation = Animation::createWithSpriteFrames(run);
		runAnimation->setDelayPerUnit(0.12f);
		runAnimation->setRestoreOriginalFrame(true);
		auto runAction = Animate::create(runAnimation);
		if (lastDirection != 'D') {
			player->setFlipX(false);
		}

		if (player->getPosition().x + 30 < visibleSize.width - player->getContentSize().width) {
			auto moveTo = MoveTo::create(0.5f, Vec2(player->getPosition().x + 30, player->getPosition().y));
			player->runAction(Spawn::create(runAction, moveTo, NULL));
		}
		else
			player->runAction(runAction);
		lastDirection = 'D';
	});
	DButton->setPosition(Vec2(origin.x + 95, origin.y + 15));
	
	menu = Menu::create(XButton, YButton, WButton, SButton, AButton, DButton, NULL);
	menu->setPosition(Vec2::ZERO);
	addChild(menu, 4);
	//init 10 monsters
	for (int i = 0; i < 10; i++)
		addMonster();

    return true;
}

void HelloWorld::addMonster() {
	auto monster = Factory::getInstance()->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	monster->setPosition(x, y);
	addChild(monster, 3);
}

void HelloWorld::hitByMonster(float dt) {
	auto factory = Factory::getInstance();
	Sprite* collision = factory->collider(player->getBoundingBox());

	if (collision != NULL) {
		auto decrease = ProgressFromTo::create(0.4, pT->getPercentage(), pT->getPercentage() - 10);
		pT->runAction(Sequence::create(decrease, CallFunc::create([&] {
			if (pT->getPercentage() == 0 && flag) {
				flag = 0;
				auto deadAnimation = Animation::createWithSpriteFrames(dead);
				deadAnimation->setDelayPerUnit(0.12f);
				auto deadAction = Animate::create(deadAnimation);
				player->runAction(deadAction);
				menu->setEnabled(false);
				unschedule(schedule_selector(HelloWorld::updateCustomMonsterMove));
				time->setString("Game over!");
				unschedule(schedule_selector(HelloWorld::updateCustom));
			}
		}), NULL));
		factory->removeMonster(collision);
		addMonster();
	}
}
