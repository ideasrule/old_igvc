#pragma once


typedef struct Lattice {
	const static int n = 16;    //direction-16
	const static int dx[n];
	const static int dy[n];

	const static int update_n = 16;
	const static int update_dx[update_n];
	const static int update_dy[update_n];
} Lattice;

//just for the hell of it direction-4 search
//const int Lattice::dx[n] = {1, 0, -1, 0};  
//const int Lattice::dy[n] = {0, -1, 0, 1};
//
//const int Lattice::update_dx[update_n] = {1, 0, -1, 0}; 
//const int Lattice::update_dy[update_n] = {0, -1, 0, 1};


////direction-8 search (clockwise)
//const int Lattice::dx[n] = {1, 1, 0, -1, -1, -1, 0, 1};  
//const int Lattice::dy[n] = {0, 1, 1, 1, 0, -1, -1, -1};
//
//const int Lattice::update_dx[update_n] = {1, 1, 0, -1, -1, -1, 0, 1};  
//const int Lattice::update_dy[update_n] = {0, 1, 1, 1, 0, -1, -1, -1};


//direction 16 
const int Lattice::dx[n] = {2, 1, -1, -2, -2, -1, 1, 2, 1, 1, 0, -1, -1, -1, 0, 1};
const int Lattice::dy[n] = {-1, -2, -2, -1, 1, 2, 2, 1, 0, 1, 1, 1, 0, -1, -1, -1};

const int Lattice::update_dx[n] = {2, 1, -1, -2, -2, -1, 1, 2, 1, 1, 0, -1, -1, -1, 0, 1};
const int Lattice::update_dy[n] = {-1, -2, -2, -1, 1, 2, 2, 1, 0, 1, 1, 1, 0, -1, -1, -1};