//
//  EditorLayer.cpp
//  FishMove
//
//  Created by FreeBlank on 17/7/29.
//
//

#include "EditorLayer.h"
#include "StraightLineMove.h"

bool EditorLayer::init() {
    if (!LayerColor::initWithColor(Color4B::BLACK)) return false;
    
    _selectMoveType = MoveType_Null;
    _move = nullptr;
    
    initUI();
    return true;
}

EditorLayer::~EditorLayer()
{
    if (_move) {
        delete _move;
        _move = nullptr;
    }
}

void EditorLayer::initUI() {
    float i_x = 1020;
    float i_y = 40;
    float i_step = 100;
    Button *i_button = createButton(Button_Blue);
    i_button->setPosition(Vec2(i_x, i_y));
    i_button->setTag(MoveType_StraightLine);
    i_button->setTitleText("StraightLine");
    addChild(i_button);
    i_y += i_step;
    
    i_button = createButton(Button_Blue);
    i_button->setPosition(Vec2(i_x, i_y));
    i_button->setTag(MoveType_Lagrange);
    i_button->setTitleText("Lagrange");
    addChild(i_button);
    i_y += i_step;
    
    
    i_button = createButton(Button_Blue);
    i_button->setPosition(Vec2(i_x, i_y));
    i_button->setTag(MoveType_Bezier);
    i_button->setTitleText("Bezier");
    addChild(i_button);
    i_y += i_step;
    
    i_button = createButton(Button_Blue);
    i_button->setPosition(Vec2(i_x, i_y));
    i_button->setTag(MoveType_Paramtric);
    i_button->setTitleText("Paramtric");
    addChild(i_button);
    i_y += i_step;
    
    i_button = createButton(Button_Blue);
    i_button->setPosition(Vec2(i_x, i_y));
    i_button->setTag(MoveType_Polar);
    i_button->setTitleText("Polar");
    addChild(i_button);
    i_y += i_step;
    
    i_button = createButton(Button_Yellow);
    i_button->setPosition(Vec2(800, 40));
    i_button->setTitleText("Finish");
    i_button->addClickEventListener([=](Ref*){
        show(false);
        
        if (_haveChange) {
            _eventDispatcher->dispatchCustomEvent("Editor_Finish", &_selectMoveType);
        } else {
            _eventDispatcher->dispatchCustomEvent("Editor_Finish_No_Change", &_selectMoveType);
        }
    });
    addChild(i_button);
    
    _layerParamtric = Layer::create();
    _layerParamtric->setAnchorPoint(Vec2::ZERO);
    _layerParamtric->setPosition(Vec2::ZERO);
    addChild(_layerParamtric, 1);
    
    _layerPolar = Layer::create();
    _layerPolar->setAnchorPoint(Vec2::ZERO);
    _layerPolar->setPosition(Vec2::ZERO);
    addChild(_layerPolar, 1);
    
    _layerPoint = Layer::create();
    _layerPoint->setAnchorPoint(Vec2::ZERO);
    _layerPoint->setPosition(Vec2::ZERO);
    addChild(_layerPoint, 2);
    
    _drawNode = DrawNode::create();
    addChild(_drawNode, 10);
}

Button* EditorLayer::createButton(ButtonType type) {
    std::stringstream ss;
    ss << "button" << type << ".png";
    log("%s, %d\n", ss.str().c_str(), type);
    
    Button *i_button = Button::create(ss.str().c_str());
    i_button->setTitleColor(Color3B::BLACK);
    i_button->setTitleFontSize(30);
    
    i_button->setScale9Enabled(true);
    i_button->setCapInsets(Rect(10, 10, 230, 38));
    i_button->setContentSize(Size(200, 50));
    
    if (type == Button_Blue)
    {
        i_button->addClickEventListener([=](Ref *ref){
            Button* button = dynamic_cast<Button *>(ref);
            onClickButton((MoveType)button->getTag());
        });
    }
    return i_button;
}

