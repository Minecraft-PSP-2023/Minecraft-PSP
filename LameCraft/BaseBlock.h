#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <Aurora/Graphics/Vertex.h>
#include <Aurora/Math/BoundingBox.h>
#include <stdlib.h>
#include <string>
#include <Aurora/Graphics/RenderManager.h>
#include <pspmath.h>
#include <psptypes.h>

using namespace Aurora::Graphics;

typedef unsigned char block_t;

class BaseBlock
{
public:
	//���������� � �������� �����

	char upPlane_x, upPlane_y;
	char downPlane_x, downPlane_y;
	char leftPlane_x, leftPlane_y;
	char rightPlane_x, rightPlane_y;
    char botPlane_x, botPlane_y;
    char forPlane_x, forPlane_y;

	TexturesPSPVertex *verticesLeftRight;
	char pointsLeftRight;

    TexturesPSPVertex *verticesBackFront;
	char pointsBackFront;

    TexturesPSPVertex *verticesBottomTop;
	char pointsBottomTop;

	// ������ ���������
	bool editable;              // ����������� ����? true false
	bool transparent;           // ���������� ����? true false
	bool lightSource;           // ��������? true false
	bool update;                // ����������� ����? true false
	bool solid;                 // ������� ����? true false
	bool animated;
	bool inflamable;
	bool allowLight;            // �������� �� ���� ������������ ��� ��������������� �����? true - ��� ; false - ��

    short furnItem;               // �������, ���������� ��� ���������� ����� (��������� id)
    short loot;                   // ��� ����� (��������� id)

    char material;              // �������� ����� (1 - ������, 2 - ������, 3 - �����, 4 - ������, 5 - ��������, 6 - ������, 7 - ����, 8 - ������� ������, 9 - ������� ������, 10 - �������)
    //char points;              // ���������� ����� � ������ (36 - � ����� �.� 6 ������, �� ������ �� 2 ������������ = 12, � ������ ������������ �� 3 �����, 36 �����)
	char soundType;             // ����, ����� ���������� �� ���� (0 - ���� �����, 1 - ���� �����\������, 2 - ���� �����, 3 - ���� ������, 5 - ���� �����, 6 - ���� �����)
    char blockModel;            // ������ ����� (0 - ����������� ���,
                                       //        1 - ��������,
                                       //        2 - �����,
                                       //        3 - ��������� ������(������, �������),
                                       //        4 - ������ �������,
                                       //        5 - ������ ����, ������ �������� �� ������ ����� � CraftWorld2.cpp,
                                       //        6 - ��������� z++
                                       //        7 - ��������� z--
                                       //        8 - ��������� x++
                                       //        9 - ��������� x--
                                       //        10 - ��������� ����)
									   
    char refraction;
    BoundingBox collideBox;

    std::string name;

    virtual bool MatchWithRecipe2x2(std::string recipe_string[2], int& return_id, int& return_am, bool& return_st); // may be in future
    virtual bool MatchWithRecipe3x3(std::string recipe_string[3], int& return_id, int& return_am, bool& return_st);

	BaseBlock();
	virtual ~BaseBlock();

	static block_t getBaseID();
};

#endif
