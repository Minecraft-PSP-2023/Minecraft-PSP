#ifndef ARROW2_H_
#define ARROW2_H_

#include <Lamecraft/Zombie2.h>
#include <Lamecraft/Spider2.h>
#include <Lamecraft/Cow2.h>
#include <Lamecraft/Chicken2.h>
#include <Lamecraft/Pig2.h>
#include <Lamecraft/Creeper2.h>
#include <Lamecraft/Sheep2.h>
#include <Lamecraft/LameMob.h>

#include <Aurora/Math/Vector3.h>
#include <Aurora/Graphics/Vertex.h>
#include <pspmath.h>
#include <LameCraft/CraftWorld2.h>
#include <math.h>
#include <psptypes.h>

namespace Aurora
{
	namespace Graphics
	{
		class Arrow2
		{
		public:
			Arrow2(float pos2x, float pos2y, float pos2z);
			~Arrow2();

			void SetTexture(int texture);
			void SetVeloc(float verAngle, float horAngle);
			void Update(CraftWorld* crtf, float dt);
			bool CheckCollision(CraftWorld* crtf);
			
			//void Kick(CraftWorld* world);

			void Render();

			Vector3 position;
			Vector3 velocity;

		private:

			CraftPSPVertex *ballVertices;
			int textureNumber;
			float snowHScale;
			float snowVScale;

			float startSpeed;
		};
	}
}

#endif

