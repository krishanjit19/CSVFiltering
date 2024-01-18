#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct CSVRow {
    vector<string> columns;
};

class CSVReader {
private:
    string filename;
    char delimiter;
    vector<CSVRow> data;

public:
    CSVReader(const string& filename, char delimiter = ',') : filename(filename), delimiter(delimiter) {
        data = readCSV();
    }

    vector<CSVRow> readCSV() {
        vector<CSVRow> fileData;
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return fileData;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            CSVRow row;

            while (getline(ss, token, delimiter)) {
                row.columns.push_back(token);
            }

            fileData.push_back(row);
        }

        file.close();
        return fileData;
    }

    vector<CSVRow> filterRows(const string& filterExpression) {
        vector<CSVRow> filteredData;

        for (const auto& row : data) {
            if (evaluateFilterExpression(row, filterExpression)) {
                filteredData.push_back(row);
            }
        }

        return filteredData;
    }

private:
    bool evaluateFilterExpression(const CSVRow& row, const string& filterExpression) {
        map<string, string> columnValues;

        for (size_t i = 0; i < row.columns.size(); ++i) {
            columnValues["column_" + to_string(i + 1)] = row.columns[i];
        }

        return evaluateExpression(filterExpression, columnValues);
    }

    bool evaluateExpression(const string& expression, const map<string, string>& values) {
        string parsedExpression = parseExpression(expression);
        return evaluateParsedExpression(parsedExpression, values);
    }

    string parseExpression(const string& expression) {
        string parsedExpression;
        for (char c : expression) {
            if (c == '(') {
                parsedExpression += "( ";
            } else if (c == ')') {
                parsedExpression += " )";
            } else {
                parsedExpression += c;
            }
        }
        return parsedExpression;
    }

    bool evaluateParsedExpression(const string& parsedExpression, const map<string, string>& values) {
        istringstream iss(parsedExpression);
        string token;
        iss >> token;
        if (token == "(") {
            return evaluateAndOr(iss, values);
        } else {
            return evaluateComparison(token, iss, values);
        }
    }

    bool evaluateAndOr(istringstream& iss, const map<string, string>& values) {
        string token;
        iss >> token;
        bool result = (token == "and");

        while (true) {
            string nextToken;
            iss >> nextToken;
            if (nextToken == "(") {
                result = (token == "and") ? (result && evaluateAndOr(iss, values)) : (result || evaluateAndOr(iss, values));
            } else if (nextToken == ")") {
                break;
            } else {
                result = (token == "and") ? (result && evaluateComparison(nextToken, iss, values)) : (result || evaluateComparison(nextToken, iss, values));
            }
        }

        return result;
    }

    bool evaluateComparison(const string& columnName, istringstream& iss, const map<string, string>& values) {
        string op, value;
        iss >> op >> value;
        string placeholder = "column_" + columnName;
        return evaluateSingleComparison(values.at(placeholder), op, value);
    }

    bool evaluateSingleComparison(const string& columnValue, const string& op, const string& value) {
        if (op == "=") {
            return columnValue == value;
        } else if (op == "!=") {
            return columnValue != value;
        } else if (op == "<") {
            return stoi(columnValue) < stoi(value);
        } else if (op == "<=") {
            return stoi(columnValue) <= stoi(value);
        } else if (op == ">") {
            return stoi(columnValue) > stoi(value);
        } else if (op == ">=") {
            return stoi(columnValue) >= stoi(value);
        } else {
            cerr << "Unsupported operator: " << op << endl;
            return false;
        }
    }
};

int main() {
    string filename = "example.csv";
    CSVReader csvReader(filename);

    string filterExpression = "(((1 = 'practo') and (2 != 'dogreat')) or (3 <= 100))";

    vector<CSVRow> filteredData = csvReader.filterRows(filterExpression);

    for (const auto& row : filteredData) {
        for (const auto& column : row.columns) {
            cout << column << " ";
        }
        cout << endl;
    }

    return 0;
}
