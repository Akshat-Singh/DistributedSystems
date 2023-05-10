#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

class Acceptor {
public:
    int highestPrepareProposal = -1;
    int acceptedProposal = -1;
    int acceptedValue = -1;

    pair<int, int> prepare(int proposalNumber) {
        if (proposalNumber > highestPrepareProposal) {
            highestPrepareProposal = proposalNumber;
            return make_pair(acceptedProposal, acceptedValue);
        }
        return make_pair(-1, -1);
    }

    bool accept(int proposalNumber, int value) {
        if (proposalNumber >= highestPrepareProposal) {
            acceptedProposal = proposalNumber;
            acceptedValue = value;
            return true;
        }
        return false;
    }
};

class Proposer {
public:
    int proposalNumber;
    int value;

    Proposer(int proposalNumber, int value) : proposalNumber(proposalNumber), value(value) {}

    int propose(vector<Acceptor> &acceptors) {
        int quorumSize = acceptors.size() / 2 + 1;
        int prepareResponses = 0;
        int highestAcceptedProposal = -1;
        int highestAcceptedValue = -1;

        for (Acceptor &acceptor : acceptors) {
            pair<int, int> response = acceptor.prepare(proposalNumber);
            if (response.first != -1) {
                if (response.first > highestAcceptedProposal) {
                    highestAcceptedProposal = response.first;
                    highestAcceptedValue = response.second;
                }
            }
            prepareResponses++;
            if (prepareResponses >= quorumSize) 
                break;
            
        }

        if (prepareResponses < quorumSize) 
            return -1;

        if (highestAcceptedValue != -1) 
            value = highestAcceptedValue;

        int acceptResponses = 0;
        for (Acceptor &acceptor : acceptors) {
            if (acceptor.accept(proposalNumber, value)) 
                acceptResponses++;
            if (acceptResponses >= quorumSize) 
                return value;
        }

        return -1;
    }
};

int main() {
    srand(time(nullptr));

    const int NUM_ACCEPTORS = 5;
    vector<Acceptor> acceptors(NUM_ACCEPTORS);

    Proposer proposer1(1, 10);
    
    int result1 = proposer1.propose(acceptors);
    if (result1 != -1) 
        cout << "Proposer 1 reached consensus with value: " << result1 << endl; 
    else 
        cout << "Proposer 1 failed to reach consensus"<< endl;
    

    Proposer proposer2(2, 20);
    int result2 = proposer2.propose(acceptors);
    if (result2 != -1) 
        cout << "Proposer 2 reached consensus with value: " << result2 << endl;
    else 
        cout << "Proposer 2 failed to reach consensus" << endl;

    Proposer proposer3(3, 30);
    int result3 = proposer3.propose(acceptors);
    if (result3 != -1) 
        cout << "Proposer 3 reached consensus with value: " << result3 << endl;
    else 
        cout << "Proposer 3 failed to reach consensus" << endl;

    return 0;
}


