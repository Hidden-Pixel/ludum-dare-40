/*
 *  entity.h
 *
 */
#ifndef __ENTITY_H__
#define __ENTITY_H__ 1

typedef enum _entityType
{
    NOTYPE      = 0x00,
    PLAYER      = 0x01,
    WEAPON      = 0x03,
    ENEMY       = 0x02,
} EntityType;

typedef enum _entitySubTypes
{
    NOSUBTYPE = 0x00,

    // enemy types
    BOULDER   = 0x01,
    SKELETON  = 0x02,
    SOLIDER   = 0X03,

    // weapon types
    WHIP      = 0x0F,
    REVOVLER  = 0x10,
    BULLET    = 0x11,

    // item types
    HEALTHPACK = 0xF0,
    AMMO       = 0xF0,
} EntitySubType;

typedef enum _entityAttributes
{
    NOATTRIBUTES = 0x00,
    DRAG         = 0x01,
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
    Vector2 direction;
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