// RussiaSquareDoc.cpp :  CRussiaSquareDoc 类的实现
//

#include "stdafx.h"
#include "RussiaSquare.h"
#include "RussiaSquareDoc.h"
#include "iostream"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRussiaSquareDoc

IMPLEMENT_DYNCREATE(CRussiaSquareDoc, CDocument)

BEGIN_MESSAGE_MAP(CRussiaSquareDoc, CDocument)
END_MESSAGE_MAP()


// CRussiaSquareDoc 构造/析构

CRussiaSquareDoc::CRussiaSquareDoc()
{
	m_nLevel = 0;
	m_nScore = 0;
	m_nMatrixWidth = 17;
	m_nMatrixHeight = 30;
	m_nBackgroundColor = RGB(255,255,255);
	m_nDeleteLines[0] = m_nDeleteLines[1] = m_nDeleteLines[2] = m_nDeleteLines[3] = -1;

	m_pColorGridMatrix = new COLORREF[m_nMatrixWidth*m_nMatrixHeight];
	m_pGridMatrixStatus = new GridStatus[m_nMatrixWidth*m_nMatrixHeight];
	m_pSquareBlock = new SquareBlock;
	m_pNextSquareBlock = new SquareBlock;
	//AllocConsole();                     // 打开控制台资源
	//freopen( "CONOUT$", "w+t", stdout );// 申请写

	
	OnNewDocument();
}

CRussiaSquareDoc::~CRussiaSquareDoc()
{
	delete []m_pColorGridMatrix;
	delete []m_pGridMatrixStatus;
	delete m_pSquareBlock;
	delete m_pNextSquareBlock;
	//FreeConsole();                      // 释放控制台资源
}

UINT CRussiaSquareDoc::GetMatrixWidth()
{
	return m_nMatrixWidth;
}

UINT CRussiaSquareDoc::GetMatrixHeight()
{
	return m_nMatrixHeight;
}

int CRussiaSquareDoc::GetGameLevel()
{
	return m_nLevel;
}

int CRussiaSquareDoc::GetGameScore()
{
	return m_nScore;
}

GameType CRussiaSquareDoc::GetGameType()
{
	return m_nGameType;
}

GameStatus CRussiaSquareDoc::GetGameStatus()
{
	return m_nGameStatus;
}

const SquareBlock* CRussiaSquareDoc::GetNextSquareBlock()
{
	return m_pNextSquareBlock;
}

COLORREF CRussiaSquareDoc::GetGridColor(UINT x,UINT y)
{
	if(x >= m_nMatrixWidth || y >= m_nMatrixHeight)
	{
		return 0;
	}
	return m_pColorGridMatrix[y*m_nMatrixWidth + x];
}

BOOL CRussiaSquareDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetGameType(GAME_RUSSIASQUARE);
	SetGameStatus(GAME_STOP);
	m_pNextSquareBlock->GenerateBlock(m_nMatrixWidth/2, 0);
	return TRUE;
}

void CRussiaSquareDoc::InitMatrixData(BOOL isStart)
{
	int matrixSize = m_nMatrixWidth*m_nMatrixHeight;
	if(isStart)
	{
		m_nLevel = 0;
		m_nScore = 0;
		for(int i = 0; i < matrixSize; i++)
		{
			m_pColorGridMatrix[i] = m_nBackgroundColor;
			m_pGridMatrixStatus[i] = GRID_EMPTY; 
		}
	}
	else
	{
		for(int i = 0; i < matrixSize; i++)
		{
			m_pColorGridMatrix[i] = RGB(0,0,0);
			m_pGridMatrixStatus[i] = GRID_FILL; 
		}
	}

}

void CRussiaSquareDoc::ClearSquareBlock()
{
	int matrixSize = m_nMatrixWidth*m_nMatrixHeight;
	for(int i = 0; i < matrixSize; i++)
	{
		if(GRID_EMPTY == m_pGridMatrixStatus[i])
		{
			m_pColorGridMatrix[i] = m_nBackgroundColor;
		}
	}
}

void CRussiaSquareDoc::SetGameType(GameType type)
{
	m_nGameType = type;
}

void CRussiaSquareDoc::SetGameStatus(GameStatus status)
{
	GameStatus lastStatus = m_nGameStatus;
	m_nGameStatus = status;
	switch(m_nGameStatus)
	{
	case GAME_START:
		InitMatrixData(TRUE);
		UpdateSquareBlock();
		break;
	case GAME_PAUSE:
		if(lastStatus == GAME_PAUSE)
		{
			m_nGameStatus = GAME_START;
		} 
		else if(lastStatus == GAME_STOP)
		{
			m_nGameStatus = lastStatus;
		}
		break;
	case GAME_STOP:
		InitMatrixData(FALSE);
		break;
	default:
		break;
	}
	UpdateAllViews(NULL,0,NULL);
}

void CRussiaSquareDoc::SetSquareBlockToMatrix()
{
	int x,y;

	for(int i = 0; i < 4; i++)
	{
		x = m_pSquareBlock->GetX(i);
		y = m_pSquareBlock->GetY(i);
		if(x >= 0 && y >= 0)
		{
			m_pColorGridMatrix[x + m_nMatrixWidth*y] 
			= m_pSquareBlock->GetBlockColor();
		}

	}
}

