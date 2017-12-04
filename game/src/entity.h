/*
 *  entity.h
 *
 */

#ifndef __ENTITY_H__
#define __ENTITY_H__ 1

typedef enum _entityType
{
    NOENTITYTYPE      = 0x00,
    PLAYER            = 0x01,
    ENEMY             = 0x02,
} EntityType;

typedef enum _entitySubType
{
    NOENTITYSUBTYPE = 0x00,
    BOULDER   = 0x01,
    SKELETON  = 0x02,
    SOLIDER   = 0X03,
} EntitySubType;

typedef enum _entityAttributes
{
    NOENTITYATTRIBUTES = 0x00,
    DRAG               = 0x01,
} EntityAttribute;

typedef struct _entityProps
{
    EntityType type;
    EntitySubType subType;
    EntityAttribute attributes;
} EntityProp;

typedef struct _entity
{
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float maxVelocity;
    Vector3 collider;
    EntityProp props;
    Color color;
} Entity;

typedef struct _entityCollection
{
    Entity list[256];
    int size;
} EntityCollection;

#endif
