#ifndef functions_rec
#define functions_rec

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>

using namespace std;

// Declarations to avoid compiler errors
void copy_matrix(int _matrix_to_copy_[][7], int _matrix_copied_[][7]);
bool check_result(int _target_[][7], int _matrix_[][7]);
int try_move(unsigned long long move, int previous_state[][7], int next_state[][7], unsigned long long& code_moves, short current_move);
void dump_matrix(int** _matrix_);
void dump_matrix(int _matrix_[][7]);
void dump_number_to_moves_string(unsigned long long cod_move, short moves);

// Retrieving the external variables
extern string str_moves;
extern int target[5][7];

// Defining masks for last moves
unsigned long long mask_1m = 3;       // mask to count the moves
unsigned long long mask_3m = 63;      // mask to check following combinations of moves: UDU, DUD, RLR, LRL
unsigned long long mask_5m = 1023;    // mask to check: UUUUU, DDDDD
unsigned long long mask_7m = 16383;   // mask to check: RRRRRRR, LLLLLLL



bool check_min_moves(int current_state[5][7], short still_available_moves) {
	// Max should not be less than 0 because the max delta row can be at least 0 (otherwise it is a min)
	short delta_row_max = 0;
	// Min should not be greater than 0 because the min delta row can be at least 0 (otherwise it is a max)
	short delta_row_min = 0;
	short delta_col_max = 0;
	short delta_col_min = 0;
	// N.B.: targets matrix is globally visible, no need to pass it as argument
	for (short ii = 0; ii < 35; ii++) {
		short row = ii/7;
		short col = ii%7;
		if (current_state[row][col] == 1 ||
			current_state[row][col] == 2 ||
			current_state[row][col] == 3 ||
			current_state[row][col] == 4)
		{
			int value = current_state[row][col];
			for (short jj = 0; jj < 35; jj++) {
				if (target[jj/7][jj%7] == value) {
					short delta_row = (jj/7 - row);
					short delta_col = (jj%7 - col);
					delta_row_max = max(delta_row_max, delta_row);
					delta_row_min = min(delta_row_min, delta_row);
					delta_col_max = max(delta_col_max, delta_col);
					delta_col_min = min(delta_col_min, delta_col);
					// skipping the rest of this loop because a match was found and they are supposed to be unique
					continue;
				}
			}
		}
	}
	// If this condition is true, the level is still achievable
	return (still_available_moves >= (delta_row_max - delta_row_min + delta_col_max - delta_col_min));
}


