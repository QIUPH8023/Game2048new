#include "AppDelegate.h"
#include "GameLayer.h"

static cocos2d::Size gameResolutionSize = cocos2d::Size(480, 480);

AppDelegate::AppDelegate() = default;

AppDelegate::~AppDelegate() = default;

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // Initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("Cpp Sample");
        director->setOpenGLView(glview);
    }

    std::string icons = "icons/icon.png";
    glview->setIcon(icons);

    director->setOpenGLView(glview);

    // Set the design resolution
    glview->setDesignResolutionSize(
        gameResolutionSize.width,
        gameResolutionSize.height,
        ResolutionPolicy::NO_BORDER
    );

    // Turn on display FPS
    director->setDisplayStats(false);

    // Set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Create a scene. it's an autorelease object
    auto scene = GameLayer::createScene();

    // Run
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground(){
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
