#pragma once
#include "model.h"
#include <vector>
#include <iostream>
#include <math.h>

#include <math.h>
#include <stdlib.h>

#define PERMUTATION_SIZE 256
#define EXTENDED_SIZE (PERMUTATION_SIZE * 2)
int perm[EXTENDED_SIZE];
bool generated = false;


float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

void shuffle(int* array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void generatePermutationTable() {
    for (int i = 0; i < PERMUTATION_SIZE; i++) {
        perm[i] = i;
    }

    shuffle(perm, PERMUTATION_SIZE);

    for (int i = 0; i < PERMUTATION_SIZE; i++) {
        perm[PERMUTATION_SIZE + i] = perm[i];
    }
}

float perlin(float x, float y) {
    int xi = (int)floor(x) & 255;
    int yi = (int)floor(y) & 255;
    float xf = x - floor(x);
    float yf = y - floor(y);
    float u = fade(xf);
    float v = fade(yf);

    int aa, ab, ba, bb;
    aa = perm[perm[xi] + yi];
    ab = perm[perm[xi] + yi + 1];
    ba = perm[perm[xi + 1] + yi];
    bb = perm[perm[xi + 1] + yi + 1];

    float x1, x2;
    x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

    return lerp(x1, x2, v)/8.f;
}

struct t_CaveChunk
{
	std::vector<t_VertexStruct> points;
    int stepX;
    int stepY;
};

t_VertexStruct& genVertex(std::vector<t_VertexStruct>& vector)
{
	t_VertexStruct& vertex = vector.emplace_back();
	//vertex.position.z = 0.0f;
	vertex.uv = { 0.0f, 1.0f };
	vertex.normal = { 0.0f, 0.0f, 1.0f };

	return vertex;
}

void generateCaveChunk(t_CaveChunk& chunk)
{
    if (!generated)
    {
        srand(time(NULL));
        generatePermutationTable();
        generated = true;
    }

    // 920

    int size = 128;
	for (float y = 0; y < size; y++)
	{
		for (float x = 0; x < size; x++)
		{
			t_Vec3 quad[6] = {
                {x, perlin(x,y + 1.0f), y + 1.0f}, {x + 1.0f, perlin(x+1.0f,y), y}, {x, perlin(x,y), y },
                {x, perlin(x,y + 1.0f), y + 1.0f}, {x + 1.0f, perlin(x+1.0f,y+1.0f), y + 1.0f}, {x + 1.0f, perlin(x + 1.0f,y), y}
			};

			for (int i = 0; i < 6; i++)
			{
                t_VertexStruct& strut = genVertex(chunk.points);
				strut.position = quad[i];
                strut.normal = { strut.position.y,strut.position.y,strut.position.y };
			}
		}
	}
}

/*

                    CUBE MARCHING
                    -------------

*/

void generateChunkSpace()
{

}

void cellMarch()
{
    t_Vec3 circle = { 32.0f, 32.0f, 32.0f };
    float radius = 32;
 
    int chunksize = 64;
    const int cubemarchoffest = chunksize / 2;
    
    for (int y = 0; y < cubemarchoffest; y++)
    {
        for (int z = 0; z < cubemarchoffest; z++)
        {
            for (int x = 0; x < cubemarchoffest; x++)
            {
                /*
                   *-------*
                   |\       \
                   | \       \
                   *  *-------*
                    \ |       |
                     \|       |
                      *-------*
                */
            }
        }
    }
}