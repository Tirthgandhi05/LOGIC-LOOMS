#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <chrono>
#include <limits> // For std::numeric_limits

using namespace std;
using namespace std::chrono;

class Task {
public:
    string name;
    string description;
    string deadline;
    string status;

    Task(const string& na, const string& des, const string& dl)
        : name(na), description(des), deadline(dl), status("Pending") {}

    bool operator<(const Task& other) const {
        return deadline > other.deadline; // Sorting by deadline, earlier deadlines come first
    }
};

class TaskManager {
private:
    priority_queue<Task> pq;
    int maxTasks;
    string filename;

public:
    TaskManager(int max, const string& file) : maxTasks(max), filename(file) {}

    void addTask(const string& name, const string& description, const string& deadline) {
        if (pq.size() >= maxTasks) {
            cerr << "Maximum number of tasks reached. Cannot add more tasks." << endl;
            return;
        }
        Task task(name, description, deadline);
        pq.push(task);
    }

    void completeTask(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid task index." << endl;
            return;
        }

        priority_queue<Task> tempQueue;
        int i = 1;
        while (!pq.empty()) {
            Task task = pq.top();
            pq.pop();
            if (i == index) {
                task.status = "Done";
            }
            tempQueue.push(task);
            ++i;
        }

        pq = tempQueue;
        saveToCSV();
    }

    void doLateDone(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid task index." << endl;
            return;
        }

        priority_queue<Task> tempQueue;
        int i = 1;
        while (!pq.empty()) {
            Task task = pq.top();
            pq.pop();
            if (i == index) {
                task.status = "Late Done";
            }
            tempQueue.push(task);
            ++i;
        }

        pq = tempQueue;
        saveToCSV();
    }

    void printPendingTasks() const {
        cout << "Pending Tasks:" << endl;
        priority_queue<Task> pqCopy = pq;
        int index = 1;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            if (task.status == "Pending") {
                cout << index << ". Name: " << task.name << ", Description: " << task.description
                     << ", Deadline: " << task.deadline << endl;
                ++index;
            }
            pqCopy.pop();
        }
    }

    void showMissedTasks() const {
        auto currentTime = system_clock::to_time_t(system_clock::now());
        priority_queue<Task> missedTasks;
        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            pqCopy.pop();
            if (task.status != "Done" && task.status != "Late Done") {
                time_t deadlineTime = parseDeadline(task.deadline);
                if (currentTime > deadlineTime) {
                    task.status = "Missed";
                    missedTasks.push(task);
                }
            }
        }

        if (missedTasks.empty()) {
            cout << "No missed tasks." << endl;
            return;
        }

        cout << "Missed Tasks:" << endl;
        int index = 1;
        while (!missedTasks.empty()) {
            Task task = missedTasks.top();
            cout << index << ". Name: " << task.name << ", Description: " << task.description
                 << ", Deadline: " << task.deadline << ", Status: " << task.status << endl;
            missedTasks.pop();
            ++index;
        }
    }

    void saveToCSV() {
        ofstream file(filename, ios::app); 
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        file.seekp(0, ios::end);
        bool isEmpty = file.tellp() == 0;
        if (isEmpty) {
            file << "Task Name,Description,Deadline,Status\n";
        }

        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            file << task.name << "," << task.description << "," << task.deadline << ","
                 << task.status << "\n";
            pqCopy.pop();
        }

        file.close();
    }

private:
    time_t parseDeadline(const string& dl) const {
        stringstream ss(dl);
        int hour, min;
        char colon;
        ss >> hour >> colon >> min;

        time_t now = system_clock::to_time_t(system_clock::now());
        tm deadline_tm = *localtime(&now);
        deadline_tm.tm_hour = hour;
        deadline_tm.tm_min = min;

        return mktime(&deadline_tm);
    }
};

int main() {
    cout << "Enter maximum number of tasks: ";
    int maxTasks;
    cin >> maxTasks;
    cin.ignore();

    cout << "Enter filename to save tasks: ";
    string filename;
    getline(cin, filename);

    TaskManager taskManager(maxTasks, filename);

    while (true) {
        cout << "--------------------------------------------------" << endl;
        cout << "Press 1 to add New Task" << endl;
        cout << "Press 2 to complete a Task" << endl;
        cout << "Press 3 to display Pending Tasks" << endl;
        cout << "Press 4 to show Missed Tasks" << endl;
        cout << "Press 5 to do Late Done for Missed Task" << endl;
        cout << "Press 6 to Exit from program" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (cin.fail()) { // Check if input failed
            cin.clear(); // Clear the error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the invalid input
            cout << "Invalid input. Please enter a valid option." << endl;
            continue;
        }

        switch (choice) {
            case 1: {
                cout << "Enter Task Name: ";
                string name;
                getline(cin, name);
                cout << "Enter Task Description: ";
                string description;
                getline(cin, description);
                cout << "Enter Task Deadline (hh:mm): ";
                string deadline;
                getline(cin, deadline);
                taskManager.addTask(name, description, deadline);
                break;
            }

            case 2: {
                cout << "Enter the index of the task to complete: ";
                int index;
                cin >> index;
                taskManager.completeTask(index);
                break;
            }

            case 3:
                taskManager.printPendingTasks();
                break;

            case 4:
                taskManager.showMissedTasks();
                break;

            case 5: {
                cout << "Enter the index of the missed task to mark as Late Done: ";
                int index;
                cin >> index;
                taskManager.doLateDone(index);
                break;
            }

            case 6:
                taskManager.saveToCSV();
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
