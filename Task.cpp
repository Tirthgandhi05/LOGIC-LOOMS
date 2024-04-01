#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Task {
    string name;
    string description;
    int priority;

    Task(const string& _name, const string& _description, int _priority) : name(_name), description(_description), priority(_priority) {}

    bool operator>(const Task& other) const {
        return priority > other.priority;
    }
};

class TaskManager {
private:
    priority_queue<Task, vector<Task>, greater<Task>> pq;

public:
    TaskManager() {}

    void addTask(const string& name, const string& description, int priority) {
        Task task{name, description, priority};
        pq.push(task);
    }

    void loadFromCSV(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Unable to open file for reading." << endl;
        return;
    }

    // Skip the first line
    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string name, description, priorityStr;
        getline(ss, name, ',');
        getline(ss, description, ',');
        getline(ss, priorityStr, ',');

        try {
            int priority = stoi(priorityStr);
            addTask(name, description, priority);
        } catch (const std::invalid_argument& e) {
            cerr << "Invalid priority value encountered in the file: " << priorityStr << endl;
            // Handle the error, e.g., skip this line or take appropriate action
        }
    }

    file.close();
}
    void saveToCSV(const string& filename) {
        vector<Task> tasks;
        while (!pq.empty()) {
            tasks.push_back(pq.top());
            pq.pop();
        }

        ofstream file(filename);

        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        file << "Task Name,Description,Priority\n";

        for (const auto& task : tasks) {
            file << task.name << "," << task.description << "," << task.priority << "\n";
        }

        file.close();
    }
};

int main() {
    TaskManager taskManager;
    string filename = "tasks_sorted.csv";

    taskManager.loadFromCSV(filename);

    int n;
    cout << "Enter the number of tasks you want to enter: ";
    cin >> n;

    cin.ignore(); // Ignore newline character after entering n

    for (int i = 0; i < n; ++i) {
        string name, description;
        int priority;

        cout << "Enter Task Name for Task " << i + 1 << ": ";
        getline(cin, name);
        cout << "Enter description for Task " << i + 1 << ": ";
        getline(cin, description);
        cout << "Enter Priority for Task " << i + 1 << ": ";
        cin >> priority;

        cin.ignore(); // Ignore newline character after entering priority

        taskManager.addTask(name, description, priority);
    }

    taskManager.saveToCSV(filename);
    cout << "Tasks saved to CSV file: " << filename << endl;

    return 0;
}