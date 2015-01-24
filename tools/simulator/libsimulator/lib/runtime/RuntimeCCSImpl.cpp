
#include "RuntimeCCSImpl.h"
#include "ConfigParser.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UIHelper.h"

class RuntimeCCSStartScene : public Scene
{
public:
	RuntimeCCSStartScene();
	static RuntimeCCSStartScene *create();
	virtual void update(float dt);
	void setImpl(RuntimeCCSImpl *impl) { _impl = impl; };

private:
	RuntimeCCSImpl *_impl;
};

RuntimeCCSStartScene::RuntimeCCSStartScene()
:_impl(nullptr)
{

}

RuntimeCCSStartScene* RuntimeCCSStartScene::create()
{
	RuntimeCCSStartScene *ret = new (std::nothrow) RuntimeCCSStartScene();
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void RuntimeCCSStartScene::update(float dt)
{
	auto scheduler = cocos2d::Director::getInstance()->getScheduler();
	scheduler->unschedule(CC_SCHEDULE_SELECTOR(RuntimeCCSStartScene::update), this);
	if(_impl) _impl->load();
}

////////////////////////////////////////

RuntimeCCSImpl *RuntimeCCSImpl::create()
{
    auto instance = new RuntimeCCSImpl();
    return instance;
}

void RuntimeCCSImpl::onStartDebuger(const rapidjson::Document& dArgParse, rapidjson::Document& dReplyParse)
{
}

void RuntimeCCSImpl::onReload(const rapidjson::Document &dArgParse, rapidjson::Document &dReplyParse)
{
}

void RuntimeCCSImpl::load()
{
	std::string filename(_file);
	if (filename.empty())
	{
		filename = ConfigParser::getInstance()->getEntryFile();
	}

	if (filename.rfind(".csd") != std::string::npos)
	{
		loadCSDProject(filename);
	}
	else if (filename.rfind(".csb") != std::string::npos)
	{
		loadCSBProject(filename);
	}
}

void RuntimeCCSImpl::startScript(const std::string& file)
{
	_file = file;
	auto scene = RuntimeCCSStartScene::create();
	scene->setImpl(this);
	//auto node = Node::create();
	if (Director::getInstance()->getRunningScene())
	{
		//scene->addChild(node);
		Director::getInstance()->replaceScene(scene);
	}
	else
	{
		//scene->addChild(node);
		Director::getInstance()->runWithScene(scene);
	}

	auto scheduler = cocos2d::Director::getInstance()->getScheduler();
	scheduler->schedule(CC_SCHEDULE_SELECTOR(RuntimeCCSStartScene::update), scene, 0.05f, false);
}

void RuntimeCCSImpl::loadCSDProject(const std::string& file)
{
    CCLOG("------------------------------------------------");
    CCLOG("LOAD Cocos Studio FILE (.csd): %s", file.c_str());
    CCLOG("------------------------------------------------");
    
    auto node = CSLoader::getInstance()->createNodeWithFlatBuffersForSimulator(file.c_str());
    auto action = cocostudio::timeline::ActionTimelineCache::getInstance()->createActionWithFlatBuffersForSimulator(file.c_str());
    if (action)
    {
        node->runAction(action);
        action->gotoFrameAndPlay(0);
    }
    
    if (node)
    {
        Size frameSize = Director::getInstance()->getVisibleSize();
        node->setContentSize(frameSize);
        ui::Helper::doLayout(node);

        if (Director::getInstance()->getRunningScene())
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->replaceScene(scene);
        }
        else
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->runWithScene(scene);
        }
    }
}

void RuntimeCCSImpl::loadCSBProject(const std::string& file)
{
    CCLOG("\n------------------------------------------------\n");
    CCLOG("[WARNING]: using **SUITABLE** Cocos Studio generate csb file!!");
    CCLOG("LOAD Cocos Studio FILE (.csb): %s", file.c_str());
    CCLOG("\n------------------------------------------------\n");
    
    auto node = CSLoader::getInstance()->createNode(file);
    if (node)
    {
        Size frameSize = Director::getInstance()->getVisibleSize();
        node->setContentSize(frameSize);
        ui::Helper::doLayout(node);

        if (Director::getInstance()->getRunningScene())
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->replaceScene(scene);
        }
        else
        {
            auto scene = Scene::create();
            scene->addChild(node);
            Director::getInstance()->runWithScene(scene);
        }
    }
}
