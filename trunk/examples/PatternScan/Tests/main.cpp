#include <iostream>

int findYou = 6;
int findMe;

void incr(int amt)
{
	findMe += amt;
}

int main()
{
	findYou = 0;
	findMe = 0;
	int by = 0;

	do
	{
		std::cout << "Increase variable by: ";
		std::cin >> by;
		incr(by);
		std::cout << "Variable now contains: " << findMe << std::endl;
	} while(true);
	findYou += 1;
	std::cout << findYou;

	return EXIT_SUCCESS;
}

