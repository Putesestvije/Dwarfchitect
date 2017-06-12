#include "site.h"
#include <iostream>


Site::Site()
{

}

Site::Site(int width, int height)
    : _width(width),
      _height(height)
{
    Floor *f = new Floor(width,height);
    connect(f, &Floor::syncRequired, this, &Site::syncRequired);
    _topFloor = f;
    _bottomFloor = f;
    _currFloor = f;
}

Site::~Site()
{
    Floor *curr = _topFloor;
    Floor *next = nullptr;
    if(curr->floorBelow() != nullptr)
        Floor *next = _topFloor->floorBelow();

    while (curr != nullptr){
        delete curr;
        if(next != nullptr){
            curr = next;
            next = curr->floorBelow();
        }
    }
}

void Site::addFloorAboveCurr()
{
    Floor *f = new Floor(_width,
                         _height,
                         _currFloor->floorAbove(),
                         _currFloor->floorBelow());
    if(_currFloor->floorAbove() != nullptr)
        _currFloor->floorAbove()->setFloorBelow(f);
    else
        _topFloor = f;
    _currFloor->setFloorAbove(f);
    connect(f, &Floor::syncRequired, this, &Site::syncRequired);
    //connect()
}

void Site::addFloorBelowCurr()
{
    Floor *f = new Floor(_width,
                         _height,
                         _currFloor->floorAbove(),
                         _currFloor->floorBelow());
    if(_currFloor->floorBelow() != nullptr)
        _currFloor->floorBelow()->setFloorAbove(f);
    else
        _bottomFloor = f;
    _currFloor->setFloorBelow(f);
    connect(f, &Floor::syncRequired, this, &Site::syncRequired);
}

void Site::addNewTopFloor()
{
    Floor *f = new Floor(_width,
                         _height,
                         nullptr,
                         _topFloor);
    _topFloor->setFloorAbove(f);
    _topFloor = f;
    connect(f, &Floor::syncRequired, this, &Site::syncRequired);
}

void Site::addNewBottomFloor()
{
    Floor *f = new Floor(_width,
                         _height,
                         _bottomFloor,
                         nullptr);
    _bottomFloor->setFloorBelow(f);
    _bottomFloor = f;
    connect(f, &Floor::syncRequired, this, &Site::syncRequired);
}

Floor *Site::currFloor() const
{
    return _currFloor;
}

void Site::setCurrFloor(Floor *currFloor)
{
    _currFloor = currFloor;
}

void Site::moveCurrUp()
{
    if(_currFloor->floorAbove())
        _currFloor = _currFloor->floorAbove();
}

void Site::moveCurrDown()
{
    if(_currFloor->floorBelow())
        _currFloor = _currFloor->floorBelow();
}

void Site::undo()
{
    _currFloor->undo();
}

void Site::redo()
{
    _currFloor->redo();
}

void Site::tester()
{
    std::cout << "Sync getting through" << std::endl;
}

Floor *Site::bottomFloor() const
{
    return _bottomFloor;
}

Floor *Site::topFloor() const
{
    return _topFloor;
}
