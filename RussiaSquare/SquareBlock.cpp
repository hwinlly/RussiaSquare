#include "StdAfx.h"
#include "squareblock.h"

SquareBlock::SquareBlock(void)
{
    m_x[0] = m_y[0] = 0;
	m_x[1] = m_y[1] = 0;
	m_x[2] = m_y[2] = 0;
	m_x[3] = m_y[3] = 0;
	m_blockColor = RGB(0,255,255);
}

SquareBlock::~SquareBlock(void)
{
}

int SquareBlock::GetX(int index) const
{
	if(index < 0 || index >= 4)
	{
		return 0;
	}
	return m_x[index];
}

int SquareBlock::GetY(int index) const
{
	if(index < 0 || index >= 4)
	{
		return 0;
	}
	return m_y[index];
}

int SquareBlock::GetCenterX() const
{
	return m_xcenter;
}

int SquareBlock::GetCenterY() const
{
	return m_ycenter;
}

COLORREF SquareBlock::GetBlockColor() const
{
	return m_blockColor;
}

void SquareBlock::FallDown()
{
	m_y[0]++;
	m_y[1]++;
	m_y[2]++;
	m_y[3]++;
	m_ycenter++;
}

void SquareBlock::GoUp()
{
	m_y[0]--;
	m_y[1]--;
	m_y[2]--;
	m_y[3]--;
	m_ycenter--;
}

void SquareBlock::MoveLeft()
{
	m_x[0]--;
	m_x[1]--;
	m_x[2]--;
	m_x[3]--;
	m_xcenter--;
}

void SquareBlock::MoveRight()
{
	m_x[0]++;
	m_x[1]++;
	m_x[2]++;
	m_x[3]++;
	m_xcenter++;
}

void SquareBlock::TurnLeft()
{
	int temp;
	int oldyCenter = m_ycenter;

	for(int i = 0; i < 4; i++)
	{
		m_x[i] -= m_xcenter;
		m_y[i] -= m_ycenter;
		temp = m_x[i];
		m_x[i] = m_y[i];
		m_y[i] = -temp;
		m_x[i] += m_xcenter;
		m_y[i] += m_ycenter;
	}
	m_xcenter = m_ycenter = 0;

	for(int i = 0; i < 4; i++)
	{
		m_xcenter += m_x[i];
		m_ycenter += m_y[i];
	}

	m_xcenter = (m_xcenter + 3)/4;
	m_ycenter = (m_ycenter + 3)/4;
	if(oldyCenter < m_ycenter)
	{
		GoUp();
	}
}

void SquareBlock::TurnRight()
{
	int temp;
	int oldxCenter = m_xcenter;

	for(int i = 0; i < 4; i++)
	{
		m_x[i] -= m_xcenter;
		m_y[i] -= m_ycenter;
		temp = m_x[i];
		m_x[i] = -m_y[i];
		m_y[i] = temp;
		m_x[i] += m_xcenter;
		m_y[i] += m_ycenter;
		
	}
	m_xcenter = m_ycenter = 0;

	for(int i = 0; i < 4; i++)
	{
		m_xcenter += m_x[i];
		m_ycenter += m_y[i];
	}
	
	m_xcenter = (m_xcenter + 3)/4;
	m_ycenter = (m_ycenter + 3)/4;
	if(oldxCenter < m_xcenter)
	{
		MoveLeft();
	}
}

#define VALID_POSITION_COUNT 8

int SquareBlock::AddValidPositions(LOCATION* validPositions, int index)
{
	int px[4];
	int py[4];
	int count = 0;

	px[0] = m_x[index] + 1;
	py[0] = m_y[index];
	px[1] = m_x[index] - 1;
	py[1] = m_y[index];
	px[2] = m_x[index];
	py[2] = m_y[index] + 1;
	px[3] = m_x[index];
	py[3] = m_y[index] - 1;

	for(int i = 0; i < 4; i++)
	{
		if(IsValidPosition(validPositions,index,px[i],py[i]))
		{
			if(AddToValidPosition(validPositions,px[i],py[i]))
			{
				count++;
			}
		}
	}
	return count;
}

BOOL SquareBlock::AddToValidPosition(LOCATION* validPositions,int x,int y)
{
	for(int i = 0; i < VALID_POSITION_COUNT; i++)
	{
		if(!validPositions[i].valid)
		{
			validPositions[i].x = x;
			validPositions[i].y = y;
			validPositions[i].valid = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SquareBlock::RemoveFromValidPosition(LOCATION* validPositions, int x, int y)
{
	for(int i = 0; i < VALID_POSITION_COUNT; i++)
	{
		if(validPositions[i].valid)
		{
			if(validPositions[i].x == x&&validPositions[i].y == y)
			{
				for(int j = i + 1; j < VALID_POSITION_COUNT; j++)
				{
					if(validPositions[j].valid)
					{
						validPositions[j - 1] = validPositions[j];
					}
					else
					{
						validPositions[j - 1].valid = FALSE;
						return TRUE;
					}
				}
			}
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL SquareBlock::IsValidPosition(LOCATION* validPositions,int index,int x,int y)
{
	for(int i = 0; i <= index; i++)
	{
		if(x == m_x[i]&&y == m_y[i])
		{
			return FALSE;
		}
	}
	for(int j = 0; j < VALID_POSITION_COUNT; j++)
	{
		if(validPositions[j].valid)
		{
			if(x == validPositions[j].x && y == validPositions[j].y)
			{
				return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return TRUE;
}

void SquareBlock::GenerateBlock(int x0,int y0)
{
	m_xcenter = m_x[0] = x0;
	m_ycenter = m_y[0] = y0;
	int locationIndex;
	
	static bool isFirstIn = true;
	if(isFirstIn)
	{
		srand( (unsigned)time( NULL ) ); // 只在第一次运行时初始化随机数种子
		isFirstIn = false;
	}

	LOCATION validPositions[VALID_POSITION_COUNT];
	memset(validPositions,0,sizeof(validPositions));
	int locationcount = AddValidPositions(validPositions, 0);

	m_blockColor = RGB(rand()%180,rand()%180,rand()%180);

	for(int i = 1; i < 4; i++)
	{
		locationIndex = rand()%locationcount;

		m_x[i] = validPositions[locationIndex].x;
		m_y[i] = validPositions[locationIndex].y;
		RemoveFromValidPosition(validPositions,m_x[i],m_y[i]);
		locationcount--;
		locationcount += AddValidPositions(validPositions, i);

		m_xcenter += m_x[i];
		m_ycenter += m_y[i];
	}
	m_xcenter = (m_xcenter + 3)/4;
	m_ycenter = (m_ycenter + 3)/4;
}