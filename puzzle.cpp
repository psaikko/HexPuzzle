#include <bits/stdc++.h>
#include "pieces.h"

using namespace std;

#define N 11

#define Point pair<int,int>
#define X first
#define Y second

#define GRID_SIZE 5

vector< vector< vector< uint64_t > > > pieceOrientationBits;

namespace std
{
    template<> struct hash<Point>
    {
        typedef Point argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& p) const
        {
            return (N+1)*p.X + p.Y;
        }
    };
}

unordered_map<Point, unsigned> pointToIndex;
unordered_map<unsigned, Point> indexToPoint;

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

bool bitsHasHex(uint64_t bits, Point p) {
	return bits & (1ul << pointToIndex[p]);
}

uint64_t bitsSetHex(uint64_t bits, Point p) {
	return bits | (1ul << pointToIndex[p]);
}

void printBits(uint64_t bits) {
	uint64_t i = 1;
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

// place piece on grid (does not check validity)
uint64_t makePieceOrientationMask(Piece piece, Rot o, Point p) {

	uint64_t newGrid = 0;

	deque<Point> piecePtQ;
	deque<Point> gridPtQ;

	newGrid = bitsSetHex(newGrid, p);

	piecePtQ.push_back({1,4});
	gridPtQ.push_back(p);

	// (absolute) grid directions
	vector<Point (*)(Point)> directions = {*R, *UR, *UL, *L, *DL, *DR};

	// (relative) piece directions
	vector<Point (*)(Point)> pieceDirections = {*o.r, *o.ur, *o.ul, *o.l, *o.dl, *o.dr};

	// do bfs to place piece on grid
	while (piecePtQ.size()) {
		Point piecePt = piecePtQ.front();
		Point gridPt = gridPtQ.front();

		piecePtQ.pop_front(); gridPtQ.pop_front();

		for (unsigned i = 0; i < directions.size(); ++i) {
			auto pieceDir = pieceDirections[i];
			auto gridDir  = directions[i];

			Point newGridPt = gridDir(gridPt);
			Point newPiecePt = pieceDir(piecePt);

			if (!onPieceGrid(newPiecePt)) continue;
			if (!piece[newPiecePt.Y][newPiecePt.X]) continue;
			if (!onGrid(newGridPt)) return 0;
			if (bitsHasHex(newGrid, newGridPt)) continue;

			piecePtQ.push_back(newPiecePt);
			gridPtQ.push_back(newGridPt);

			newGrid = bitsSetHex(newGrid, newGridPt);
		}
	}

	return newGrid;
}


bool canCoverHexes_bits(bitset<10> usedPieces, uint64_t grid) {
	uint64_t scratch = grid;

	for (unsigned i = 0; i < allPieces.size(); ++i) {
		if (usedPieces[i]) continue;

		bool canPlace = false;

		for (unsigned j = 0; j < allOrientations.size(); ++j) {

			for (uint64_t pieceBits : pieceOrientationBits[i][j]) {

				if ((pieceBits & grid) == 0) {
					scratch |= pieceBits;
					canPlace = true;
				}
			}
		}

		if (!canPlace) return false;
	}


	// does scratch grid have uncovered hexes?
	if (bitset<61>(scratch).count() != 61) {
		return false;
	}

	return true;
}

bool forcedHexes(uint64_t &grid, bitset<10> usedPieces) {
	// find hexes that can only be covered by one piece in one orientation

	vector<uint64_t> pieceCoverage;

	for (unsigned i = 0; i < allPieces.size(); ++i) {
		if (usedPieces[i]) {
			pieceCoverage.push_back(0);
			continue;
		}

		uint64_t coverage = 0;

		for (unsigned j = 0; j < allOrientations.size(); ++j) {

			for (uint64_t pieceBits : pieceOrientationBits[i][j]) {

				if ((pieceBits & grid) == 0) {
					coverage |= pieceBits;
				}
			}
		}

		pieceCoverage.push_back(coverage);
	}

	for (unsigned i = 0; i < pieceCoverage.size(); ++i) {
		uint64_t coverage = pieceCoverage[i];
		uint64_t otherCoverage = 0;
		for (unsigned j = 0; j < pieceCoverage.size(); ++j) {
			if (i != j) {
				otherCoverage |= pieceCoverage[j];
			}
		}


		uint64_t uniq = coverage & ~otherCoverage;

		if (uniq) {
			int choices = 0;
			uint64_t foo;

			for (unsigned j = 0; j < allOrientations.size(); ++j) {

				for (uint64_t pieceBits : pieceOrientationBits[i][j]) {

					if ((pieceBits & uniq) && !(pieceBits & grid)) {
						foo = pieceBits;
						++choices;
					}
				}
			}

			if (choices == 1) {
				grid |= foo;
				usedPieces[i] = 1;
				return true;
			}
		}
	}
	return false;
}

int tries;

bool search(uint64_t gridBits, bitset<10> usedPieces) {
	// for each empty position on the grid (ignoring the filled edge hexes):

	++tries;

	for (uint64_t m = 1; m < (uint64_t(1) << 62); m <<= 1) {

		if ((gridBits & m) == 0) {

			// try each piece
			for (unsigned i = 0; i < allPieces.size(); ++i) {
				if (usedPieces[i]) continue;

				// in each orientation
				for (unsigned j = 0; j < allOrientations.size(); ++j) {

					// if in can be placed, add resulting Grid to the stack

					for (uint64_t pieceBits : pieceOrientationBits[i][j]) {

						if ((pieceBits & m) == 0) continue;

						if ((gridBits & pieceBits) == 0) {
							uint64_t newGridBits = gridBits | pieceBits;

							bitset<10> remainingPieces(usedPieces);
							remainingPieces[i] = 1;
							/*
							do {
								if (!canCoverHexes_bits(remainingPieces, newGridBits)) 
									goto skip;
							}
							while (forcedHexes(newGridBits, remainingPieces));
							*/
							//printBits(newGridBits);

							if (!canCoverHexes_bits(remainingPieces, newGridBits)) {
								continue;
							}

							// all pieces placed?
							if (remainingPieces.count() == 10) {
								return true;
							}

							if (search(newGridBits, remainingPieces)) {
								printBits(newGridBits ^ gridBits);
								return true;
							}
						}	
					}
				}
			}
			return false;
		}
	}
	return false;
}

int main() {

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

	unsigned count = 0;

	for (unsigned i = 0; i < allPieces.size(); ++i) {
		pieceOrientationBits.emplace_back();
		for (unsigned j = 0; j < allOrientations.size(); ++j) {
			pieceOrientationBits[i].emplace_back();

			for (int y = 1; y < 2*GRID_SIZE; ++y) { 
				for (int x = max(1, y - (GRID_SIZE - 1)); 
					 x < min(2*GRID_SIZE, y + GRID_SIZE);
					 x++) {

					Point p = {x, y};

					uint64_t bits = makePieceOrientationMask(allPieces[i], allOrientations[j], p);

					if (bits) {
						pieceOrientationBits[i][j].push_back(bits);
						++count;						
					}
			 	}
			}
		}
	}
	cout << count << endl;

	bitset<10> noPieces(0);
	search(0, noPieces);

	cout << tries << endl;
}