#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Row {
    int id;
    string name;
    int age;
};

vector<Row> readCSV(const string& filename) {
    vector<Row> rows;
    ifstream file(filename);
    string line;

    getline(file, line);

    while (getline(file, line)) {
        istringstream iss(line);
        Row row;
        char comma; 
        iss >> row.id >> comma >> row.name >> comma >> row.age;
        rows.push_back(row);
    }

    return rows;
}


bool evaluateFilter(const Row& row, const string& criteria) {
    return row.name == criteria;
}


vector<Row> getFilteredRows(const string& filename, const string& criteria) {
    vector<Row> allRows = readCSV(filename);
    vector<Row> filteredRows;

    for (const auto& row : allRows) {
        if (evaluateFilter(row, criteria)) {
            filteredRows.push_back(row);
        }
    }

    return filteredRows;
}

int main() {
    string filename = "demo.csv";
    string criteria = "adarsh";

    vector<Row> filteredRows = getFilteredRows(filename, criteria);
    for (const auto& row : filteredRows) {
        cout << "ID: " << row.id << "\tName: " << row.name << "\tAge: " << row.age << endl;
    }

    return 0;
}
