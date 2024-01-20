#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>

using namespace std;

struct Row {
    vector<string> columns;
};

struct TreeNode {
    string value;
    vector<TreeNode*> children;
};

class CSVReader {
public:
    CSVReader(const string& filename) : filename(filename) {}

    bool readCSV() {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return false;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string token;
            Row row;

            while (getline(iss, token, ',')) {
                row.columns.push_back(token);
            }

            rows.push_back(row);
        }

        file.close();
        return true;
    }

    const vector<Row>& getRows() const {
        return rows;
    }

private:
    vector<Row> rows;
    string filename;
};

class FilterExpressionParser {
public:
    FilterExpressionParser(const string& expression) : expression(expression), currentPos(0) {}

    TreeNode* parseExpression() {
        return parseOr();
    }

private:
    string expression;
    size_t currentPos;

    char getCurrentChar() const {
        return (currentPos < expression.size()) ? expression[currentPos] : '\0';
    }

    void consumeChar() {
        if (currentPos < expression.size()) {
            ++currentPos;
        }
    }

    TreeNode* parseOr() {
        TreeNode* left = parseAnd();
        while (getCurrentChar() == 'o' || getCurrentChar() == 'O') {
            consumeChar();  // Consume 'o' or 'O'
            consumeChar();  // Consume 'r' or 'R'
            TreeNode* right = parseAnd();
            TreeNode* orNode = new TreeNode;
            orNode->value = "OR";
            orNode->children.push_back(left);
            orNode->children.push_back(right);
            left = orNode;
        }
        return left;
    }

    TreeNode* parseAnd() {
        TreeNode* left = parseComparison();
        while (getCurrentChar() == 'a' || getCurrentChar() == 'A') {
            consumeChar();  // Consume 'a' or 'A'
            consumeChar();  // Consume 'n' or 'N'
            consumeChar();  // Consume 'd' or 'D'
            TreeNode* right = parseComparison();
            TreeNode* andNode = new TreeNode;
            andNode->value = "AND";
            andNode->children.push_back(left);
            andNode->children.push_back(right);
            left = andNode;
        }
        return left;
    }

    TreeNode* parseComparison() {
        if (getCurrentChar() == '(') {
            consumeChar();  // Consume '('
            TreeNode* innerExpression = parseOr();
            consumeChar();  // Consume ')'
            return innerExpression;
        } else {
            return parseSimpleComparison();
        }
    }

    TreeNode* parseSimpleComparison() {
        TreeNode* node = new TreeNode;
        // Assume a simple comparison for demonstration purposes
        while (isalnum(getCurrentChar()) || getCurrentChar() == '_') {
            node->value += getCurrentChar();
            consumeChar();
        }
        return node;
    }
};

class FilterExpressionEvaluator {
public:
    FilterExpressionEvaluator(TreeNode* root) : root(root) {}

    bool evaluateExpression(const Row& row) const {
        return evaluateNode(root, row);
    }

private:
    TreeNode* root;

    bool evaluateNode(const TreeNode* node, const Row& row) const {
        if (node->value == "AND") {
            for (const TreeNode* child : node->children) {
                if (!evaluateNode(child, row)) {
                    return false;
                }
            }
            return true;
        } else if (node->value == "OR") {
            for (const TreeNode* child : node->children) {
                if (evaluateNode(child, row)) {
                    return true;
                }
            }
            return false;
        } else {
            // Assume leaf node represents a column name for simplicity
            return find(row.columns.begin(), row.columns.end(), node->value) != row.columns.end();
        }
    }
};

void postorderTraversal(const TreeNode* node) {
    if (node == nullptr) {
        return;
    }

    for (const TreeNode* child : node->children) {
        postorderTraversal(child);
    }

    cout << node->value << " ";
}

int main() {
    // Example usage:
    CSVReader csvReader("example.csv");
    if (csvReader.readCSV()) {
        string filterExpression = "(((column_name = 'practo') and (column_name != 'dogreat')) or (column_name <= 100))";
        FilterExpressionParser parser(filterExpression);
        TreeNode* root = parser.parseExpression();

        cout << "Postorder Traversal of the Expression Tree: ";
        postorderTraversal(root);
        cout << endl;

        FilterExpressionEvaluator evaluator(root);

        // Filter and process rows as needed
        for (const Row& row : csvReader.getRows()) {
            if (evaluator.evaluateExpression(row)) {
                // Process the matching row
                for (const string& column : row.columns) {
                    cout << column << "\t";
                }
                cout << endl;
            }
        }
    }

    return 0;
}
