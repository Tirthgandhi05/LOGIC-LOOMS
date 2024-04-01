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
    int priority;

    Task() : name(""), description(""), priority(-1) {}

    Task(const string& na, const string& des, int pri) : name(na), description(des), priority(pri) {}

    bool operator>(const Task& other) const {
        return priority > other.priority;
    }
};

class TaskManager {
private:
    priority_queue<Task, vector<Task>, greater<Task>> pq; // To sort the tasks in ascending- cpp stl syntax

public:
    TaskManager() {}

    void addTask(const string& name, const string& description, int priority) {
        Task task{name, description, priority};
        pq.push(task);
    }

    Task removeTask(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid index. No task removed." << endl;
            return Task("", "", -1); 
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
            cout << index << ". Name: " << task.name << ", Description: " << task.description << ", Priority: " << task.priority << endl;
            pqCopy.pop();
            ++index;
        }
    }

    void saveToCSV(const string& filename) {
        ofstream file(filename);

        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        file << "Task Name,Description,Priority\n";

        vector<Task> tasks;
        while (!pq.empty()) {
            tasks.push_back(pq.top());
            pq.pop();
        }

        for (const auto& task : tasks) {
            file << task.name << "," << task.description << "," << task.priority << "\n";
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
                cout << "Enter Task Priority: ";
                cin >> task.priority;
                taskManager.addTask(task.name, task.description, task.priority);
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
