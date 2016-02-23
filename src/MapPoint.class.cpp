#include "MapPoint.class.hpp"

MapPoint::MapPoint() : terrain_height(0), water_level(0)
{}

MapPoint::MapPoint(const MapPoint & rhs)
{
	terrain_height = rhs.terrain_height.load();
	water_level = rhs.water_level.load();
}

MapPoint::~MapPoint()
{}

MapPoint & MapPoint::operator=(const MapPoint & rhs)
{
	std::memcpy(this, &rhs, sizeof(*this));
	return *this;
}
