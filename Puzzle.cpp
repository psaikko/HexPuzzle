#include <QtCore>
#include <QApplication>
#include <QWidget>

#include <bits/stdc++.h>
#include "Pieces.h"
#include "SolutionWindow.h"

using namespace std;

// {x, y} coordinate pairs
typedef pair<int,int> Point;
#define X first
#define Y second

// length of one side of hex grid
#define GRID_SIZE 5

// hash function for points
namespace std
{
    template<> struct hash<Point>
    {
        typedef Point argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& p) const
        {
            return (2*GRID_SIZE - 1)*p.X + p.Y;
        }
    };
}

// Grid contains 61 hexes, so a state can be represented as
// a 64-bit integer
typedef uint64_t GridBits;

// precomputed "piece x orientation x point" bitmasks
vector< vector< vector< GridBits > > > pieceOrientationBits;

// precomputed mapping of coordinate pairs to bit-indices
unordered_map<Point, unsigned> pointToIndex;
unordered_map<unsigned, Point> indexToPoint;

// direction functions on hex grid coordinate pairs
Point L(Point p)  { return {p.X - 1, p.Y}; }
Point R(Point p)  { return {p.X + 1, p.Y}; }
Point UL(Point p) { return {p.X - 1, p.Y - 1}; }
Point UR(Point p) { return {p.X , p.Y - 1}; }
Point DL(Point p) { return {p.X + 1, p.Y + 1}; }
Point DR(Point p) { return {p.X, p.Y + 1}; }

// struct containing an orientation
struct Rot {
	Point (*r)(Point);
	Point (*ur)(Point);
	Point (*ul)(Point);
	Point (*l)(Point);
	Point (*dl)(Point);
	Point (*dr)(Point);
};

// turn an orientation 60 degrees ccw
Rot turn(Rot r) {
	Rot o;

	o.l = r.dr;
	o.ul = r.l;
	o.ur = r.ul;
	o.r = r.ur;
	o.dr = r.dl;
	o.dl = r.r;

	return o;
}

// flip an orientation around x-axis
Rot flip(Rot r) {
	Rot o;

	o.l  = r.l;
	o.ul = r.dr;
	o.ur = r.dl;
	o.r  = r.r;
	o.dr = r.ul;
	o.dl = r.ur;

	return o;
}

// create base orientation:
// piece in same orientation as grid
Rot base = { *R, *UR, *UL, *L, *DL, *DR };

// initialize all possible rotations and flips 
// of piece relative to grid
vector<Rot> allOrientations = {
	base,
	flip(base),
	turn(base),
	flip(turn(base)),
	turn(turn(base)),
	flip(turn(turn(base))),
	turn(turn(turn(base))),
	flip(turn(turn(turn(base)))),
	turn(turn(turn(turn(base)))),
	flip(turn(turn(turn(turn(base))))),
	turn(turn(turn(turn(turn(base))))),
	flip(turn(turn(turn(turn(turn(base))))))
};

// check / set if a bit is filled at a coordinate
bool bitsHasHex(GridBits bits, Point p) {
	return bits & (1ul << pointToIndex[p]);
}

GridBits bitsSetHex(GridBits bits, Point p) {
	return bits | (1ul << pointToIndex[p]);
}

void printBits(GridBits bits) {
	GridBits i = 1;
	for (int y = 1; y < 2*GRID_SIZE; ++y) {
		for (int j = 0; j < y - GRID_SIZE; ++j)
			cout << " ";
		for (int j = 0; j < GRID_SIZE - y; ++j)
			cout << " ";
		for (int x = max(1, y - (GRID_SIZE - 1)); 
			 x < min(2*GRID_SIZE, y + GRID_SIZE);
			 x++) {
			if (bits & i)
				cout << "X ";
			else
				cout << "_ ";
			i <<= 1;
		}
		cout << endl;
	}
	cout << endl;
}

// check bounds for piece grid coordinate
bool onPieceGrid(Point p) {
	return p.X >= 0 && p.Y >= 0 && p.X < M && p.Y < M;
}

// check bounds for grid coordinate
bool onGrid(Point p) {
	return pointToIndex.count(p);
}

