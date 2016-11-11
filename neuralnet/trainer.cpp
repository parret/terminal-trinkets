#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

int main()
{
	// Random training set for XOR -- two input and one output

	for (int i = 10000000; i >= 0; --i)
	{
		int n1 = (int)(2.0 * rand() / double(RAND_MAX));
		int n2 = (int)(2.0 * rand() / double(RAND_MAX));

		// int t = n1 ^ n2; // should be 0 or 1
		// int t = n1 & n2; // should be 0 or 1
		// int t = n1 | n2; // should be 0 or 1
		int t = (n1 & !n2) || (!n1 & n2);

		std::cout << n1 << endl << n2 << endl << t << endl;
	}
}
