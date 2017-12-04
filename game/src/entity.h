/*
 *  entity.h
 *
 */
#ifndef __ENTITY_H__
#define __ENTITY_H__ 1

#define MAX_ENTITIES 256


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
	int state;
	int counter;
	float sightDistance;
    int height;
    int width;
} Entity;

typedef struct _entityCollection
{
    Entity list[MAX_ENTITIES];
    int size;
    int capacity;
} EntityCollection;

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
RemoveEntity(EntityCollection *collection, int entityIx)
{
	collection->list[entityIx] = collection->list[collection->capacity-1];
	collection->list[collection->capacity-1].props.type = NOTYPE;
	collection->list[collection->capacity-1].props.subType = NOSUBTYPE;
	collection->list[collection->capacity-1].props.attributes = NOATTRIBUTES;
	collection->capacity--;
}

internal Entity GetBullet(Entity *spawnEntity) {
    Entity bullet = (Entity){
        .props.type = ENEMY,
        .props.subType = BULLET,
        .props.attributes = NOATTRIBUTES,
        .color = RED,
        .maxVelocity = BULLET_DEFAULT_SPEED,
        .width = BULLET_DEFAULT_SIZE,
        .height = BULLET_DEFAULT_SIZE
    };

    bullet.position = Vector2Zero();
    bullet.position.x = (spawnEntity->direction.x > spawnEntity->position.x) ?
        spawnEntity->position.x + (spawnEntity->width/2) :
        spawnEntity->position.x - (spawnEntity->width/2);
    bullet.position.y = (spawnEntity->direction.y > spawnEntity->position.y) ?
        spawnEntity->position.y + (spawnEntity->height/2) :
        spawnEntity->position.y - (spawnEntity->height/2);
    bullet.direction = (Vector2) {spawnEntity->direction.x, spawnEntity->direction.y};
    bullet.velocity = (Vector2) {bullet.direction.x, bullet.direction.y};
    Vector2Normalize(&bullet.velocity);
    Vector2Scale(&bullet.velocity, bullet.maxVelocity);
    return bullet;
}

internal void
HandlePlayerAction(EntityCollection *collection, Entity *entity) {
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Entity bullet = GetBullet(entity);
        bullet.props.type = WEAPON;
        AddEntity(collection, bullet);
    }
}

internal bool
HandleEntityActions(TileMap *gameMap, EntityCollection *collection, int entityIx, bool collisionWithTile) {
    Entity entity = collection->list[entityIx];
    switch(entity.props.type) {
        case PLAYER:
            HandlePlayerAction(collection, &entity);
            return false;
        case WEAPON:
            if (HandleWeaponAction(&entity, collisionWithTile)) {
                RemoveEntity(collection, entityIx);
                return true;
            }
            return false;            
        default:
            return false;
    }
}

internal bool HandleWeaponAction(Entity *entity, bool collisionWithTile) {
    switch (entity->props.subType) {
        case BULLET:
            if (collisionWithTile) return true;
        default:
            return false;
    }
}
#endif