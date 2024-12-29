#include "DFA.h"

using namespace std;

int main()
{
	auto dfa = DFA("testDFA.txt");
	dfa.draw();
	cout << "\n\n";
	cout << "New DFA:\n\n";
	dfa.minimize();
	dfa.draw();
}