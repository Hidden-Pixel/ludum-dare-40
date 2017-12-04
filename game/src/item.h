/*
 *  item.h
 *
 */

#ifndef __ITEM_H__
#define __ITEM_H__ 1

typedef enum _itemType
{
    NOITEMTYPE = 0x00,
    HEALTHPACK = 0x01,
    AMMO       = 0x02,
    WEAPON     = 0x03,
} ItemType;

typedef enum _itemSubType
{
    NOITEMSUBTYPE = 0x00,
    WHIP          = 0x01,
    REVOVLER      = 0x02,
    BULLET        = 0x03,
} ItemSubType;

typedef struct _item
{
    unsigned int type;
} Item;

#endif
