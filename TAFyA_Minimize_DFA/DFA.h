#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;



//struct State
//{
//    string symbol;
//    bool isFinal;
//    State(string s, bool f) : symbol(s), isFinal(f) {}
//};
//
//struct Transition
//{
//    string trigger;
//    State* startState;
//    State* endState;
//
//    Transition(string tr, State* sS, State* eS) : trigger(tr), startState(sS), endState(eS) {}
//};



class DFA
{
    //<<startState, trigger>, endState>
	map<pair<string, string>, string> transitionTable;
    //<symbol, isFinal>
    map<string, bool> states;
    set<string> triggers;
    string startState;

public:

	DFA(const string& filename) 
    {
		parseTransitionTable(filename);
	}

    void draw() const
    {
        cout << "States:\n";
        for (const auto& state : states) {
            cout << state.first;
            if (state.second)
                cout << " (Final)";
            cout << "; ";
        }
        cout << "\nStart State: " << startState << "\n";

        cout << "Transition Table:\n";
        for (const auto& trans : transitionTable) 
            cout << trans.first.first << " (" << trans.first.second << ") -> " << trans.second << "\n";
    }

    void minimize()
    {
        set<string> finals, nonFinals;
        for (const auto& state : states)
        {
            if (state.second)
                finals.insert(state.first);
            else
                nonFinals.insert(state.first);
        }
        vector<set<string>> statesSets = { finals, nonFinals };

        bool hasChanges;
        int i = 1;
        do
        {
            hasChanges = false;
            set<vector<string>> triggerCombinations = get_trigger_combinations(i);
            vector<set<string>> newSS;
            for (int j = 0; j < statesSets.size(); j++)
            {
                map<vector<int>, set<string>> setsMap;
                for (const auto& state : statesSets[j])
                {
                    vector<int> pathsResults;
                    for (const auto& combination : triggerCombinations)
                    {
                        string curState = state;
                        for (const auto& trigger : combination)
                        {
                            curState = transitionTable[make_pair(curState, trigger)];
                        }
                        for (int k = 0; k < statesSets.size(); k++)
                        {
                            if (statesSets[k].find(curState) != statesSets[k].end())
                            {
                                pathsResults.push_back(k);
                                break;
                            }
                        }
                    }
                    setsMap[pathsResults].insert(state);
                }
                
                for (const auto& m_pair : setsMap)
                {
                    newSS.push_back(m_pair.second);
                }
                if (newSS.size() > 1 + j)
                    hasChanges = true;
            }
            statesSets = newSS;
            //cout << "step " << i << "; " << hasChanges << "\n";
            i++;
        } while (hasChanges || i <= 3);

#pragma region Remove dublicate states

        for (const auto& s : statesSets)
        {
            if (s.size() > 1) // => these states are equal
            {
                auto it = s.begin();
                string firstState = *it++;
                for (auto trigger : triggers)
                {
                    if (transitionTable.find(make_pair(firstState, trigger)) != transitionTable.end() && s.find(transitionTable[make_pair(firstState, trigger)]) != s.end())
                        transitionTable[make_pair(firstState, trigger)] = firstState;
                }
                if (s.find(startState) != s.end())
                    startState = firstState;
                while (it != s.end())
                {
                    for (auto trigger : triggers)
                    {
                        if (transitionTable.find(make_pair(*it, trigger)) != transitionTable.end())
                        {
                            if (s.find(transitionTable[make_pair(*it, trigger)]) != s.end())
                                transitionTable[make_pair(firstState, trigger)] = firstState;
                            else
                                transitionTable[make_pair(firstState, trigger)] = transitionTable[make_pair(*it, trigger)];
                            transitionTable.erase(make_pair(*it, trigger));
                        }
                    }
                    states.erase(*it);
                    it++;
                }
               
            }
        }

#pragma endregion
    }

private:

    void parseTransitionTable(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Could not open the file!\n";
            return;
        }

#pragma region Get The Transitions

        string linePrev;
        string line;
        getline(file, linePrev);
        while (getline(file, line)) 
        {
            istringstream iss(linePrev);
            string sS, eS, trigger;

            if (iss >> sS >> trigger >> eS) 
            {
                states[sS] = false;
                states[eS] = false;
                triggers.insert(trigger);
                transitionTable[make_pair(sS, trigger)] = eS;
            }
            linePrev = line;
        }

#pragma endregion

#pragma region Get The Start State and Final States in the last line

        //it is supposed that the first state is StartState while all the others are Final (so StartState can be encountered 2 times if it is also the FinalState)
        istringstream iss(line);
        string symbol;

        iss >> startState;
        while (iss >> symbol)
        {
            states[symbol] = true;
        }

#pragma endregion

        file.close();
    }

    set<vector<string>> get_trigger_combinations(int combLen)
    {
        set<vector<string>> res;
        for (auto symbol : triggers)
        {
            vector<string> curComb = { symbol };
            gen_combination(res, curComb, combLen);
        }
        return res;
    }

    void gen_combination(set<vector<string>>& res, vector<string>& curCombination, int combLen)
    {
        if (curCombination.size() == combLen)
        {
            res.insert(curCombination);
            return;
        }

        for (auto symbol : triggers)
        {
            curCombination.push_back(symbol);
            gen_combination(res, curCombination, combLen);
            curCombination.pop_back();
        }
    }

};