void EditorLayer::show(bool i_show) {
    if (i_show) {
        _haveChange = false;
        FishManager::getInstance()->lockAllFishesForEditor();
        onClickButton(MoveType_StraightLine);
        setVisible(true);
    } else {
        FishManager::getInstance()->removeAllFish();
        FishManager::getInstance()->unlockAllFishes();
        setVisible(false);
    }
}

void EditorLayer::cleanEditor() {
    //hide all button
    showPolar(false);
    showParamtric(false);
    
    _layerPoint->removeAllChildren();
    _drawNode->clear();
}

void EditorLayer::onClickButton(MoveType tag) {
    _selectMoveType = tag;
    
    cleanEditor();
    
    switch (tag) {
        case MoveType_StraightLine:
            break;
        case MoveType_Lagrange:
            break;
        case MoveType_Bezier:
            break;
        case MoveType_Paramtric:
            showParamtric(true);
            break;
        case MoveType_Polar:
            showPolar(true);
            break;
        default:
            break;
    }
    
    FishManager::getInstance()->loadPoints(_selectMoveType, _points);
    for (int i=0; i<_points.size(); ++i) {
        addPoint(_points[i], i);
    }
    refreshFishes();
}

void EditorLayer::showPolar(bool show) {
    
}

void EditorLayer::showParamtric(bool show) {
    
}

void EditorLayer::refreshFishes() {
    FishManager::getInstance()->removeAllFish();
    
    switch (_selectMoveType) {
        case MoveType_StraightLine:
            _move = new StraightLineMove();
            dynamic_cast<StraightLineMove*>(_move)->setPoints(_points);
            break;
            
        default:
            break;
    }
    
    if (!_move) return;
    
    float i_total_time = 10;
    _move->setTotalTime(i_total_time);
    for (int i=0; i<4; i++)
    {
        _move->next(i_total_time/5);
        FishSprite* i_fish = FishManager::getInstance()->addFish();
        i_fish->setPosition(_move->getCurPos());
        i_fish->setRotation(180-_move->getAngle());
    }
}

void EditorLayer::movePoint(Button *button, Vec2 pos) {
    button->setPosition(pos);
    _points[button->getTag()] = pos;
    
    _drawNode->clear();
    for (int i=0; i<_points.size(); ++i)
    {
        _drawNode->drawPoint(_points[i], 5, Color4F::RED);
    }
}

void EditorLayer::addPoint(Vec2 pos, int tag) {
    if (tag < 0)
    {
        tag = (int)_points.size();
        _points.push_back(pos);
    }
    _drawNode->drawPoint(pos, 5, Color4F::RED);
    
    Button *i_button = Button::create("point.png");
    i_button->setAnchorPoint(Vec2(0.5, 0.5));
    i_button->setPosition(pos);
    i_button->setScale(80);
    i_button->setTag(tag);
    i_button->setColor(Color3B::BLUE);
    i_button->setOpacity(50);
    i_button->addTouchEventListener([=](Ref* ref,Widget::TouchEventType type) {
        Button *button = dynamic_cast<Button *>(ref);
        if (type == Widget::TouchEventType::MOVED) {
            this->movePoint(button, button->getTouchMovePosition());
        } else if (type == Widget::TouchEventType::ENDED) {
            this->movePoint(button, button->getTouchEndPosition());
            FishManager::getInstance()->savePoints(_selectMoveType, _points);
            this->refreshFishes();
            
            _haveChange = true;
        }
    });
    
    _layerPoint->addChild(i_button);
}



void EditorLayer::onEnter()
{
    Layer::onEnter();
    
    // Register Touch Event
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    
    listener->onTouchBegan = CC_CALLBACK_2(EditorLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(EditorLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(EditorLayer::onTouchEnded, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool EditorLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    cocos2d::log("onTouchBegan");
    
    if (!isVisible()) return false;
    return true;
}

void EditorLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    cocos2d::log("onTouchMoved");
}

void EditorLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    cocos2d::log("onTouchEnded");
}




