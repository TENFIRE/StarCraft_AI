#include "WalkTile.h"

WalkTile::WalkTile(TilePosition tp)
{
	Position pos = Position(tp.x()*32+16, tp.y()*32+16);
	x = pos.x() / 8;
	y = pos.y() / 8;
}

WalkTile::WalkTile(Position pos)
{
	x = pos.x() / 8;
	y = pos.y() / 8;
}

WalkTile::WalkTile(Unit* unit)
{
	Position pos = unit->getPosition();
	x = pos.x() / 8;
	y = pos.y() / 8;
}

WalkTile::WalkTile(int x, int y)
{
	this->x = x;
	this->y = y;
}

int WalkTile::X()
{
	return x;
}

int WalkTile::Y()
{
	return y;
}

TilePosition WalkTile::getTilePosition()
{
	Position pos = Position(x*8+4, y*8+4);
	return TilePosition(pos);
}

Position WalkTile::getPosition()
{
	Position pos = Position(x*8+4, y*8+4);
	return pos;
}

double WalkTile::getDistance(WalkTile wt)
{
	Position p1 = getPosition();
	Position p2 = wt.getPosition();
	double d = p1.getDistance(p2);
	return d/8;
}
