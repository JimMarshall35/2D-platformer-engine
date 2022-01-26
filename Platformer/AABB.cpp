#include "AABB.h"

bool AABBCollision(const glm::vec4& bb1, const glm::vec4& bb2)
{
	return ((bb1[3] > bb2[1]) && (bb2[3] > bb1[1])) &&
		((bb1[2] > bb2[0]) && (bb2[2] > bb1[0]));
}
