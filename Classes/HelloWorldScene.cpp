#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "tinyxml2/tinyxml2.h"
#include <openssl/evp.h>

using namespace std;
using namespace tinyxml2;
USING_NS_CC;

namespace {
    const unsigned char encryptkey[] = "encryptKey";
    const unsigned char iv[] = "abcdefghijklmnop";
    const char mojiretsu[] = "mojiretsu";
    const char seisuu[] = "seisuu";
    const char shousuu[] = "shousuu";
}

// to_stringはAndroid未対応の為、コンパイルを通す為,定義
namespace std {
    template< typename T > std::string to_string(T num) {
        std::ostringstream stm;
        stm << num;
        return stm.str();
    }
}

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
std::vector<unsigned char> Encrypt(const unsigned char *encryptMessage,
                                   int length,
                                   const unsigned char *key,
                                   const unsigned char *iv)
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
std::vector<unsigned char> Decrypt(std::vector<unsigned char> encrypt,
                                   const unsigned char *key,
                                   const unsigned char *iv)
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
    ValueMap valueMap;
    valueMap[mojiretsu] = "test文字列test文字列";
    valueMap[seisuu] = 11111;
    valueMap[shousuu] = 3.14f;
    
    printValueMap(valueMap);
    
    char* str = createCharStringFromValueMap(valueMap);

    // 暗号化
    // 配列を渡す際は、データ長がリセットされる為、データ長は同時に渡す
    std::vector<unsigned char> encrypt = Encrypt((unsigned char*)str, (int)strlen(str), encryptkey, iv);
    
    free(str);
    
    // 確認出力
    auto _enstr = __String::createWithData(encrypt.data(), encrypt.size());
    CCLOG("start:\n%s", _enstr->getCString());
    
    // 復号化を行う
    std::vector<unsigned char> decrypt = Decrypt(encrypt, encryptkey, iv);
    
    // 確認出力
    auto _str = __String::createWithData(decrypt.data(), decrypt.size());
    CCLOG("%s", _str->getCString());
}
void printValueMap(const cocos2d::ValueMap& valueMap)
{
    std::string str;
    for (auto it = valueMap.begin();
         it != valueMap.end();
         it++)
    {
        str += it->first + " : ";
        Value value = it->second;
        switch (value.getType()) {
            case cocos2d::Value::Type::STRING:
                str += value.asString();
                break;
            case cocos2d::Value::Type::INTEGER:
                str += std::to_string(value.asInt());
                break;
            case cocos2d::Value::Type::FLOAT:
            case cocos2d::Value::Type::DOUBLE:
                str += std::to_string(value.asFloat());
                break;
            default:
                break;
        }
        str += "\n";
    }
    CCLOG("%s",str.c_str());
}

static XMLElement* _generateElementForObject(const Value& value, XMLDocument *doc);
static XMLElement* _generateElementForDict(const ValueMap& dict, XMLDocument *doc);
static XMLElement* _generateElementForArray(const ValueVector& array, XMLDocument *pDoc);

char* createCharStringFromValueMap(const ValueMap& valueMap)
{
    XMLDocument *doc = new (nothrow)XMLDocument();
    if (nullptr == doc) {
        return nullptr;
    }
    
    XMLDeclaration *declaration = doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    if (nullptr == declaration)
    {
        delete doc;
        return nullptr;
    }
    
    doc->LinkEndChild(declaration);
    XMLElement *docType = doc->NewElement("!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
    doc->LinkEndChild(docType);
    
    XMLElement *rootEle = doc->NewElement("plist");
    rootEle->SetAttribute("version", "1.0");
    if (nullptr == rootEle)
    {
        delete doc;
        return nullptr;
    }
    doc->LinkEndChild(rootEle);
    
    XMLElement *innerDict = _generateElementForDict(valueMap, doc);
    if (nullptr == innerDict)
    {
        delete doc;
        return nullptr;
    }
    rootEle->LinkEndChild(innerDict);
    
    XMLPrinter streamer;
    doc->Print( &streamer );
    
    char* ret = (char*)malloc(streamer.CStrSize());
    strcpy(ret, streamer.CStr());
    return ret;
}

/*
 * _generate XMLElement for Object through a XMLDocument
 */
static XMLElement* _generateElementForObject(const Value& value, XMLDocument *doc)
{
    // object is String
    if (value.getType() == Value::Type::STRING)
    {
        XMLElement* node = doc->NewElement("string");
        XMLText* content = doc->NewText(value.asString().c_str());
        node->LinkEndChild(content);
        return node;
    }
    
    // object is integer
    if (value.getType() == Value::Type::INTEGER)
    {
        XMLElement* node = doc->NewElement("integer");
        XMLText* content = doc->NewText(value.asString().c_str());
        node->LinkEndChild(content);
        return node;
    }
    
    // object is real
    if (value.getType() == Value::Type::FLOAT || value.getType() == Value::Type::DOUBLE)
    {
        XMLElement* node = doc->NewElement("real");
        XMLText* content = doc->NewText(value.asString().c_str());
        node->LinkEndChild(content);
        return node;
    }
    
    //object is bool
    if (value.getType() == Value::Type::BOOLEAN) {
        XMLElement* node = doc->NewElement(value.asString().c_str());
        return node;
    }
    
    // object is Array
    if (value.getType() == Value::Type::VECTOR)
        return _generateElementForArray(value.asValueVector(), doc);
    
    // object is Dictionary
    if (value.getType() == Value::Type::MAP)
        return _generateElementForDict(value.asValueMap(), doc);
    
    CCLOG("This type cannot appear in property list");
    return nullptr;
}

/*
 * _generate XMLElement for Dictionary through a XMLDocument
 */
static XMLElement* _generateElementForDict(const ValueMap& dict, XMLDocument *doc)
{
    XMLElement* rootNode = doc->NewElement("dict");
    
    for (const auto &iter : dict)
    {
        XMLElement* tmpNode = doc->NewElement("key");
        rootNode->LinkEndChild(tmpNode);
        XMLText* content = doc->NewText(iter.first.c_str());
        tmpNode->LinkEndChild(content);
        
        XMLElement *element = _generateElementForObject(iter.second, doc);
        if (element)
            rootNode->LinkEndChild(element);
    }
    return rootNode;
}

/*
 * _generate XMLElement for Array through a XMLDocument
 */
static XMLElement* _generateElementForArray(const ValueVector& array, XMLDocument *pDoc)
{
    XMLElement* rootNode = pDoc->NewElement("array");
    
    for(const auto &value : array) {
        XMLElement *element = _generateElementForObject(value, pDoc);
        if (element)
            rootNode->LinkEndChild(element);
    }
    return rootNode;
}