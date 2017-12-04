/*
 *  item.h
 *
 */

#ifndef __ITEM_H__
#define __ITEM_H__ 1

#define MAX_ITEMS 256

typedef enum _itemType
{
    NOITEMTYPE = 0x00,
    PICKUP     = 0x01,
    POWERUP    = 0x02,
} ItemType;

typedef enum _itemSubType
{
    NOITEMSUBTYPE = 0x00,
    AMMO          = 0X01,
    HEALTHPACK    = 0x02,
    REVOVLER      = 0x03,
    WEAPON        = 0x04,
    WHIP          = 0x05,
} ItemSubType;

typedef struct _item
{
    unsigned char type;
    unsigned char subtype;
} Item;

typedef struct _itemCollection
{
    Item list[MAX_ITEMS];
    int size;
    int capacity;
} ItemCollection;

#endif
