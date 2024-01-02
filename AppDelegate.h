#ifndef  APP_DELEGATE_H
#define  APP_DELEGATE_H

#include "cocos2d.h"

using namespace cocos2d;

class AppDelegate : private Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();
    virtual void initGLContextAttrs();
    virtual bool applicationDidFinishLaunching();
    virtual void applicationDidEnterBackground();
    virtual void applicationWillEnterForeground();
};

#endif
