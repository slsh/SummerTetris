#include <stdlib.h>
#include <iostream>

// Konstanter för pjäserna
enum PieceID{
	G_PIECE_I,
	G_PIECE_O,
	G_PIECE_T,
	G_PIECE_S,
	G_PIECE_Z,
	G_PIECE_L,
	G_PIECE_IL,
	G_PIECE_COUNT
};

//Konstater för directions
enum Direction{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ROTATE
};


//struct för pjäser
struct Pieces{
	PieceID pieceName;
	Direction direction;
	int posX;
	int posY;
	int rotation;
	
	int thisPiece[4][4];
	int nextPiece[4][4];

	int leftEdge;
	int rightEdge;
};

//Prototyper
Pieces* createPiece();
void destroyPiece(Pieces* p);

int updatePiece(Pieces* p, int rotation);
void rotatePiece(Pieces* p);
bool rotationVerifier(int rotation);
PieceID randomisePiece();
int randomiseRotation();

void testPiece();
void printPiece(Pieces* p);