#include "PlaceItem.h"

// �Ծ�̬��Ա�������г�ʼ��
int PlaceItem::startPlaceId = -1;
int PlaceItem::endPlaceId = -1;
bool PlaceItem::selectingStart = false;
bool PlaceItem::selectingEnd = false;
PlaceItem::ItemMode PlaceItem::currentMode = PlaceItem::ViewProperties; // ��ʼ��Ϊ�鿴����ģʽ
