#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <openssl/evp.h>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto item = MenuItemLabel::create(Label::createWithSystemFont("Item", "Helvetica", 30),
                                      CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    item->setPosition(origin + visibleSize / 2);

    // create menu, it's an autorelease object
    auto menu = Menu::create(item, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);
/*
    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
  */
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    unsigned char password[] = "PASSWORD";
    std::string str = "暗号化復号化テスト\naaaaabbbbbbあああああああんんんんんんん";
    
    // std::stringをunsigned char*に変換
    unsigned char* message = (unsigned char*)malloc(str.length() + 1);
    message[str.length()] = '\0';
    memcpy(message, str.c_str(), str.length());
    
    // 暗号化を行う
    // encrypt内に暗号化したバイトデータが入る
    int len = 0;
    std::vector<unsigned char> encrypt(str.length() + EVP_MAX_BLOCK_LENGTH);
    EVP_CIPHER_CTX context;
    EVP_EncryptInit(&context, EVP_aes_128_ecb(), password, nullptr);
    EVP_EncryptUpdate(&context, encrypt.data(), &len, message,
                      static_cast<int>(str.length() + EVP_MAX_BLOCK_LENGTH));
    EVP_EncryptFinal(&context, encrypt.data() + len, &len);
    EVP_CIPHER_CTX_cleanup(&context);
    
    // 復号化を行う
    // decrypt内に復号化した文字列が入る
    int _length = 0;
    std::vector<unsigned char> decrypt(encrypt.size() + EVP_MAX_BLOCK_LENGTH);
    EVP_DecryptInit(&context, EVP_aes_128_ecb(), password, nullptr);
    EVP_DecryptUpdate(&context, decrypt.data(), &_length, encrypt.data(),
                      static_cast<int>(encrypt.size() + EVP_MAX_BLOCK_LENGTH));
    EVP_DecryptFinal(&context, decrypt.data() + len, &len);
    _length += len;
    EVP_CIPHER_CTX_cleanup(&context);
    
    // 復号化したデータを文字列に変換
    auto _str = __String::createWithData(decrypt.data(), _length);
    CCLOG("%s", _str->getCString());
    
    // メモリ解放
    free(message);
    EVP_cleanup();
    /*
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif*/
}
