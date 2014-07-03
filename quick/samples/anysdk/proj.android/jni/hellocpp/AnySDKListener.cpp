
#include "cocos2d.h"
#include "CCLuaBridge.h"
#include "AnySDKListener.h"
#include "stringbuffer.h"
#include "prettywriter.h"

namespace anysdk { namespace framework {

AnySDKListener* AnySDKListener::gInstance = NULL;

AdsListenerLua::AdsListenerLua()
{
}

AdsListenerLua::~AdsListenerLua()
{
}

void AdsListenerLua::onAdsResult(AdsResultCode code, const char* msg)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

    writer.String("AdsResultCode");
    writer.Int(code);
    writer.String("msg");
    writer.String(msg);

    writer.EndObject();

	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "ads", buffer.GetString());
}

void AdsListenerLua::onPlayerGetPoints(ProtocolAds* pAdsPlugin, int points)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

    writer.String("points");
    writer.Int(points);

    writer.EndObject();

	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "ads", buffer.GetString());
}

PayResultListenerLua::PayResultListenerLua()
{
}

PayResultListenerLua::~PayResultListenerLua()
{
}

void PayResultListenerLua::onPayResult(PayResultCode ret, const char* msg, TProductInfo info)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

	writer.String("msg");
    writer.String(msg);
    writer.String("info");

    writer.StartObject();
    for (TProductInfo::iterator it = info.begin(); it != info.end(); ++it)
    {
        writer.String(it->first.c_str());
        writer.String(it->second.c_str());
    }
    writer.EndObject();

    writer.EndObject();
	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "iap", buffer.GetString());
}

PushActionListenerLua::PushActionListenerLua()
{
}

PushActionListenerLua::~PushActionListenerLua()
{

}

void PushActionListenerLua::onActionResult(ProtocolPush* pPlugin, PushActionResultCode code, const char* msg)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

    writer.String("code");
    writer.Int(code);
    writer.String("msg");
    writer.String(msg);

    writer.EndObject();

	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "push", buffer.GetString());
}

ShareResultListenerLua::ShareResultListenerLua()
{

}

ShareResultListenerLua::~ShareResultListenerLua()
{

}

void ShareResultListenerLua::onShareResult(ShareResultCode ret, const char* msg)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

    writer.String("msg");
    writer.String(msg);

    writer.EndObject();

	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "share", buffer.GetString());
}

SocialListenerLua::SocialListenerLua()
{

}

SocialListenerLua::~SocialListenerLua()
{

}

void SocialListenerLua::onSocialResult(SocialRetCode code, const char* msg)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

    writer.String("msg");
    writer.String(msg);

    writer.EndObject();

	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "social", buffer.GetString());
}

UserActionListenerLua::UserActionListenerLua()
{

}

UserActionListenerLua::~UserActionListenerLua()
{

}

void UserActionListenerLua::onActionResult(ProtocolUser* pPlugin, UserActionResultCode code, const char* msg)
{
	rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();

    writer.String("msg");
    writer.String(msg);

    writer.EndObject();

	AnySDKListener::getInstance()->callLuaFunction(m_luaFunction, "user", buffer.GetString());
}


AnySDKListener::AnySDKListener()
{

}

AnySDKListener::~AnySDKListener()
{

}

AnySDKListener* AnySDKListener::getInstance()
{
	if (NULL == gInstance)
	{
		gInstance = new AnySDKListener();
	}

	return gInstance;
}

void AnySDKListener::callLuaFunction(int luaFunction, const char* protocolName, const char* jsonStr)
{
	cocos2d::LuaEngine *engine = cocos2d::LuaEngine::getInstance();
    cocos2d::LuaStack *stack = engine->getLuaStack();
    cocos2d::LuaBridge::pushLuaFunctionById(luaFunction);
    stack->pushString(protocolName);
    stack->pushString(jsonStr);
    stack->executeFunction(2);
}

void AnySDKListener::releaseLuaFunction(int luaFunction)
{
	if (0 == luaFunction)
	{
		return;
	}
	cocos2d::LuaBridge::releaseLuaFunctionById(luaFunction);
}


void AnySDKListener::setAdListener(int luaFunction, ProtocolAds* ads)
{
	if (NULL == ads)
	{
		ads = AgentManager::getInstance()->getAdsPlugin();
	}
	if (NULL == m_listenerAds)
	{
		m_listenerAds = new AdsListenerLua();
	}
	ads->setAdsListener(m_listenerAds);
	m_listenerAds->m_luaFunction = luaFunction;
}

void AnySDKListener::setIAPListener(int luaFunction, ProtocolIAP* iap)
{
	if (NULL == iap)
	{
		return;
	}

	if (NULL == m_listenerIAP)
	{
		m_listenerIAP = new PayResultListenerLua();
	}
	iap->setResultListener(m_listenerIAP);
	m_listenerIAP->m_luaFunction = luaFunction;
}

void AnySDKListener::setPushListener(int luaFunction, ProtocolPush* push)
{
	if (NULL == push)
	{
		push = AgentManager::getInstance()->getPushPlugin();
	}
	if (NULL == m_listenerPush)
	{
		m_listenerPush = new PushActionListenerLua();
	}
	push->setActionListener(m_listenerPush);
	m_listenerPush->m_luaFunction = luaFunction;
}

void AnySDKListener::setShareListener(int luaFunction, ProtocolShare* share)
{
	if (NULL == share)
	{
		share = AgentManager::getInstance()->getSharePlugin();
	}
	if (NULL == m_listenerShare)
	{
		m_listenerShare = new ShareResultListenerLua();
	}
	share->setResultListener(m_listenerShare);
	m_listenerShare->m_luaFunction = luaFunction;
}

void AnySDKListener::setSocialListener(int luaFunction, ProtocolSocial* social)
{
	if (NULL == social)
	{
		social = AgentManager::getInstance()->getSocialPlugin();
	}
	if (NULL == m_listenerSocial)
	{
		m_listenerSocial = new SocialListenerLua();
	}
	social->setListener(m_listenerSocial);
	m_listenerSocial->m_luaFunction = luaFunction;
}

void AnySDKListener::setUserListener(int luaFunction, ProtocolUser* user)
{
	if (NULL == user)
	{
		user = AgentManager::getInstance()->getUserPlugin();
	}
	if (NULL == m_listenerUser)
	{
		m_listenerUser = new UserActionListenerLua();
	}
	user->setActionListener(m_listenerUser);
	m_listenerUser->m_luaFunction = luaFunction;
}

}}
