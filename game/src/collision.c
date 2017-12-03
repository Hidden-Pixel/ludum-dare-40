/*
 *  collision.c
 *
 */

// Output vector 3 details the move r2 has to make to no longer collide with r1.  The z of the vector 3 will be true (positive int) if it has collided, false (0) if it has not.
internal Vector3
RectCollision3(Vector2 r1tl, Vector2 r1br, Vector2 r2tl, Vector2 r2br)
{
	Vector2 r1Center = Vector2Add(r1tl, r1br);
	Vector2Divide(&r1Center, 2);
	Vector2 r2Center = Vector2Add(r2tl, r2br);
	Vector2Divide(&r2Center, 2);
	Vector2 difference = Vector2Subtract(r1Center, r2Center);
	difference.x = fabs(difference.x);
	difference.y = fabs(difference.y);
	Vector2 r1Size = Vector2Subtract(r1br, r1tl);
	Vector2 r2Size = Vector2Subtract(r2br, r2tl);
	Vector2 avgSize = Vector2Add(r1Size, r2Size);
	Vector2Divide(&avgSize, 2);
	DrawRectangle(r1tl.x, r1tl.y, r1Size.x, r1Size.y, RED);
	DrawRectangle(r2tl.x, r2tl.y, r2Size.x, r2Size.y, BLUE);
	Vector3 move = Vector3Zero();
	move.z = (difference.x < avgSize.x && difference.y < avgSize.y);
	if (!move.z) 
	{
		move.x = 0;
		move.y = 0;
		return move;
	}
	
	Vector2 d = Vector2Subtract(avgSize, difference);
	if (d.x > d.y) 
	{
		move.y = (r2Center.y > r1Center.y) ? d.y : -d.y;
	}
	else 
	{
		move.x = (r2Center.x > r1Center.x) ? d.x : -d.x;
	}

	return move;
}

internal bool 
RectCollision(Vector2 r1tl, Vector2 r1br, Vector2 r2tl, Vector2 r2br) 
{
	return RectCollision3(r1tl, r1br, r2tl, r2br).z;
}

internal Vector2
RectCollisionMove(Vector2 r1tl, Vector2 r1br, Vector2 r2tl, Vector2 r2br)
{
	Vector3 move = RectCollision3(r1tl, r1br, r2tl, r2br);
	return (Vector2){move.x, move.y};
}
