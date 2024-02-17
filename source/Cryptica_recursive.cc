#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>

#include "functions_rec.h"


// Defyning all variables needed do be global in all functions
string str_move = "";			// string of SOLUTION moves
int target[5][7];				// matrix of the final positions of the "special" rocks

int main(int argc, char* argv[]) {

	std::cout << "\nStarting...\n" << endl;

	// Unfortunately, this is hardcoded for now
	string directory = "../Levels/";
	string targets = "_targets";

	// collecting all possible info
	vector<string> args;
	args.resize(argc);
	for ( int iarg = 1; iarg < argc; ++iarg ) args[iarg] = argv[iarg];
	
	
// ---------- INSTANTIATING ALL VARIABLES NEEDED ----------

	string init_moves = "";			// string of initial moves: crucial to avoid some computations
	string final_moves = "";		// string of final moves: crucial to avoid some computations
	int less_moves = 0;			// int that counts how many moves the algorithm can skip
	int num_moves = 0;			// total number of moves
	string name = "";				// name of the level: tha program already has the folder and the respective target file set
	int env[5][7];				// matrix of the initial environment
	int work[5][7];				// copy of env to be modified at every iteration; at the beginning of every iteration this would be resetted by copying env
	
// ---------- READING INPUT ----------
	
	int cont_info = 0;
	while (cont_info < argc) {
		if (args[cont_info] == "input") {

			// opening environment file
			name = args[cont_info+1];
			ifstream file_env( directory + name );
	
			// check if file is open
			if (not( file_env.is_open())) {
				std::cout << "\nError during environment file opening...\n" << endl;
				return 1;
			}
			
			// opening target file	
			ifstream file_target( directory + name + targets );

			// check if file is open
			if (not( file_target.is_open())) {
				std::cout << "\nError during target file opening...\n" << endl;
				return 1;
			}
			
			// building environmnet matrix from file
			int** env_app = read_matrix(file_env);

			// copying matrix to avoid using dynamic allocated memory
			for (int row = 0; row < 5; row++) {		
				for (int col = 0; col < 7; col++) {
					env[row][col] = env_app[row][col];
				}
			}

			// deallocating allocated memory
			delete_matrix(env_app);

			// closing file after read it
			file_env.close();
			

			// building target matrix from file
			int** target_app = read_matrix(file_target);
	
			// copying matrix to avoid using dynamic allocated memory
			for (int row = 0; row < 5; row++) {		
				for (int col = 0; col < 7; col++) {
					target[row][col] = target_app[row][col];
				}
			}
	
			// deallocating allocated memory
			delete_matrix(target_app);
	
			// closing file after read it
			file_target.close();


			// copying matrix (to keep the original one)
			for (int row = 0; row < 5; row++) {		
				for (int col = 0; col < 7; col++) {
					work[row][col] = env[row][col];
				}
			}
	
			std::cout << "Files opened...\n" << endl;
		}
		
		if (args[cont_info] == "moves") {
			// getting number of moves
			const string n = args[cont_info+1];
			stringstream sstr;
			sstr.str(n);
			sstr >> num_moves;
			sstr.clear();
		}
		
		if (args[cont_info] == "start") {
			init_moves = args[cont_info+1];
			less_moves += init_moves.length();
		}
		
		/* Skipping final moves in this program
		if (args[cont_info] == "final") {
			final_moves = args[cont_info+1];
			less_moves += final_moves.length();
		}*/
		
		cont_info++;
	}

	std::cout << "Number of moves is: " << num_moves << endl << endl;

	std::cout << "Ended initialization...\n" << endl;

// ---------- ENDED INITIALIZZATION ----------

// ---------- CHECKING ERRORS OR NOT INITIALIZED THINGS ----------

	if (name == "") {
		print_file_error();
		return 1;
	}

	if (num_moves == 0) {
		print_num_moves_error();
		return 1;
	}



// ---------- EVALUATING INITIAL MOVES ----------

	// Using a bool variable to store checks results
	bool testing = true;

	// Copying the matrix no not modify "env" variable
	copy_matrix(env, work);

	if (init_moves != "") {
		for (char &c : init_moves) {
			
			switch(c) {
				case('L'):
				if (not(move_left(work)))	testing = false;
				break;
				
				case('R'):
				if (not(move_right(work)))	testing = false;
				break;
				
				case('D'):
				if (not(move_down(work)))	testing = false;
				break;
				
				case('U'):
				if (not(move_up(work)))		testing = false;
				break;
			}
		}
	}
	
	std::cout << "\nMatrix after provided initial moves:\n" << std::endl;

	dump_matrix(work);
	
	if (testing) {
		std::cout << "\nValid initial moves; going on...\n" << std::endl;
	} else {
		// TODO: check for invalid initial combinations. like LRL, etc.
		std::cout << "\nProvided initial moves are invalid: exiting execution...\n" << std::endl;
		return 0;
	}

	// N.B.: for now not implementing insights for final moves as they are tricky to check


// ---------- TRANSLATING INITIAL AND FINAL MOVES INTO NUMBERS ----------

	unsigned long long tail_moves_bits = 0;

	// adding initial moves
	for (char &c : init_moves) {
		switch(c) {
			case('L'):				// posso saltare questo caso in quanto aggiungo semplicemente 0
			tail_moves_bits += 0;
			//countL++;
			break;
			
			case('R'):
			tail_moves_bits += 1;
			//countR++;
			break;
			
			case('D'):
			tail_moves_bits += 2;
			//countD++;
			break;
			
			case('U'):
			tail_moves_bits += 3;
			//countU++;
			break;
		}
		
		tail_moves_bits <<= 2;
	}
	
	tail_moves_bits >>= 2;		// deleting the last shift because it is wasteful/pointless

// ---------- CHECKING tail_moves_bytes IS CORRECT ----------

	// to verify the string is correct
	std::cout << "\nChecking initial moves:\n";
	dump_number_to_moves_string(tail_moves_bits, init_moves.length());
	std::cout << "\n" << endl;

	// Build numbers properly using initial moves
	// Start looking for solutions from that state (eventually returning 1)

	unsigned long long counter_for_moves = 0;
	short current_move = 1 + init_moves.length();
	// const short max_moves = num_moves - init_moves.length();
	// Since current move is already taking into consideration the initial moves,
	//  the max moves should be the original value
	const short max_moves = num_moves;
	
	for (char& c : init_moves) {
		switch(c) {
			case('L'):				// posso saltare questo caso in quanto aggiungo semplicemente 0
				//counter_for_moves += 0;
				counter_for_moves <<= 2;
				break;
			
			case('R'):
				counter_for_moves += 1;
				counter_for_moves <<= 2;
				break;
			
			case('D'):
				counter_for_moves += 2;
				counter_for_moves <<= 2;
			break;
			
			case('U'):
				counter_for_moves += 3;
				counter_for_moves <<= 2;
			break;
		}
	}
	// Resetting last move as it is unnecessary
	counter_for_moves >>= 2;

	std::cout << "Code moves: " << counter_for_moves << std::endl;

	// TODO: try some usages of the recursive function
	if (evaluate_level(work, counter_for_moves, current_move, max_moves) == 0) {
		// Solution has been found
		std::cout << "Found solution: " << counter_for_moves << std::endl;
		dump_number_to_moves_string(counter_for_moves, num_moves);
	} else {
		std::cout << "\nSolution was not found.\nTry different initial moves..." << std::endl;
	}

	return 0;
}