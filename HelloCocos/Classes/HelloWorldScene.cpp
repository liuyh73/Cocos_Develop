#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
#include <string>

USING_NS_CC;

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

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }
	//std::cout<<"*********************"<<closeItem->getPosition().x<<" "<< closeItem->getPosition().y<<std::endl;
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);


    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "16340157"
    // create and initialize a label

    auto labelId = Label::createWithTTF("16340157", "fonts/Marker Felt.ttf", 24);
    if (labelId == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
		labelId->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - 50));
		labelId->setColor(Color3B(80,80,255));
		//labelId->getContentSize().height
        // add the label as a child to this layer
        this->addChild(labelId, 1);
    }

	// add a label shows "刘亚辉"
	// create and initialize a label
	CCDictionary *Name = CCDictionary::createWithContentsOfFile("Name.xml");
	CCString* myName = (CCString*)Name->objectForKey("Name");
	auto labelName = Label::createWithSystemFont(myName->getCString(), "华文中宋", 24);
	if (labelName == nullptr)
	{
		problemLoading("'华文中宋'");
	}
	else
	{
		// position the label on the center of the screen
		labelName->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - 50));
		labelName->setColor(Color3B(80, 80, 255));
		// add the label as a child to this layer
		this->addChild(labelName, 1);
	}
	labelName->setVisible(false);

	auto toggleItem = MenuItemFont::create("Toggle", CC_CALLBACK_1(HelloWorld::menuToggleCallback, this, labelId, labelName));
	if (toggleItem == nullptr)
	{
		problemLoading("'Toggle'");
	}
	else
	{
		float x = origin.x + visibleSize.width/2;
		float y = origin.y + visibleSize.height - 100;
		toggleItem->setPosition(Vec2(x, y));
	}
	// create menu, it's an autorelease object
	menu = Menu::create(toggleItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	// add "HelloWorld" splash screen"
    auto sprite = Sprite::create("Background.jpg");
    if (sprite == nullptr)
    {
        problemLoading("'Background.jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::menuToggleCallback(Ref* pSender, Label* &labelId, Label* &labelName)
{
	//Close the cocos2d-x game scene and quit the application
	if (labelId->isVisible()) {
		labelId->setVisible(false);
		labelName->setVisible(true);
	}
	else {
		labelId->setVisible(true);
		labelName->setVisible(false);
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

}