// place piece on empty grid
GridBits makePieceOrientationMask(Piece piece, Rot o, Point p) {

	GridBits newGrid = 0;

	deque<Point> piecePtQ;
	deque<Point> gridPtQ;

	// set first hex on grid (always valid)
	newGrid = bitsSetHex(newGrid, p);

	// convention: all piece grids are filled at {1, 4}
	piecePtQ.push_back({1,4});
	gridPtQ.push_back(p);

	// (absolute) grid directions
	vector<Point (*)(Point)> directions = {*R, *UR, *UL, *L, *DL, *DR};

	// (relative) piece directions
	vector<Point (*)(Point)> pieceDirections = {*o.r, *o.ur, *o.ul, *o.l, *o.dl, *o.dr};

	// do bfs to place piece on grid
	while (piecePtQ.size()) {
		// take previously placed piece from stack
		Point piecePt = piecePtQ.front();
		Point gridPt = gridPtQ.front();

		piecePtQ.pop_front(); gridPtQ.pop_front();

		// check adjacent hexes
		for (unsigned i = 0; i < directions.size(); ++i) {
			auto pieceDir = pieceDirections[i];
			auto gridDir  = directions[i];

			Point newGridPt = gridDir(gridPt);
			Point newPiecePt = pieceDir(piecePt);

			// does the piece extend here?
			if (!onPieceGrid(newPiecePt)) continue;
			if (!piece[newPiecePt.Y][newPiecePt.X]) continue;

			// can the hex be placed on the grid?
			if (!onGrid(newGridPt)) return 0;
			if (bitsHasHex(newGrid, newGridPt)) continue;

			// push the piece to the stack
			piecePtQ.push_back(newPiecePt);
			gridPtQ.push_back(newGridPt);

			// update resulting grid
			newGrid = bitsSetHex(newGrid, newGridPt);
		}
	}

	return newGrid;
}


bool canCoverHexes(bitset<10> usedPieces, GridBits grid) {
	GridBits scratch = grid;

	// for each unused piece 
	for (unsigned i = 0; i < allPieces.size(); ++i) {
		if (usedPieces[i]) continue;

		bool canPlace = false;

		// try to place the piece in any hex, in any valid orientation
		for (unsigned j = 0; j < allOrientations.size(); ++j) {
			for (GridBits pieceBits : pieceOrientationBits[i][j]) {

				if ((pieceBits & grid) == 0) {
					// keep track of hexes that can be covered in some way
					scratch |= pieceBits;
					canPlace = true;
				}
			}
		}

		// if piece can't be placed anywhere, fail
		if (!canPlace) return false;
	}

	// are there hexes that could not be covered by any piece?
	if (bitset<61>(scratch).count() != 61) {
		return false;
	}

	return true;
}

bool search(GridBits gridBits, bitset<10> &usedPieces, vector<GridBits>& solution) {

	// for each position on the grid, starting from top left
	for (GridBits m = 1; m < (GridBits(1) << 62); m <<= 1) {

		// if the position is empty
		if ((gridBits & m) == 0) {

			// try each piece
			for (unsigned i = 0; i < allPieces.size(); ++i) {

				// skipping used pieces
				if (usedPieces[i]) continue;

				// in each orientation
				for (unsigned j = 0; j < allOrientations.size(); ++j) {

					// in each location
					for (GridBits pieceBits : pieceOrientationBits[i][j]) {

						// does the piece fill the hex we want to cover next?
						if ((pieceBits & m) == 0) 
							continue;

						// does the piece overlap with the grid?
						if ((gridBits & pieceBits) != 0) 
							continue;

						// create an updated grid and piece bitset
						GridBits newGridBits = gridBits | pieceBits;
						bitset<10> remainingPieces(usedPieces);
						remainingPieces[i] = 1;

						// can each remaining hex be covered by some piece?
						if (!canCoverHexes(remainingPieces, newGridBits))
							continue;

						// continue search until all pieces are placed
						if (remainingPieces.count() == 10 || 
							search(newGridBits, remainingPieces, solution)) {

							// solution was found, add state to solution vector
							solution.push_back(newGridBits);
							//printBits(newGridBits ^ gridBits);
							return true;
						}
					}
				}
			}
			return false;
		}
	}
	return false;
}

int main(int argc, char ** argv) {

	// precompute mapping of {x,y} coordinates to bit indices
	unsigned index = 0;
	for (int y = 1; y < 2*GRID_SIZE; ++y) {
		for (int x = max(1, y - (GRID_SIZE - 1)); 
			 x < min(2*GRID_SIZE, y + GRID_SIZE);
			 x++) 
		{
			Point p = {x, y};
			pointToIndex[p] = index;
			indexToPoint[index] = p;
			index++;
		}
	}

	// preprocess grids with each single piece
	// in each possible orientation 
	// in each possible location
	for (unsigned i = 0; i < allPieces.size(); ++i) {
		pieceOrientationBits.emplace_back();
		for (unsigned j = 0; j < allOrientations.size(); ++j) {
			pieceOrientationBits[i].emplace_back();

			for (int y = 1; y < 2*GRID_SIZE; ++y) { 
				for (int x = max(1, y - (GRID_SIZE - 1)); 
					 x < min(2*GRID_SIZE, y + GRID_SIZE);
					 x++) {

					Point p = {x, y};

					GridBits bits = makePieceOrientationMask(allPieces[i], allOrientations[j], p);

					if (bits) {
						pieceOrientationBits[i][j].push_back(bits);
					}
			 	}
			}
		}
	}


	bitset<10> noPieces(0);
	vector<GridBits> solution;
	search(0, noPieces, solution);

	// Solution is reversed, add the initial empty state and reverse it
	solution.push_back(0);
	reverse(solution.begin(), solution.end()); 

	QApplication app(argc, argv);	

    SolutionWindow window(solution);

    window.resize(500, 520);
    window.setWindowTitle("Hex Puzzle Solver");
    window.show();

    return app.exec();
}