#pragma once

#include "SquareBlock.h"

typedef enum {
	
	GRID_EMPTY,GRID_FILL

} GridStatus;

typedef enum {

	GAME_STOP,GAME_START,GAME_PAUSE

} GameStatus;

typedef enum {

	GAME_RUSSIASQUARE,GAME_GRIDSNAKE,

} GameType;

class CRussiaSquareDoc : public CDocument
{
protected: // �������л�����
	CRussiaSquareDoc();
	DECLARE_DYNCREATE(CRussiaSquareDoc)

// ����
private:
	int m_nLevel;
	int m_nScore;
	UINT m_nMatrixWidth;
	UINT m_nMatrixHeight;
	int m_nDeleteLines[4];
	GameType m_nGameType;
	GameStatus m_nGameStatus;
	COLORREF m_nBackgroundColor;
	COLORREF *m_pColorGridMatrix;
	GridStatus *m_pGridMatrixStatus;
	SquareBlock *m_pSquareBlock;
	SquareBlock *m_pNextSquareBlock;

// ����
public:
	int GetGameLevel();
	int GetGameScore();
	GameType   GetGameType();
	GameStatus GetGameStatus();
	UINT GetMatrixWidth();
	UINT GetMatrixHeight();
	COLORREF GetGridColor(UINT x,UINT y);
	const SquareBlock* GetNextSquareBlock();
	void SetGameType(GameType type);
	void SetGameStatus(GameStatus status);

	void ClearSquareBlock();

	void TransferSquareBlock();
	int FallSquareBlock();
	void LeftMoveSquareBlock();
	void RightMoveSquareBlock();

	void DeleteLines();
	int MarkDeleteLines(UINT index);
	

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CRussiaSquareDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void SetSquareBlockToMatrix();
	BOOL IsBlockInMatrix();
	void SetSquareBlockMarkToMatrix();
	void InitMatrixData(BOOL isStart);
	void UpdateSquareBlock();
	void UpdateGameScore(UINT deleteLines);

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


