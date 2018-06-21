#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

enum Tag {SHIP, BOX, BALL, PLAYER};

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug 模式
  // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);

  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();

  auto edgeSp = Sprite::create();  //创建一个精灵
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);

  preloadMusic(); // 预加载音效

  addSprite();    // 添加背景和各种精灵
  addListener();  // 添加监听器 
  addPlayer();    // 添加球与板
  BrickGeneraetd();  // 生成砖块

  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;
  spHolded = false;
  spFactor = 0;
  return true;
}

// 关节连接，固定球与板子
// Todo
void HitBrick::setJoint() {
	//log("ball: %d %d\n",ball->getPosition().x, ball->getPosition().y);
	//log("ball-anchor: %d %d\n", ball->getAnchorPoint().x, ball->getAnchorPoint().y);
	//log("player: %d %d\n", player->getPosition().x, player->getPosition().y);
	//log("player-anchor: %d %d\n", player->getAnchorPoint().x, player->getAnchorPoint().y);

	joint1= PhysicsJointPin::construct(
		ball->getPhysicsBody(),player->getPhysicsBody(), ball->getPosition()
	);
	m_world->addJoint(joint1);
}

// 预加载音效
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);
  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 0);
  //PhysicsMaterial(密度, 恢复系数, 摩擦系数)
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 0));
  shipbody->setCategoryBitmask(0xFFFFFFFF);
  shipbody->setCollisionBitmask(0xFFFFFFFF);
  shipbody->setContactTestBitmask(0xFFFFFFFF);
  //shipbody->setTag(SHIP);
  shipbody->setDynamic(false);  // ??????岻?????????, ????????????б??
  ship->setPhysicsBody(shipbody);
  ship->setTag(SHIP);
  this->addChild(ship, 1);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);
}

// 添加监听器
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {
  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setAnchorPoint(Vec2(0.5,0.5));
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  // 设置板的刚体属性
  auto playerPhysicsBody = PhysicsBody::createBox(Size(player->getContentSize().width, player->getContentSize().height), PhysicsMaterial(100.0f, 1.0, 1.0f));
  playerPhysicsBody->setCategoryBitmask(0x1);
  playerPhysicsBody->setCollisionBitmask(0x1);
  playerPhysicsBody->setContactTestBitmask(0x1);
  playerPhysicsBody->setDynamic(false);
  player->setTag(PLAYER);
  player->setPhysicsBody(playerPhysicsBody);
  this->addChild(player, 2);
  //log("width:%d height:%d\n", player->getContentSize().width, player->getContentSize().height);

  ball = Sprite::create("ball.png");
  //ball->setAnchorPoint(Vec2(0.5, 0.5));
  ball->setPosition(Vec2(xpos, player->getPosition().y + ball->getContentSize().height*0.1f/2+ player->getContentSize().height*0.1f/2));
  ball->setScale(0.1f, 0.1f);
  // 设置球的刚体属性
  auto ballPhysicsBody = PhysicsBody::createCircle(ball->getContentSize().width/2.0, PhysicsMaterial(100.0f, 1.0, 1.0f));
  ballPhysicsBody->setCategoryBitmask(0x3);
  ballPhysicsBody->setCollisionBitmask(0x3);
  ballPhysicsBody->setContactTestBitmask(0x3);
  ballPhysicsBody->setGravityEnable(false);
  ballPhysicsBody->setRotationEnable(false);
  ball->setTag(BALL);
  ball->setPhysicsBody(ballPhysicsBody);
  addChild(ball, 3);

  meteorBall = ParticleMeteor::create();
  meteorBall->setPosition(ball->getPosition());
  meteorBall->setDuration(-1);
  addChild(meteorBall);
}

// 实现简单的蓄力效果
void HitBrick::update(float dt) {
	if (spHolded) {
		spFactor += 15;
	}

	if (player->getPosition().x >= visibleSize.width - player->getContentSize().width / 20 && currDir=="Right")
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));

	if(player->getPosition().x <= player->getContentSize().width / 20 && currDir=="Left")
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	
	meteorBall->setPosition(ball->getPosition());
}

void HitBrick::BrickGeneraetd() {

	for (int i = 0; i < 3; i++) {
		int cw = 0;
		while (cw <= visibleSize.width) {
			auto box = Sprite::create("box.png");
			//box->setAnchorPoint(Vec2(0.5, 0.5));
			// 为砖块设置刚体属性
			box->setPosition(cw+box->getContentSize().width/2, visibleSize.height-i*box->getContentSize().height-box->getContentSize().height/2);
			auto boxPhysicsBody = PhysicsBody::createBox(Size(box->getContentSize().width, box->getContentSize().height), PhysicsMaterial(50.0f, 1.0, 1.0f));
			boxPhysicsBody->setCategoryBitmask(0x2);
			boxPhysicsBody->setCollisionBitmask(0x2);
			boxPhysicsBody->setContactTestBitmask(0x2);
			boxPhysicsBody->setDynamic(false);
			box->setTag(BOX);
			box->setPhysicsBody(boxPhysicsBody);
			this->addChild(box, 2);
			cw += box->getContentSize().width;
		}
	}
}

// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

	switch (code) {
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			currDir = "Left";
			player->getPhysicsBody()->setVelocity(Vec2(-500, 0));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			currDir = "Right";
			player->getPhysicsBody()->setVelocity(Vec2(500, 0));
			break;

		case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // 开始蓄力
			spFactor = 0;
			spHolded = true;
			break;
		default:
		break;
	}
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_A:
  case cocos2d::EventKeyboard::KeyCode::KEY_D:
	  // 停止运动
    // Todo
	  player->getPhysicsBody()->setVelocity(Vec2(0, 0));
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // 蓄力结束，小球发射
	  if (onBall) {
		  m_world->removeAllJoints();
		  ball->getPhysicsBody()->setVelocity(Vec2(0, spFactor));
	  }
	  onBall = false;
	  spHolded = false;
	  break;
  default:
    break;
  }
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
  //auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	//log("%d %d\n", nodeA->getTag(), nodeB->getTag());
	if (nodeA->getTag() == BALL) {
		if (nodeB->getTag() == BOX)
			nodeB->removeFromParentAndCleanup(true);
		else if (nodeB->getTag() == SHIP)
			GameOver();
	}
	
	else if (nodeB->getTag() == BALL) {
		if (nodeA->getTag() == BOX)
			nodeA->removeFromParentAndCleanup(true);
		else if (nodeA->getTag() == SHIP)
			GameOver();
	}
	return true;
}

void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
