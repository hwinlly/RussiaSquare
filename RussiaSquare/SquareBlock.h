#pragma once

typedef enum {
	BLOCK_UP,
	BLOCK_LEFT,
	BLOCK_DOWN,
	BLOCK_RIGHT
} BlockDirection;

typedef struct
{
	int  x;
	int  y;
	BOOL valid;
} LOCATION;

class SquareBlock
{
private:
	int m_x[4];
	int m_y[4];
	int m_xcenter;
	int m_ycenter;
    COLORREF m_blockColor;

public:
	SquareBlock(void);
	~SquareBlock(void);
public:
	int GetX(int index) const;
	int GetY(int index) const;
	int GetCenterX() const;
	int GetCenterY() const;
	COLORREF GetBlockColor() const;
	void GenerateBlock(int x0,int y0);
	void TurnLeft();
	void TurnRight();
	void FallDown();
	void GoUp();
	void MoveLeft();
	void MoveRight();
private:
	int AddValidPositions(LOCATION* validPositions, int index);
	BOOL AddToValidPosition(LOCATION* validPositions,int x,int y);
	BOOL RemoveFromValidPosition(LOCATION* validPositions, int x, int y);
	BOOL IsValidPosition(LOCATION* validPositions,int index,int x,int y);
};
