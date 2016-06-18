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

namespace {
    const char encryptkey[] = "encryptKey";
}
std::vector<unsigned char> Encrypt(const unsigned char *encryptMessage, int length, unsigned char *key, const unsigned char *iv)
{
    // 暗号化を行う
    // encrypt内に暗号化したバイトデータが入る
    EVP_CIPHER_CTX context;
    int len = 0;
    
    // データ長と、暗号化後、肥大化するデータを保存する為、ライブラリ指定の余剰領域を併せて確保
    std::vector<unsigned char> encrypt(length + EVP_MAX_BLOCK_LENGTH);
    
    // context 初期化
    EVP_CIPHER_CTX_init(&context);
    
    // 暗号化タイプにより初期化
    EVP_EncryptInit_ex(&context, EVP_aes_128_cbc(), NULL, key, iv);
    
    
    // encryptMessageのデータを暗号化しencryptへ
    EVP_EncryptUpdate(&context, encrypt.data(), &len, encryptMessage,
                      static_cast<int>(length + EVP_MAX_BLOCK_LENGTH));
    
    // ブロック長に満たないデータを不足分を補って暗号化
    // 機能をオフにしていないとEVP_EncryptUpdateの段階でPaddingが実行済み
    //EVP_EncryptFinal(&context, encrypt.data() + len, &len);
    
    // メモリ解放
    EVP_CIPHER_CTX_cleanup(&context);
    EVP_cleanup();
    
    return encrypt;
}
std::vector<unsigned char> Decrypt(std::vector<unsigned char> encrypt, unsigned char *key, const unsigned char *iv)
{
    // 復号化を行う
    // decrypt内に復号化した文字列が入る
    EVP_CIPHER_CTX context;
    int _length = 0;
    std::vector<unsigned char> decrypt(encrypt.size() + EVP_MAX_BLOCK_LENGTH);
    
    // context 初期化
    EVP_CIPHER_CTX_init(&context);
    
    // contextの暗号化タイプにより初期化
    EVP_DecryptInit_ex(&context, EVP_aes_128_cbc(), NULL, key, iv);
    
    // encryptのデータを復号化しdecryptへ
    EVP_DecryptUpdate(&context, decrypt.data(), &_length, encrypt.data(),
                      static_cast<int>(encrypt.size() + EVP_MAX_BLOCK_LENGTH));
    
    // ブロック長に満たなかったブロックを復号化し補ったデータを除去
    int len = _length - (int)encrypt.size();
    EVP_DecryptFinal(&context, decrypt.data() + len, &len);
    
    
    
    // コンテキストの解放
    EVP_CIPHER_CTX_cleanup(&context);
    EVP_cleanup();
    
    return decrypt;
}
void HelloWorld::menuCloseCallback(Ref* pSender)
{
    const unsigned char iv[] = "abcdefghijklmnop";
    unsigned char key[] = "PASSWORD";
    const unsigned char test[] = "暗号化復号化テスト\naaaaabbbbbbあああああああんんんんんんん";
    
    // 暗号化
    // 配列を渡す際は、データ長がリセットされる為、データ長は同時に渡す
    std::vector<unsigned char> encrypt = Encrypt(test, sizeof(test), key, iv);
    
    // 確認出力
    auto _enstr = __String::createWithData(encrypt.data(), encrypt.size());
    CCLOG("%s", _enstr->getCString());
    
    // 復号化を行う
    std::vector<unsigned char> decrypt = Decrypt(encrypt, key, iv);
    
    // 確認出力
    auto _str = __String::createWithData(decrypt.data(), decrypt.size());
    CCLOG("%s", _str->getCString());
}