// N.B.: the following "MOVE FUNCTIONS" MODIFY the input matrix
bool move_left(int matrix[][7])
{
	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;
	
	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 0; row < 5; row++)
	{
		for (int col = 1; col < 7; col++)
		{
			if (matrix[row][col] != 0 and matrix[row][col] != 7 and matrix[row][col-1] == 0)
			{
				matrix[row][col-1] = matrix[row][col];
				// placing "floor" after the move
				matrix[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}

bool move_right(int matrix[][7])
{
	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;
	
	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 0; row < 5; row++)
	{
		for (int col = 5; col >= 0; col--)		// col starts from 5 because: the indeces in arrays are lowered by 1 w.r.t. the position, so the last colomn has index = 6; we want to avoid verifying the rightest elements because for sure they can't move right
		{
			if (matrix[row][col] != 0 and matrix[row][col] != 7 and matrix[row][col+1] == 0)
			{
				matrix[row][col+1] = matrix[row][col];
				// placing "floor" after the move
				matrix[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}

bool move_up(int matrix[][7])
{

	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;

	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 1; row < 5; row++)
	{
		for (int col = 0; col < 7; col++)
		{
			if (matrix[row][col] != 0 and matrix[row][col] != 7 and matrix[row-1][col] == 0)
			{
				matrix[row-1][col] = matrix[row][col];
				// placing "floor" after the move
				matrix[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}

bool move_down(int matrix[][7])
{

	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;

	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 3; row >= 0; row--)		// row starts from 3 because: the indeces in arrays are lowered by 1 w.r.t. the position, so the last row has index = 4; we want to avoid verifying the "southest" elements because for sure they can't move downward
	{
		for (int col = 0; col < 7; col++)
		{
			if (matrix[row][col] != 0 and matrix[row][col] != 7 and matrix[row+1][col] == 0)
			{
				matrix[row+1][col] = matrix[row][col];
				// placing "floor" after the move
				matrix[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}


// N.B.: the following "MOVE FUNCTIONS" DO NOT MODIFY the input matrix (_nc = "no copy")
bool move_left_nc(int matrix[][7])
{
	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;
	// Copying the matrix to not modify the passed "matrix" argument
	int app[5][7];
	copy_matrix(matrix, app);
	
	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 0; row < 5; row++)
	{
		for (int col = 1; col < 7; col++)
		{
			if (app[row][col] != 0 and app[row][col] != 7 and app[row][col-1] == 0)
			{
				app[row][col-1] = app[row][col];
				// placing "floor" after the move
				app[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}


// Function to recursively evaluate the moves
// TODO:
//  - move_after_3
//  - move_after_5
//  - mover_after_7
//  to skip if statements, that can take much time because they are
//  executed billions of times
// TODO: optimization substituting the string with code move and using bitmasks
int evaluate_level(int previous_state[][7], unsigned long long& code_moves, short current_move, const short max_moves) {

	/* std::cout << "\nEvaluating level with following info:\n";
	std::cout << "Current code move: " << code_moves << std::endl;
	std::cout << "Current move: " << current_move << std::endl;
	std::cout << "Max_moves: " << max_moves << "\n" << std::endl; */

	if (!check_min_moves(previous_state, max_moves - current_move + 1)) return 1;

	int next_state[5][7];

	// Trying all moves: if one fails, try next one; if all fails, return
	// Trying L move (code 0)
	if (try_move(0, previous_state, next_state, code_moves, current_move) == 0) {
		// Move L is ok, check if now the level is solved
		// std::cout << std::string(current_move, ' ') << "L" << std::endl; // for debugging
		if (current_move == max_moves) {
			if (check_result(target, next_state)) { // Level is solved
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		} else { // If not last move go on checking other moves
			// Probably found a solution if return value is 0
			if (evaluate_level(next_state, code_moves, current_move + 1, max_moves) == 0) {
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		}
	}

	// Trying R move (code 1)
	if (try_move(1, previous_state, next_state, code_moves, current_move) == 0) {
		// Move R is ok, check if now the level is solved
		// std::cout << std::string(current_move, ' ') << "R" << std::endl; // for debugging
		if (current_move == max_moves) {
			if (check_result(target, next_state)) { // Level is solved
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		} else { // If not last move go on checking other moves
			// Probably found a solution if return value is 0
			if (evaluate_level(next_state, code_moves, current_move + 1, max_moves) == 0) {
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		}
	}

	// Trying D move (code 2)
	if (try_move(2, previous_state, next_state, code_moves, current_move) == 0) {
		// Move D is ok, check if now the level is solved
		// std::cout << std::string(current_move, ' ') << "D" << std::endl; // for debugging
		if (current_move == max_moves) {
			if (check_result(target, next_state)) { // Level is solved
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		} else { // If not last move go on checking other moves
			// Probably found a solution if return value is 0
			if (evaluate_level(next_state, code_moves, current_move + 1, max_moves) == 0) {
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		}
	}

	// Trying U move (code 3)
	if (try_move(3, previous_state, next_state, code_moves, current_move) == 0) {
		// Move U is ok, check if now the level is solved
		// std::cout << std::string(current_move, ' ') << "U" << std::endl; // for debugging
		if (current_move == max_moves) {
			if (check_result(target, next_state)) { // Level is solved
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		} else { // If not last move go on checking other moves
			// Probably found a solution if return value is 0
			if (evaluate_level(next_state, code_moves, current_move + 1, max_moves) == 0) {
				return 0;
			} else {
				// delete last move
				code_moves >>= 2;
			}
		}
	}
	
	// If at this point, evaluated all moves with no success:
	//  going back to check other moves
	return 1;
}


bool last_3_valid(unsigned long long& code_move) {
	// LRL = 0b000100 = 4     skipping from LRL... to LRR...
	// RLR = 0b010001 = 17    skipping from RLR... to RLD...
	// DUD = 0b101110 = 46    skipping from DUD... to DUU...
	// UDU = 0b111011 = 59    skipping from UDU... to UUL...
	return (((code_move & mask_3m) != 4) &
			((code_move & mask_3m) != 17) &
			((code_move & mask_3m) != 46) &
			((code_move & mask_3m) != 59));
}


bool last_5_valid(unsigned long long& code_move) {
	// DDDDD = 0b1010101010 = 682   skipping from DDDDD... to DDDDU...
	// UUUUU = 0b1111111111 = 1023  skipping from ...XYUUUUU... to ..XLLLLLL...
	// LLRRL = 0b0000010100 = 20    skipping because this is an invalid combination
	// RRLLR = 0b0101000001 = 321
	// UUDDU = 0b1010111110 = 702
	// DDUUD = 0b1111101011 = 1003
	return (((code_move & mask_5m) != 682) &
			((code_move & mask_5m) != 1023) &
			((code_move & mask_5m) != 20) &
			((code_move & mask_5m) != 321) &
			((code_move & mask_5m) != 702) &
			((code_move & mask_5m) != 1003));
}


bool last_7_valid(unsigned long long& code_move) {
	// LLLLLLL = 0b00000000000000 = 0   skipping from LLLLLLL... to LLLLLLR...
	// RRRRRRR = 0b01010101010101 = 5461    skipping from RRRRRRR... to RRRRRRU...
	return (((code_move & mask_7m) != 0) &
			((code_move & mask_7m) != 5461));
}


bool last_3_5_valid(unsigned long long& code_move) {
	// LRL = 0b000100 = 4     skipping from LRL... to LRR...
	// RLR = 0b010001 = 17    skipping from RLR... to RLD...
	// DUD = 0b101110 = 46    skipping from DUD... to DUU...
	// UDU = 0b111011 = 59    skipping from UDU... to UUL...
	// DDDDD = 0b1010101010 = 682   skipping from DDDDD... to DDDDU...
	// UUUUU = 0b1111111111 = 1023  skipping from ...XYUUUUU... to ..XLLLLLL...
	// LLRRL = 0b0000010100 = 20    skipping because this is an invalid combination
	// RRLLR = 0b0101000001 = 321
	// UUDDU = 0b1010111110 = 702
	// DDUUD = 0b1111101011 = 1003
	return (((code_move & mask_3m) != 4) &
			((code_move & mask_3m) != 17) &
			((code_move & mask_3m) != 46) &
			((code_move & mask_3m) != 59) &

			((code_move & mask_5m) != 682) &
			((code_move & mask_5m) != 1023) &
			((code_move & mask_5m) != 20) &
			((code_move & mask_5m) != 321) &
			((code_move & mask_5m) != 702) &
			((code_move & mask_5m) != 1003));
}


bool last_3_5_7_valid(unsigned long long& code_move) {
	// LRL = 0b000100 = 4     skipping from LRL... to LRR...
	// RLR = 0b010001 = 17    skipping from RLR... to RLD...
	// DUD = 0b101110 = 46    skipping from DUD... to DUU...
	// UDU = 0b111011 = 59    skipping from UDU... to UUL...
	// DDDDD = 0b1010101010 = 682   skipping from DDDDD... to DDDDU...
	// UUUUU = 0b1111111111 = 1023  skipping from ...XYUUUUU... to ..XLLLLLL...
	// LLRRL = 0b0000010100 = 20    skipping because this is an invalid combination
	// RRLLR = 0b0101000001 = 321
	// UUDDU = 0b1010111110 = 702
	// DDUUD = 0b1111101011 = 1003
	// LLLLLLL = 0b00000000000000 = 0   skipping from LLLLLLL... to LLLLLLR...
	// RRRRRRR = 0b01010101010101 = 5461    skipping from RRRRRRR... to RRRRRRU...
	return (((code_move & mask_3m) != 4) &
			((code_move & mask_3m) != 17) &
			((code_move & mask_3m) != 46) &
			((code_move & mask_3m) != 59) &
			
			((code_move & mask_5m) != 682) &
			((code_move & mask_5m) != 1023) &
			((code_move & mask_5m) != 20) &
			((code_move & mask_5m) != 321) &
			((code_move & mask_5m) != 702) &
			((code_move & mask_5m) != 1003) &

			((code_move & mask_7m) != 0) &
			((code_move & mask_7m) != 5461) );
}

// Function to try the moves
int try_move(unsigned long long move, int previous_state[][7], int next_state[][7], unsigned long long& code_moves, short current_move) {

	// Expecting that next_state is a copy of previous state, to be able to evaluate moves
	copy_matrix(previous_state, next_state);

	// ----- TRYING MOVE -----
	code_moves <<= 2;
	code_moves += move; // Adding move code

	// Check masks: if not valid, return
	if (current_move >= 7) {
		if (!last_3_5_7_valid(code_moves)) {
			code_moves >>= 2;
			return 1;
		}
	} else if (current_move >= 5) {
		if (!last_3_5_valid(code_moves)) {
			code_moves >>= 2;
			return 1;
		}
	} else if (current_move >= 3) {
		if (!last_3_valid(code_moves)) {
			code_moves >>= 2;
			return 1;
		}
	}

	// Try move
	switch (move)
	{
	case 0:
		if (!move_left(next_state)) {
			code_moves >>= 2;
			return 1;
		}
		break;
	case 1:
		if (!move_right(next_state)) {
			code_moves >>= 2;
			return 1;
		}
		break;
	case 2:
		if (!move_down(next_state)) {
			code_moves >>= 2;
			return 1;
		}
		break;
	case 3:
		if (!move_up(next_state)) {
			code_moves >>= 2;
			return 1;
		}
		break;
	default:
		return 2; // Returning unexpected result
		break;
	}

	return 0;
}


bool move_right_nc(int matrix[][7])
{
	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;
	// Copying the matrix to not modify the passed "matrix" argument
	int app[5][7];
	copy_matrix(matrix, app);
	
	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 0; row < 5; row++)
	{
		for (int col = 5; col >= 0; col--)		// col starts from 5 because: the indeces in arrays are lowered by 1 w.r.t. the position, so the last colomn has index = 6; we want to avoid verifying the rightest elements because for sure they can't move right
		{
			if (app[row][col] != 0 and app[row][col] != 7 and app[row][col+1] == 0)
			{
				app[row][col+1] = app[row][col];
				// placing "floor" after the move
				app[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}


bool move_up_nc(int matrix[][7])
{

	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;
	// Copying the matrix to not modify the passed "matrix" argument
	int app[5][7];
	copy_matrix(matrix, app);

	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 1; row < 5; row++)
	{
		for (int col = 0; col < 7; col++)
		{
			if (app[row][col] != 0 and app[row][col] != 7 and app[row-1][col] == 0)
			{
				app[row-1][col] = app[row][col];
				// placing "floor" after the move
				app[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}

bool move_down_nc(int matrix[][7])
{

	// this boolean flags if the move has been done; if not, the move return false and in the main the code will skip the sequence of moves because, throughout the call to "move" function, no real move has been done
	bool moved = false;
	// Copying the matrix to not modify the passed "matrix" argument
	int app[5][7];
	copy_matrix(matrix, app);

	// start by moving the first colomn on the left, thus next columns are already updated when moving
	// towards them, in the sense that if there would be "floor" on the right (because of a movement
	// of a preceding piece that was there) thus they can move.
	for (int row = 3; row >= 0; row--)		// row starts from 3 because: the indeces in arrays are lowered by 1 w.r.t. the position, so the last row has index = 4; we want to avoid verifying the "southest" elements because for sure they can't move downward
	{
		for (int col = 0; col < 7; col++)
		{
			if (app[row][col] != 0 and app[row][col] != 7 and app[row+1][col] == 0)
			{
				app[row+1][col] = app[row][col];
				// placing "floor" after the move
				app[row][col] = 0;
				moved = true;
			}
		}
	}
	
	return moved;
}


// defining the matrix reader function
int** read_matrix(ifstream& ifile_)		// N.B.: RICORDARSI DI METTERE UNA REFERENCE AD UN ifstream, NON UN ISTANZA STESSA!
{
//	int matrix[5][7];	// da allocare dinamicamente (N.B.: POI BISOGNA RICORDARSI DI USARE IL DELETE, ALTRIMENTI
						// SI RISCHIA DI FARE DANNI SERI)
						
	int** matrix = 0;
	matrix = new int*[5];
	
	for (int row = 0; row < 5; row++)
	{
		matrix[row] = new int[7];	// ad ogni riga ci sono 7 interi
		
		for (int col = 0; col < 7; col++)
		{
			ifile_ >> matrix[row][col];
		}
	}
	
	return matrix;
}

void delete_matrix(int** matrix)
{
	for (int row = 0; row < 5; row++)
	{
		delete[] matrix[row];
	}
	delete[] matrix;
	matrix = 0;

}

int** copy_matrix(int** _matrix_)
{
	int** c = 0;
	c = new int*[5];
	
	for (int row = 0; row < 5; row++)
	{
		c[row] = new int[7];	// ad ogni riga ci sono 7 interi
		
		for (int col = 0; col < 7; col++)
		{
			c[row][col] = _matrix_[row][col];
		}
	}
	
	return c;
}

// previous version (to avoid allocating memory)
void copy_matrix(int** _matrix_to_copy_, int** _matrix_copied_)
{
	for (int row = 0; row < 5; row++)
	{		
		for (int col = 0; col < 7; col++)
		{
			if (_matrix_to_copy_[row][col] != _matrix_copied_[row][col])
			{
				_matrix_copied_[row][col] = _matrix_to_copy_[row][col];
			}
		}
	}
	
	return;
}


void copy_matrix(int _matrix_to_copy_[][7], int _matrix_copied_[][7])
{
	for (int row = 0; row < 5; row++)
	{		
		for (int col = 0; col < 7; col++)
		{
			if (_matrix_to_copy_[row][col] != _matrix_copied_[row][col])
			{
				_matrix_copied_[row][col] = _matrix_to_copy_[row][col];
			}
		}
	}
	
	return;
}

bool check_result(int** _target_, int** _matrix_)
{
	for (int row = 0; row < 5; row++)
	{		
		for (int col = 0; col < 7; col++)
		{
			if (_target_[row][col] != 0)
			{
				if (_target_[row][col] != _matrix_[row][col])		return false;
			}
		}
	}
	
	return true;
}

bool check_result(int _target_[][7], int _matrix_[][7])
{
	for (int row = 0; row < 5; row++)
	{		
		for (int col = 0; col < 7; col++)
		{
			if (_target_[row][col] != 0)
			{
				if (_target_[row][col] != _matrix_[row][col])		return false;
			}
		}
	}
	
	return true;
}

void dump_matrix(int** _matrix_)
{
	for (int row = 0; row < 5; row++)
	{		
		for (int col = 0; col < 7; col++)
		{
			cout << _matrix_[row][col] << " ";
		}
		cout << endl;
	}
}

void dump_matrix(int _matrix_[][7])
{
	for (int row = 0; row < 5; row++)
	{		
		for (int col = 0; col < 7; col++)
		{
			cout << _matrix_[row][col] << " ";
		}
		cout << endl;
	}
}

string string_reverse(string &s)
{
	string rev = "";
	
	for (char &c : s)
	{
		rev = c + rev;
	}
	
	return rev;
}

void print_file_error()
{
	cout << "There is no input file specified: please, use the following syntax:\n \">>>./Cryptica input FILENAME moves NUM_MOVES\".\n\nBe sure that the file is in the correct folder." << endl;
	return;
}

void print_num_moves_error()
{
	cout << "Missing number of moves required: please, use the following syntax:\n\n>>>./Cryptica input FILENAME moves NUM_MOVES\"." << endl;
	return;
}


// function to translate the current number into a readable move string
void dump_number_to_moves_string(unsigned long long cod_move, short moves)
{
	while(moves > 0)
	{
		unsigned long long move = (3 * pow(4, moves-1));
		//cout << move << endl;
		move = move & cod_move;
		
		move >>= 2*(moves-1);
		
		switch(move)
		{
			case(0):
			cout << 'L';
			break;
			
			case(1):
			cout << 'R';
			break;
			
			case(2):
			cout << 'D';
			break;
			
			case(3):
			cout << 'U';
			break;
		}
		
		moves--;
	}
	
	cout << endl;
	
	return;
}

// function to translate the current number into a readable move string
//	N.B.: the number is intended to be coded as the reverse string of moves
void dump_reverse_number_to_moves_string(unsigned long long cod_move, short moves_to_show)
{
	while(moves_to_show > 0)
	{
		int move = 3 & cod_move;
		
		switch(move)
		{
			case(0):
			cout << 'L';
			break;
			
			case(1):
			cout << 'R';
			break;
			
			case(2):
			cout << 'D';
			break;
			
			case(3):
			cout << 'U';
			break;
		}
		
		cod_move = cod_move >> 2;
		moves_to_show--;
	}
	
	cout << endl;
	
	return;
}


#endif