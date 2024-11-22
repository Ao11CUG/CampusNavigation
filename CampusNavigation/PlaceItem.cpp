#include "PlaceItem.h"

// 对静态成员变量进行初始化
int PlaceItem::startPlaceId = -1;
int PlaceItem::endPlaceId = -1;
bool PlaceItem::selectingStart = false;
bool PlaceItem::selectingEnd = false;
PlaceItem::ItemMode PlaceItem::currentMode = PlaceItem::ViewProperties; // 初始化为查看属性模式
