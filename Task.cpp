#include <bits/stdc++.h>
#include <fstream>
#include<chrono>
using namespace std;
using namespace std::chrono;

class Task {
public:
    string name;
    string description;
    int priority;

    Task() : name(""), description(""), priority(-1) {}

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

    Task removeTask(int index) {
    if (index < 1 || index > pq.size()) {
        cerr << "Invalid index. No task removed." << endl;
        return Task("", "", -1); // Return an empty task
    }

    // Copy tasks to a vector
    vector<Task> tasks;
    while (!pq.empty()) {
        tasks.push_back(pq.top());
        pq.pop();
    }

    // Remove task at the specified index
    Task removedTask = tasks[index - 1];
    tasks.erase(tasks.begin() + index - 1);

    // Rebuild the priority queue
    for (const auto& task : tasks) {
        pq.push(task);
    }

    return removedTask;
}

    void printTasks() {
        cout << "Pending Tasks:" << endl;
        priority_queue<Task, vector<Task>, greater<Task>> pqCopy = pq; // Copy the priority queue
        int index = 1;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            cout << index << ". Name: " << task.name << ", Description: " << task.description << ", Priority: " << task.priority << endl;
            pqCopy.pop();
            ++index;
        }
    }
};

int main() {
    int maxTasks;
    string author;

    cout << "Enter maximum number of tasks: ";
    cin >> maxTasks;
    cout << endl;

    cout << "Enter name of Author: ";
    cin >> author;
    cout << endl;

    TaskManager taskManager;
    Task task;

    while (true) {
        cout << "--------------------------------------------------" << endl;
        cout << "Press 1 to add New Task" << endl;
        cout << "Press 2 to complete a Task" << endl;
        cout << "Press 3 to display All Pending Tasks" << endl;
        cout << "Press 4 to Exit from program" << endl;

        int choice;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Task Name: ";
                cin >> task.name;
                cout << "Enter Task Description: ";
                cin >> task.description;
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
                taskManager.saveToCSV("tasks_sorted.csv");
                cout << "Tasks saved to CSV file: tasks_sorted.csv" << endl;
                cout << "Exiting..." << endl;
                return 0;
                
            default:
                cout << "Invalid choice, please try again." << endl;
                break;
        }
    }
    return 0;
}
