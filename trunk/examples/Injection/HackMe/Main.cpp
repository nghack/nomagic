#include <iostream>

int findMe;
bool keepRunning;

void callMe()
{
	std::cout << "useless function is useless!" << std::endl;
}

int main()
{
	keepRunning = true;
	findMe = 0;
	int n = 0;

	std::cout << &findMe << std::endl;

	//try it -> uncomment this function -> try it again! it should still work!
	//callMe();

	while(keepRunning)
	{
		std::cout << "Add to findMe: ";
		std::cin >> n;
		findMe += n;
		std::cout << "findMe is now: " << findMe << std::endl;
		std::cin.get();
	}
	std::cin.get();
}