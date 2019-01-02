#include <iostream>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <climits>

#define DEFAULT_PROBLEM_SIZE 10000

// Sorts an array via the BubbleSort algorithm.
// inputs: int *array - Pointer to the array of numbers.
//		   int length - Length of array.
void bubbleSort(int* array, int length) {
	int n = length;
	int temp;
	bool swapped = true;
	while (swapped == true) {
		swapped = false;
		for (int i=1; i<n; i++) {
			if (array[i-1] > array[i]) {
				temp = array[i-1];
				array[i-1] = array[i];
				array[i] = temp;
				swapped = true;
			}
		}
		n--;
	}
}

// Checks whether an array is sorted. That's all this function does. Nothing else to see here.
// inputs: int *array - Pointer to the array of numbers.
//         int length - length of array.
// output: True if the array was sorted. False otherwise.
bool sorted(int *array, int length) {
	int i=0;
	for (i=0; i<length-1; i++) {
		if (array[i] > array[i+1]){
			return false;
		}
	}
	return true;
}

// Inputs: N    - length of array to sort.
int main(int argc, char **argv) {
	// Set default size and array type.
	int N = DEFAULT_PROBLEM_SIZE;

	// Handle command line inputs.
	if (argc == 2) {
		std::istringstream(argv[1]) >> N;
	} else {
		std::cout << "Program needs size of array to sort as input." << std::endl;
	}

	// Generate an array with the given length and type.
	int *array = new int[N];
	srand(time(NULL));
	for (int i=0; i<N; i++) {
		array[i] = rand() + rand() - RAND_MAX;
	}

	// Timer start.
	std::clock_t start = std::clock();
	// Sort the array.
	bubbleSort(array, N);
	// Timer stop.
	double extime = (std::clock()-start)/double(CLOCKS_PER_SEC)*1.0;

	// If the array is sorted, output the execution time.
	if (sorted(array, N)) {
		std::cout << extime << std::endl;
	} else {
		std::cout << "Error: array not sorted." << std::endl;
		exit(1);
	}

	return 0;
}
