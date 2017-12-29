/*
 *  entity.h
 *
 */

#ifndef __ENTITY_H__
#define __ENTITY_H__ 1

#define MAX_ENTITIES 2048
#define MAX_ITEM_SLOT 3

typedef enum _entityType
{
    NOENTITYTYPE      = 0x00,
    PLAYER            = 0x01,
    ENEMY             = 0x02,
} EntityType;

typedef enum _entitySubType
{
    NOENTITYSUBTYPE = 0x00,
    BOULDER         = 0x01,
    SKELETON        = 0x02,
    SOLIDER         = 0x03,
    BULLET          = 0x04,
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
    Vector2 direction;
    float rotation;
    float maxVelocity;
    EntityProp props;
    Color color;
    int state;
    int counter;
    float sightDistance;
    int height;
    int width;
    int health;
    int baseDamage;
    Item items[MAX_ITEM_SLOT];
} Entity;

typedef struct _entityCollection
{
    Entity list[MAX_ENTITIES];
    int size;
    int capacity;
} EntityCollection;

internal Entity
EntityZeroed()
{
    Entity result =
    {
        .position = { 0, 0, },
        .velocity = { 0, 0, },
        .color = 0,
        .rotation = 0.0f,
        .maxVelocity = 0.0f,
        .props.type = NOENTITYTYPE,
        .props.subType = NOENTITYSUBTYPE,
        .props.attributes = NOENTITYATTRIBUTES,
        .state = 0,
        .sightDistance = 0.0f,
        .counter = 0,
        .height = 0,
        .width = 0,
        .health = 0,
        .baseDamage = 0,
        .items = { 0, 0, 0, },
    };
    return result;
}

internal void
InitEntityCollection(EntityCollection *collection)
{
    int i;
    for (i = 0; i < MAX_ENTITIES; ++i)
    {
        collection->list[i] = EntityZeroed();
    }
}

internal int 
AddEntity(EntityCollection *collection, Entity entity)
{
	if (collection->capacity >= MAX_ENTITIES)
		InvalidCodePath;

	collection->list[collection->capacity] = entity;
	collection->capacity++;
	return collection->capacity-1;
}

internal void
RemoveEntity(EntityCollection *collection, int entityId)
{
	collection->list[entityId] = collection->list[collection->capacity-1];
    collection->list[collection->capacity-1] = EntityZeroed();
	collection->capacity--;
}

internal void
RemoveAllEntities(EntityCollection *collection)
{
    while (collection->capacity >= 0)
    {
        RemoveEntity(collection, collection->capacity - 1);
    }
}

internal Entity
GetBullet(Entity *spawnEntity)
{
    Entity bullet = (Entity)
    {
        .props.type = ENEMY,
        .props.subType = BULLET,
        .props.attributes = NOENTITYATTRIBUTES,
        .color = RED,
        .maxVelocity = BULLET_DEFAULT_SPEED,
        .width = BULLET_DEFAULT_SIZE,
        .height = BULLET_DEFAULT_SIZE,
        .baseDamage = BULLET_BASE_DAMAGE,
    };

    bullet.position = Vector2Zero();

    bullet.position.x = (spawnEntity->direction.x > spawnEntity->position.x) ?
        spawnEntity->position.x + (spawnEntity->width/2) :
        spawnEntity->position.x - (spawnEntity->width/2);

    bullet.position.y = (spawnEntity->direction.y > spawnEntity->position.y) ?
        spawnEntity->position.y + (spawnEntity->height/2) :
        spawnEntity->position.y - (spawnEntity->height/2);

    bullet.direction = (Vector2) {spawnEntity->direction.x, spawnEntity->direction.y};
    bullet.velocity = Vector2Subtract(bullet.direction, bullet.position);
    Vector2Normalize(&bullet.velocity);
    Vector2Scale(&bullet.velocity, bullet.maxVelocity);

    return bullet;
}

internal void
HandlePlayerAction(EntityCollection *collection, Entity *entity)
{
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Entity bullet = GetBullet(entity);
        AddEntity(collection, bullet);
    }
}

internal bool
HandleWeaponAction(Entity *entity, bool collisionWithTile)
{
    bool result = false;
    switch (entity->props.subType)
    {
        case BULLET:
        {
            if (collisionWithTile)
            {
                result = true;
            }
        } break;
    }
    return result;
}

internal bool
HandleEntityActions(TileMap *gameMap, EntityCollection *collection, int entityIx, bool collisionWithTile)
{
    bool result = false;
    Entity entity = collection->list[entityIx];
    switch(entity.props.type)
    {
        case PLAYER:
        {
            HandlePlayerAction(collection, &entity);
        } break;

        // NOTE(nick): bullets are considered "enemies" as well
        case ENEMY:
        {
            if (HandleWeaponAction(&entity, collisionWithTile))
            {
                RemoveEntity(collection, entityIx);
                result = true;
            }
        } break;
    }
    return result;
}

#endif
