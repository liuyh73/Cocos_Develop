#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	//Scene* scene = Scene::create();
	//GameScene *gameScene = GameScene::create();
	//scene->addChild(gameScene);
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_game = Sprite::create("level-background-0.jpg");
	bg_game->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2));
	this->addChild(bg_game, 0);

	stoneLayer = Layer::create();
	stoneLayer->setAnchorPoint(Vec2::ZERO);
	stoneLayer->setPosition(Vec2());
	stone = Sprite::create("stone.png");
	stone->setAnchorPoint(Vec2(0.5, 0.5));
	stone->setPosition(Vec2(560,480));
	stoneLayer->addChild(stone);

	mouseLayer = Layer::create();
	mouseLayer->setAnchorPoint(Vec2::ZERO);
	mouseLayer->setPosition(Vec2(0,visibleSize.height/2));
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setAnchorPoint(Vec2(0.5,0.5));
	mouse->setPosition(Vec2(visibleSize.width/2, 0));
	mouseLayer->addChild(mouse);

	this->addChild(stoneLayer);
	this->addChild(mouseLayer);

	auto labelShoot = Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 40);
	labelShoot->setColor(Color3B(80, 80, 255));
	auto labelShootButton = MenuItemLabel::create(labelShoot, CC_CALLBACK_1(GameScene::ShootCallback, this));
	labelShootButton->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y+visibleSize.height -100));
	auto menu = Menu::create(labelShootButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	// ������ʱ�������
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {

	//location���������꣬��cheese��������������
	auto location = touch->getLocation();
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	cheese->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(cheese);

	//Ȼ��cheese������ת��Ϊmouse��������꣬Ȼ�����MoveBy����
	auto relativeLocation=mouse->convertToNodeSpaceAR(cheese->getPosition());
	auto moveBy = MoveBy::create(1, relativeLocation);
	mouse->runAction(moveBy);

	auto fadeOut = FadeOut::create(4);
	cheese->runAction(fadeOut);
	return true;
}
void GameScene::ShootCallback(cocos2d::Ref* pSender) {
	auto visibleSize = Director::getInstance()->getVisibleSize();

	//Create a new stone
	auto newStone = Sprite::create("stone.png");
	newStone->setPosition(Vec2(560, 480));
	newStone->setAnchorPoint(Vec2(0.5, 0.5));
	stoneLayer->addChild(newStone);

	//���Ƚ�mouse��positionת��Ϊ�����mouseLayer���������꣨����Sprite���ĸ�layer�ϣ���������Ǹ�layer��ת������
	//Ȼ�󽫸���������ת��Ϊ�����stoneLayer��������꣬���ǣ���������stoneλ��ͬһ������ϵ�£��Ϳ���ʹ��moveTo�������������
	auto relativeLocation = stoneLayer->convertToNodeSpaceAR(mouseLayer->convertToWorldSpaceAR(mouse->getPosition()));
	auto moveTo = MoveTo::create(1, relativeLocation);
	newStone->runAction(moveTo);
	auto fadeOut = FadeOut::create(4);
	newStone->runAction(fadeOut);
	
	// Add a diamod on the mouseLayer that the mouse left
	// diamond Animate
	auto diamond = Sprite::createWithSpriteFrameName("pulled-diamond-0.png");
	Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimate"));
	diamond->runAction(RepeatForever::create(diamondAnimate));
	diamond->setAnchorPoint(Vec2(0.5, 0.5));
	diamond->setPosition(mouse->getPosition());
	mouseLayer->addChild(diamond, 0);

	//���Ȼ�ȡһ��mouse�����ƶ����ĵط��� ͨ�����·���������꼴Ϊ�������꣬
	//Ȼ�����mouseLayer��һ������ת�����Ϳ��Խ���������mouse����ͬһ������ϵ�£�ͨ������MoveTo���������������
	auto targetWorldPosition = Vec2(visibleSize.width*random<float>(0, 1), visibleSize.height*random<float>(0, 0.6));
	relativeLocation = mouseLayer->convertToNodeSpaceAR(targetWorldPosition);
	moveTo = MoveTo::create(1, relativeLocation);
	mouse->runAction(moveTo);

	//���£�Ҳ����ֱ�ӽ���������ת��Ϊ�����mouse��������꣬Ȼ�����moveBy��������
	//relativeLocation = mouse->convertToNodeSpaceAR(targetWorldPosition);

	//auto moveBy = MoveBy::create(1, relativeLocation);
	//mouse->runAction(moveBy);
}