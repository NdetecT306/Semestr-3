#include "screen.h"
using namespace std;
int main(int argc, char* argv[]) {
    string filename, query;
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--file" && i + 1 < argc) { 
            filename = argv[++i];
        } 
        else if (string(argv[i]) == "--query" && i + 1 < argc) {
            query = argv[++i];
        }
    }
    loadFromFile(filename);
    string result = processQuery(query, filename);
    cout << "-> " << result << endl;
    return 0;
}
