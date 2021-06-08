#include <iostream>
#include "MarkovChain.h"

//=========================================================================================
// An example of 3 nodes, 0x10 and 0x12 transmit each 10ms, and 0x14 transmits every 20ms.|
//=========================================================================================
// 0x10 ---'a'	===									  |
// 0x12 ---'b'     |- " Mapping of IDs in characters "					  |
// 0x14 ---'c'  ===									  |
//=========================================================================================
//	Afterwards, in the window of 40ms, all possible transmission sequences are created|
//	in the defined trainingSets, and given in the Markov Chain for evaluation of the  |
//	probabilities each time.							  |
//=========================================================================================

using namespace std;

MarkovChain chain;

//creates a sequence
void create( char * pos, char a, char b, char c, char d, char e, char zero)
{
	pos[0] = a;
	pos[1] = b;
	pos[2] = c;
	pos[3] = d;
	pos[4] = e;
	pos[5] = zero;
}

//character to ID
int map(char a)
{
	switch(a) {
	case 'a':
		return 10;
	case 'b':
		return 12;
	case 'c':
		return 14;
	}
}

//ID to character
char revmap(int a)
{
	switch(a) {

	case 10:
		return 'a';
	case 12:
		return 'b';
	case 14:
		return 'c';
	default:
		return 'x'; // could be a sign for alert.
	}
}
//checks if a valid ID.
int valid( int id)
{
	if( (id == 10) || (id == 12) || (id==14) )
		return 1;
	return 0;
}

// Id to index 0-2
int IDtoIndex(int id)
{
	switch(id) {
	case 10:
		return 0;
	case 12:
		return 1;
	case 14:
		return 2;
		//no default, since is only called when there is a valid ID.
	}
}

int main(void)
{
	char **trainingSet;
	int possibilityArray[3];
	int IDS[3] = { 10, 12, 14};

	trainingSet = (char **) malloc(12*sizeof(char *));

	for(int i = 0; i < 3; i++)
		possibilityArray[i] = 0;

	cout << "Possibilities array initialized" << endl;

	for(int i = 0; i < 12; i++)
		trainingSet[i] = (char *) malloc(6*sizeof(char));

	// trainingSets creation.
	create(trainingSet[0], 'a', 'b', 'a', 'b', 'c', '\0');
	create(trainingSet[1], 'a', 'b', 'a', 'c', 'b', '\0');
	create(trainingSet[2], 'a', 'b', 'b', 'a', 'c', '\0');
	create(trainingSet[3], 'a', 'b', 'b', 'c', 'a', '\0');
	create(trainingSet[4], 'a', 'b', 'c', 'a', 'b', '\0');
	create(trainingSet[5], 'a', 'b', 'c', 'b', 'a', '\0');
	create(trainingSet[6], 'b', 'a', 'a', 'b', 'c', '\0');
	create(trainingSet[7], 'b', 'a', 'a', 'c', 'b', '\0');
	create(trainingSet[8], 'b', 'a', 'b', 'a', 'c', '\0');
	create(trainingSet[9], 'b', 'a', 'b', 'c', 'a', '\0');
	create(trainingSet[10], 'b', 'a', 'c', 'a', 'b', '\0');
	create(trainingSet[11], 'b', 'a', 'c', 'b', 'a', '\0');

	cout << "Training Set " << endl;

	//printing the sequences.
	for(int i = 0; i < 12; i++) {
		cout << "Seq" << i << " "  << endl;
		cout << map(trainingSet[i][0]) << " " ;
		cout << map(trainingSet[i][1]) << " " ;
		cout << map(trainingSet[i][2]) << " " ;
		cout << map(trainingSet[i][3]) << " " ;
		cout << map(trainingSet[i][4]) << " " << endl;
	}

	// to get probability results every 5 messages on the bus.
	int window_index = 0;
	int window = 5;
	char trace[5];	// used to hold IDs within' a WINDOW

	char elements[] = { 'a', 'b', 'c'};
	int test = 99;
	int isValid;
	int change_flag;

	cout << "Enter valid ID (10, 12, 14) to see\nthe probability of the next shown ID on the bus.\nEnter 0 to exit" << endl;

	while(test != 0) {

		change_flag = 0;
		cout << "Enter ID: " << endl;
		cin >> test;
		trace[window_index++] = revmap(test);

		if( valid(test) ) {

			if( possibilityArray[IDtoIndex(test)] ) {
				cout << "\t\tALERT :::  ID had show-up possibility 0." << endl;
				possibilityArray[IDtoIndex(test)] = 0;
			}

			double *probs = chain.getNextTransitions(revmap(test), elements, 3, trainingSet, 12);
			cout << "Probability of IDs appearing next: " << endl;

			for(int i = 0; i < 3; i++) {
				cout << map(elements[i]) << " : " << probs[i] << endl;
				// remember this in next reception, to raise ALERT because
				// probability will be zero to see this specific ID.
				if(probs[i] == 0.0) {
					possibilityArray[i] = 1;
					change_flag++;
				}
			}

		} else {
			cout << "\t\tALERT ::: invalid ID." << endl;
		}

		if(!change_flag)
			for(int i = 0; i < 3; i++)
				possibilityArray[i] = 0;
		
		if(window_index == window) {
			double res;
			cout << "Getting recent window Sequence probability" << endl;
			res = chain.getSequenceProbability(trace, window, elements, 3, trainingSet, 12);
			cout << "Probability of recent Window: " << res << endl;
			window_index = 0;
		}

	}
	return (0);
}
