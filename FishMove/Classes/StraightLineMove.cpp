//
//  StraightLineMove.cpp
//  FishMove
//
//  Created by FreeBlank on 17/7/29.
//
//

#include "StraightLineMove.h"

bool StraightLineMove::setTotalTime(float time) {
    if (!BaseMove::setTotalTime(time)) return false;
    
    if (_pos[1].x==_pos[0].x && _pos[1].y==_pos[1].y) return false;
    
    _step.x = 1.0/_totalTime*(_pos[1].x-_pos[0].x);
    _step.y = 1.0/_totalTime*(_pos[1].y-_pos[0].y);
    
    return true;
}

Point StraightLineMove::next(float delta, bool fix) {
    if (!fix) {
        _calcPos = _curPos;
    }
    _calcPos.x += _step.x*delta;
    _calcPos.y += _step.y*delta;
    
    return BaseMove::next(delta, fix);
}

bool StraightLineMove::isEnd() {
    return (_pos[0].x-_pos[1].x)*(_curPos.x-_pos[1].x)<=0 || (_pos[0].y-_pos[1].y)*(_curPos.y-_pos[1].y)<=0;
}

void StraightLineMove::setPoints(const std::vector<Point> &points) {
    if (points.size() < 2) return;
    
    _pos[0] = points[0];
    _pos[1] = points[1];
    
    _curPos = _pos[0];
    
    setTotalTime(_totalTime);
}
