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

class Task {
public:
    string name;
    string description;
    string deadline;
    int priority;

    Task() : name(""), description(""), deadline(""), priority(-1) {}

    Task(const string& na, const string& des, const string& dl, int pri) : name(na), description(des), deadline(dl), priority(pri) {}

    bool operator>(const Task& other) const {
        return priority > other.priority;
    }
};

class TaskManager {
private:
    priority_queue<Task, vector<Task>, greater<Task>> pq;

public:
    TaskManager() {}

    void addTask(const string& name, const string& description, const string& deadline, int priority) {
        Task task{name, description, deadline, priority};
        pq.push(task);
    }

    Task removeTask(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid index. No task removed." << endl;
            return Task("", "", "", -1); 
        }

        vector<Task> tasks;
        while (!pq.empty()) {
            tasks.push_back(pq.top());
            pq.pop();
        }

        Task removedTask = tasks[index - 1];
        tasks.erase(tasks.begin() + index - 1);

        for (const auto& task : tasks) {
            pq.push(task);
        }

        return removedTask;
    }

    void printTasks() {
        cout << "Pending Tasks:" << endl;
        priority_queue<Task, vector<Task>, greater<Task>> pqCopy = pq;
        int index = 1;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            cout << index << ". Name: " << task.name << ", Description: " << task.description << ", Deadline: " << task.deadline << ", Priority: " << task.priority << endl;
            pqCopy.pop();
            ++index;
        }
    }

    void saveToCSV(const string& filename) {
        vector<Task> tasks;
        ifstream existingFile(filename);
        if (existingFile.is_open()) {
            string header;
            getline(existingFile, header);
            string line;
            while (getline(existingFile, line)) {
                stringstream ss(line);
                string name, description, deadline;
                int priority;
                getline(ss, name, ',');
                getline(ss, description, ',');
                getline(ss, deadline, ',');
                ss >> priority;
                tasks.push_back(Task(name, description, deadline, priority));
            }
            existingFile.close();
        }

        while (!pq.empty()) {
            tasks.push_back(pq.top());
            pq.pop();
        }

        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.priority < b.priority;
        });

        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        file << "Task Name,Description,Deadline,Priority\n";
        for (const auto& task : tasks) {
            file << task.name << "," << task.description << "," << task.deadline << "," << task.priority << "\n";
        }
        file.close();
    }
};

int main() {
    TaskManager taskManager;
    string filename = "Task_Manager.csv";

    int maxTasks;

    cout << "Enter maximum number of tasks: ";
    cin >> maxTasks;
    Task task;

    while (true) {
        cout << "--------------------------------------------------" << endl;
        cout << "Press 1 to add New Task" << endl;
        cout << "Press 2 to complete a Task" << endl;
        cout << "Press 3 to display All Pending Tasks" << endl;
        cout << "Press 4 to Exit from program" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Task Name: ";
                cin.ignore();
                getline(cin, task.name);
                cout << "Enter Task Description: ";
                getline(cin, task.description);
                cout << "Enter Task Deadline (hh:mm): ";
                getline(cin, task.deadline);
                cout << "Enter Task Priority: ";
                cin >> task.priority;
                taskManager.addTask(task.name, task.description, task.deadline, task.priority);
                break;

            case 2:
                int index;
                taskManager.printTasks();
                cout << "Enter the index of the task to complete: ";
                cin >> index;
                task = taskManager.removeTask(index);
                if (task.priority != -1) {
                    cout << "Task Removed: " << task.name << endl;
                } else {
                    cout << "No task removed." << endl;
                }
                break;

            case 3:
                taskManager.printTasks();
                break;

            case 4:
                taskManager.saveToCSV(filename);
                cout << "Tasks saved to CSV file: " << filename << endl;
                cout << "Exiting..." << endl;
                return 0;

            default:
                cout << "Invalid choice, please try again." << endl;
                break;
        }
    }

    return 0;
}
