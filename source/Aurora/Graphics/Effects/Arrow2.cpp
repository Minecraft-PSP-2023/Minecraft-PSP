#include <Aurora/Graphics/Effects/Arrow2.h>
#include <Aurora/Graphics/TextureManager.h>

#define PI 3.1415926535897f
#define DEG_TO_RAD (PI / 180.0f)

namespace Aurora
{
	namespace Graphics
	{
		Arrow2::Arrow2(float pos2x, float pos2y, float pos2z)
		{		
            snowHScale = 0.2f;
			snowVScale = 0.2f;

			position = Vector3(pos2x,pos2y,pos2z);

			ballVertices = (CraftPSPVertex*)memalign(16,4 * sizeof(CraftPSPVertex));

			startSpeed = 0.70;
		}

		Arrow2::~Arrow2()
		{
		    if(ballVertices != NULL)
            {
                free(ballVertices);
            }
		}

		void Arrow2::SetTexture(int texture)
		{
			textureNumber = texture;
		}

		void Arrow2::SetVeloc(float verAngle, float horAngle)
		{
			float addVerAngle = 0.01;
		    float addHorAngle = 0.0;
            velocity.x = -sinf(horAngle-PI/2 + addHorAngle) * cosf(verAngle + addVerAngle) * startSpeed;
            velocity.y = sinf(verAngle + addVerAngle) * startSpeed;
			velocity.z = cosf(horAngle-PI/2 + addHorAngle) * cosf(verAngle + addVerAngle) * startSpeed;
		}

		void  Arrow2::Update(CraftWorld* crtf, float dt)
		{
		    float myLight = crtf->BlockFinalLight(position.x,position.y,position.z);

			ScePspFMatrix4 t;
			sceGumMatrixMode(GU_VIEW);
			sceGumStoreMatrix(&t);
			sceGumMatrixMode(GU_MODEL);

            ScePspFVector3 up, right;

			right.x = t.x.x * snowHScale * 1;
			right.y = t.y.x * snowHScale * 1;
			right.z = t.z.x * snowHScale * 1;
			up.x = t.x.y * snowVScale * 1;
			up.y = t.y.y * snowVScale * 1;
			up.z = t.z.y * snowVScale * 1;

            velocity.y -= 0.3*dt;

            position.x += velocity.x;
            position.y += velocity.y;
			position.z += velocity.z;

			int i = 0;

			ballVertices[i].x = position.x - right.x + up.x;// * stepScale;
			ballVertices[i].y = position.y - right.y + up.y;// * stepScale;
			ballVertices[i].z = position.z - right.z + up.z;// * stepScale;
            ballVertices[i].u = 0.f;// * textureScale;
			ballVertices[i].v = 0.f;// * textureScale;
			ballVertices[i].color = GU_COLOR(myLight,myLight,myLight,1);
			i++;

			// (x, y - 1, z)
			ballVertices[i].x = position.x - right.x - up.x;// * stepScale;
			ballVertices[i].y = position.y - right.y - up.y;// * stepScale;
			ballVertices[i].z = position.z - right.z - up.z;// * stepScale;
			ballVertices[i].u = 0.f;// * textureScale;
			ballVertices[i].v = 1.f;// * textureScale;
			ballVertices[i].color = GU_COLOR(myLight,myLight,myLight,1);
			i++;

			// (x + 1, y, z)
			ballVertices[i].x = position.x + right.x + up.x;// * stepScale;
			ballVertices[i].y = position.y + right.y + up.y;// * stepScale;
			ballVertices[i].z = position.z + right.z + up.z;// * stepScale;
			ballVertices[i].u = 1.f;// * textureScale;
			ballVertices[i].v = 0.f;// * textureScale;
			ballVertices[i].color = GU_COLOR(myLight,myLight,myLight,1);
			i++;

			// (x + 1, y - 1, z)
			ballVertices[i].x = position.x + right.x - up.x;// * stepScale;
			ballVertices[i].y = position.y + right.y - up.y;//* stepScale;
			ballVertices[i].z = position.z + right.z - up.z;// * stepScale;
			ballVertices[i].u = 1.f;// * textureScale;
			ballVertices[i].v = 1.f;// * textureScale;
			ballVertices[i].color = GU_COLOR(myLight,myLight,myLight,1);

			sceKernelDcacheWritebackInvalidateRange(ballVertices,4 * sizeof(CraftPSPVertex));
		}

		bool Arrow2::CheckCollision(CraftWorld* crtf)
		{
            if (crtf->BlockSolid(position.x+velocity.x*1.2f,position.y+velocity.y*1.2f,position.z+velocity.z*1.2f) == true)
            {
                return 1;
            }
            else
            {
                return 0;
            }
		}
		
		/*void Arrow2::Kick(CraftWorld* world)
		{
			myWorld = world;
			
			if(myWorld->mSpiders.empty() == false)
			{
				for(unsigned int j = 0; j < myWorld->mSpiders.size(); j++)
				{
					if(j < myWorld->mSpiders.size())
					{
						Spider* TestMob = myWorld->mSpiders[j];
						if(TestMob != NULL)
						{
							float dist = myWorld->FastDistance2d(abs(position.x-TestMob->position.x)*10,abs(position.z-TestMob->position.z)*10)/10.0f + abs(position.y-(TestMob->position.y-0.7))/2.0f;

							if(dist < 5.3f)
							{
								TestMob->TakeDamageForArrow(30-(dist*4.7),0.75,dt,position);
							}
						}
					}
				}
			}
			
		}*/

		void Arrow2::Render()
		{
			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_BLEND);

			sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF |GU_COLOR_8888| GU_VERTEX_32BITF | GU_TRANSFORM_3D, 4, 0, ballVertices);

            sceGuDisable(GU_BLEND);
			sceGuDisable(GU_TEXTURE_2D);
		}
	}
}

