#pragma once
#include <Windows.h>
#include <vector>

namespace TOctree
{
	typedef struct _NODE
	{
		BOOL bIsLeaf;               // TRUE if node has no children
		UINT nPixelCount;           // Number of pixels represented by this leaf
		UINT nRedSum;               // Sum of red components
		UINT nGreenSum;             // Sum of green components
		UINT nBlueSum;              // Sum of blue components
		struct _NODE* pChild[8];    // Pointers to child nodes
		struct _NODE* pNext;        // Pointer to next reducible node
	} NODE;

	// local function to create a new node in octree
	NODE* CreateNode(UINT nLevel, UINT nColorBits, UINT* pLeafCount, NODE** pReducibleNodes);

	// local function to add a color to octree
	void AddColor(NODE** ppNode, BYTE r, BYTE g, BYTE b, UINT nColorBits,
		UINT nLevel, UINT* pLeafCount, NODE** pReducibleNodes);

	// local function to reduce the nodes of octree
	void ReduceTree(UINT nColorBits, UINT* pLeafCount, NODE** pReducibleNodes);

	// local function to delete a node of octree
	void DeleteTree(NODE** ppNode);

	// local function to get color entry from a palette
	void GetPaletteColors(NODE* pTree, RGBQUAD* pPalEntries, UINT* pIndex);

	// local function to get color entry from a palette
	void GetPaletteColors(NODE* pTree, unsigned char *&palette, UINT* pIndex);

	void GetPaletteColors(NODE* pTree, std::vector<unsigned int> &palette, UINT* pIndex);
}