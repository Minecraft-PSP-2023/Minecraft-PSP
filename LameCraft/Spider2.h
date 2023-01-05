#ifndef SPIDER_H_
#define SPIDER_H_

#include <Aurora/Graphics/Vertex.h>
#include <Aurora/Graphics/RenderManager.h>
#include <pspmath.h>
#include <psptypes.h>
#include <Aurora/Math/Frustum.h>

#include <Aurora/Math/BoundingBox.h>
#include <Aurora/Math/Frustum.h>
#include "LameMob.h"

class CraftWorld;

class Spider : public LameMob
{
public:

    Spider(Vector3 position1, int number1);
    ~Spider();

    void BoxCollision();
    void Update(CraftWorld *myWorld, Vector3 playerPos, float dt);
    void Render(Frustum &camFrustum, float dt);
    void TakeDamage(float damage, float power, float dt);
    void TakeDamageFromPointExplosion(float damage, float power, float dt, Vector3 point);
    void TakeDamageForArrow(float damage, float power, float dt, Vector3 point);
	void Haunting(CraftWorld *myWorld, float dt);

private:

    float rHandAngle;
    float lHandAngle;
    float rLegAngle;
    float lLegAngle;

    float headAngleZ;
    float headAngleY;

    float animLegAngle;
    float animLegAngleT;

    float sunTimer;
};

#endif

