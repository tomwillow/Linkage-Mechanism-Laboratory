#include "TOctree.h"

namespace TOctree
{

	// local function to create a new node in octree
	NODE* CreateNode(UINT nLevel, UINT nColorBits, UINT* pLeafCount,NODE** pReducibleNodes)
	{
		NODE* pNode;

		if ((pNode = (NODE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(NODE))) == NULL)
			return NULL;

		pNode->bIsLeaf = (nLevel == nColorBits) ? TRUE : FALSE;
		if (pNode->bIsLeaf)
			(*pLeafCount)++;
		else { // Add the node to the reducible list for this level
			pNode->pNext = pReducibleNodes[nLevel];
			pReducibleNodes[nLevel] = pNode;
		}
		return pNode;
	}

	// local function to add a color to octree
	void AddColor(NODE** ppNode, BYTE r, BYTE g, BYTE b, UINT nColorBits,
		UINT nLevel, UINT* pLeafCount, NODE** pReducibleNodes)
	{
		int nIndex, shift;
		static BYTE mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

		// If the node doesn't exist, create it
		if (*ppNode == NULL)
			*ppNode = CreateNode(nLevel, nColorBits, pLeafCount,
			pReducibleNodes);

		// Update color information if it's a leaf node
		if ((*ppNode)->bIsLeaf) {
			(*ppNode)->nPixelCount++;
			(*ppNode)->nRedSum += r;
			(*ppNode)->nGreenSum += g;
			(*ppNode)->nBlueSum += b;
		}

		// Recurse a level deeper if the node is not a leaf
		else {
			shift = 7 - nLevel;
			nIndex = (((r & mask[nLevel]) >> shift) << 2) |
				(((g & mask[nLevel]) >> shift) << 1) |
				((b & mask[nLevel]) >> shift);
			AddColor(&((*ppNode)->pChild[nIndex]), r, g, b, nColorBits,
				nLevel + 1, pLeafCount, pReducibleNodes);
		}
	}

	// local function to reduce the nodes of octree
	void ReduceTree(UINT nColorBits, UINT* pLeafCount, NODE** pReducibleNodes)
	{
		int i;
		NODE* pNode;
		UINT nRedSum, nGreenSum, nBlueSum, nChildren;

		// Find the deepest level containing at least one reducible node
		for (i = nColorBits - 1; (i > 0) && (pReducibleNodes[i] == NULL); i--);

		// Reduce the node most recently added to the list at level i
		pNode = pReducibleNodes[i];
		pReducibleNodes[i] = pNode->pNext;

		nRedSum = nGreenSum = nBlueSum = nChildren = 0;
		for (i = 0; i < 8; i++) {
			if (pNode->pChild[i] != NULL) {
				nRedSum += pNode->pChild[i]->nRedSum;
				nGreenSum += pNode->pChild[i]->nGreenSum;
				nBlueSum += pNode->pChild[i]->nBlueSum;
				pNode->nPixelCount += pNode->pChild[i]->nPixelCount;
				HeapFree(GetProcessHeap(), 0, pNode->pChild[i]);
				pNode->pChild[i] = NULL;
				nChildren++;
			}
		}

		pNode->bIsLeaf = TRUE;
		pNode->nRedSum = nRedSum;
		pNode->nGreenSum = nGreenSum;
		pNode->nBlueSum = nBlueSum;
		*pLeafCount -= (nChildren - 1);
	}

	// local function to delete a node of octree
	void DeleteTree(NODE** ppNode)
	{
		int i;

		for (i = 0; i < 8; i++) {
			if ((*ppNode)->pChild[i] != NULL)
				DeleteTree(&((*ppNode)->pChild[i]));
		}
		HeapFree(GetProcessHeap(), 0, *ppNode);
		*ppNode = NULL;
	}

	// local function to get color entry from a palette
	void GetPaletteColors(NODE* pTree, RGBQUAD* pPalEntries, UINT* pIndex)
	{
		int i;
		if (pTree->bIsLeaf)
		{
			pPalEntries[*pIndex].rgbRed = (BYTE)((pTree->nRedSum) / (pTree->nPixelCount));
			pPalEntries[*pIndex].rgbGreen = (BYTE)((pTree->nGreenSum) / (pTree->nPixelCount));
			pPalEntries[*pIndex].rgbBlue = (BYTE)((pTree->nBlueSum) / (pTree->nPixelCount));
			(*pIndex)++;
		}
		else
		{
			for (i = 0; i < 8; i++)
			{
				if (pTree->pChild[i] != NULL)
					GetPaletteColors(pTree->pChild[i], pPalEntries, pIndex);
			}
		}
	}
	// local function to get color entry from a palette
	void GetPaletteColors(NODE* pTree, unsigned char *&palette, UINT* pIndex)
	{
		if (pTree->bIsLeaf)
		{
			palette[*pIndex * 3] = (BYTE)((pTree->nRedSum) / (pTree->nPixelCount));//r
			palette[*pIndex * 3 + 1] = (BYTE)((pTree->nGreenSum) / (pTree->nPixelCount));//g
			palette[*pIndex * 3 + 2] = (BYTE)((pTree->nBlueSum) / (pTree->nPixelCount));//b
			(*pIndex)++;
		}
		else
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				if (pTree->pChild[i] != NULL)
					GetPaletteColors(pTree->pChild[i], palette, pIndex);
			}
		}
	}	
	
	void GetPaletteColors(NODE* pTree, std::vector<unsigned int> &palette, UINT* pIndex)
	{
		if (pTree->bIsLeaf)
		{
			BYTE r, g, b;
			r=(BYTE)((pTree->nRedSum) / (pTree->nPixelCount));//r
			g=(BYTE)((pTree->nGreenSum) / (pTree->nPixelCount));//g
			b=(BYTE)((pTree->nBlueSum) / (pTree->nPixelCount));//b
			palette.emplace_back((r << 16) | (g << 8) | (b));
			(*pIndex)++;
		}
		else
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				if (pTree->pChild[i] != NULL)
					GetPaletteColors(pTree->pChild[i], palette, pIndex);
			}
		}
	}
}