/*
 *  item.h
 *
 */

#ifndef __ITEM_H__
#define __ITEM_H__ 1

#define MAX_ITEMS 32

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
    Vector2 position;
    Color color;
    int height;
    int width;
    float rotation; // NOTE(nick): item spinning / shrinking?
    ItemType type;
    ItemSubType subType;
} Item;

typedef struct _itemCollection
{
    Item list[MAX_ITEMS];
    int size;
    int capacity;
} ItemCollection;

internal int
AddItem(ItemCollection *collection, Item item)
{
    if (collection->capacity >= MAX_ITEMS)
    {
        InvalidCodePath;
    }
    collection->list[collection->capacity] = item;
    collection->capacity++;
    return (collection->capacity - 1);
}

internal void
RemoveItem(ItemCollection *collection, int itemIndex)
{
    int i = collection->capacity - 1;
    collection->list[itemIndex] = collection->list[i];
    // TODO(nick): properly zero out
    collection->list[i].type = NOITEMSUBTYPE;
    collection->list[i].subType = NOITEMSUBTYPE;
    collection->capacity--;
}

#endif