void CRussiaSquareDoc::SetSquareBlockMarkToMatrix()
{
	int x,y;
	for(int n = 0; n < 4; n++)
	{
		x = m_pSquareBlock->GetX(n);
		y = m_pSquareBlock->GetY(n);
		m_pGridMatrixStatus[m_nMatrixWidth*y + x] = GRID_FILL; 
	}

	BOOL isFullLine = TRUE;
	int deleteIndex = 0;
	m_nDeleteLines[0] = m_nDeleteLines[1] = m_nDeleteLines[2] = m_nDeleteLines[3] = -1;

	for(UINT j =  0; j < m_nMatrixHeight; j++)
	{
		isFullLine = TRUE;
		for(UINT i = 0; i < m_nMatrixWidth; i++)
		{
			if(m_pGridMatrixStatus[j*m_nMatrixWidth + i] != GRID_FILL)
			{
				isFullLine = FALSE;
				break;
			}
		}
		if(isFullLine)
		{
			m_nDeleteLines[deleteIndex] = j;
			deleteIndex++;
			if(deleteIndex > 3)
			{
				break;
			}
		}
	}
}

void CRussiaSquareDoc::UpdateGameScore(UINT deleteLines)
{
	m_nScore += (deleteLines*deleteLines)*50 + 15;
	while(m_nScore > (m_nLevel + 1)*250 + m_nLevel*m_nLevel*10)
	{
		m_nLevel++;
	}
}

void CRussiaSquareDoc::UpdateSquareBlock()
{
	//printf("UpdateSquareBlock...\n");    //TRACE
	*m_pSquareBlock = *m_pNextSquareBlock;
	m_pNextSquareBlock->GenerateBlock(m_nMatrixWidth/2, 0);
	if(!IsBlockInMatrix())
	{
		SetGameStatus(GAME_STOP);
		return;
	}
	SetSquareBlockToMatrix();
}

BOOL CRussiaSquareDoc::IsBlockInMatrix()
{
	int x,y;
	int index;
	int matrixSize = m_nMatrixWidth*m_nMatrixHeight;
	for(int i = 0; i < 4; i++)
	{
		x = m_pSquareBlock->GetX(i);
		if( x < 0 || x >= (int)m_nMatrixWidth)
			return FALSE;
		y = m_pSquareBlock->GetY(i);
		if( y >= (int)m_nMatrixHeight)
			return FALSE;
		index = m_nMatrixWidth*y + x;
		for(int i = 0; i < matrixSize; i++)
		{
			if(GRID_FILL == m_pGridMatrixStatus[index])
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CRussiaSquareDoc::TransferSquareBlock()
{
	m_pSquareBlock->TurnLeft();
	if(!IsBlockInMatrix())
	{
		m_pSquareBlock->TurnRight();
	}
	SetSquareBlockToMatrix();
}

int CRussiaSquareDoc::FallSquareBlock()
{
	m_pSquareBlock->FallDown();
	if(IsBlockInMatrix())
	{
		SetSquareBlockToMatrix();
		return -1;
	}
	else
	{
		m_pSquareBlock->GoUp();
		if(m_nDeleteLines[0] > -1)
		{
			return -1;
		}
		SetSquareBlockToMatrix();
		SetSquareBlockMarkToMatrix();
		int n = MarkDeleteLines(0);
		UpdateGameScore(n);
		return n;
	}
}

void CRussiaSquareDoc::LeftMoveSquareBlock()
{
	m_pSquareBlock->MoveLeft();
	if(!IsBlockInMatrix())
	{
		m_pSquareBlock->MoveRight();
	}
	SetSquareBlockToMatrix();
}

void CRussiaSquareDoc::RightMoveSquareBlock()
{
	m_pSquareBlock->MoveRight();
	if(!IsBlockInMatrix())
	{
		m_pSquareBlock->MoveLeft();
	}
	SetSquareBlockToMatrix();
}

int CRussiaSquareDoc::MarkDeleteLines(UINT index)
{
	int n;
	if(index >= m_nMatrixWidth)
	{
		return -5;
	}

	for(n = 0; n < 4; n++)
	{
		if(-1 == m_nDeleteLines[n])
		{
			return n;
		}

		m_pColorGridMatrix[m_nDeleteLines[n]*m_nMatrixWidth + index] 
		= RGB(255,255,255);
	}
	return n;
}

void CRussiaSquareDoc::DeleteLines()
{
	//printf("Delete Lines...\n");    //TRACE
	int n;
	for(n = 0; n < 4; n++)
	{
		if(-1 == m_nDeleteLines[n])
		{
			break;
		}

		for(int j = m_nDeleteLines[n]; j > 0; j--)
		{
			for(UINT i = 0; i < m_nMatrixWidth; i++)
			{
				m_pColorGridMatrix[j*m_nMatrixWidth + i] = m_pColorGridMatrix[(j - 1)*m_nMatrixWidth + i];
				m_pGridMatrixStatus[j*m_nMatrixWidth + i] = m_pGridMatrixStatus[(j - 1)*m_nMatrixWidth + i];
			}

		}
		for(UINT i = 0; i < m_nMatrixWidth; i++)
		{
			m_pColorGridMatrix[i] = m_nBackgroundColor;
			m_pGridMatrixStatus[i] = GRID_EMPTY;
		}
		m_nDeleteLines[n] = -1;
	}
	UpdateSquareBlock();
}

// CRussiaSquareDoc 序列化

void CRussiaSquareDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CRussiaSquareDoc 诊断

#ifdef _DEBUG
void CRussiaSquareDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRussiaSquareDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRussiaSquareDoc 命令
