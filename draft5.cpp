#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

class Task {
public:
    string name;
    string description;
    string deadline;
    string status;
    bool writtenToCSV; // New field to track if task has been written to CSV

    Task() : name(""), description(""), deadline(""), status("Pending"), writtenToCSV(false) {}

    Task(const string& na, const string& des, const string& dl) : name(na), description(des), deadline(dl), status("Pending"), writtenToCSV(false) {}

    bool operator>(const Task& other) const {
        auto now = system_clock::now();
        auto deadlineTime = system_clock::from_time_t(parseDeadline(deadline));
        auto timeDiff = deadlineTime - now;
        return timeDiff > other.timeUntilDeadline();
    }

    time_t parseDeadline(const string& dl) const {
        size_t pos = dl.find(':');
        if (pos == string::npos) {
            cerr << "Invalid time format." << endl;
            return 0;
        }
        string hourStr = dl.substr(0, pos);
        string minStr = dl.substr(pos + 1);

        int hours, mins;
        try {
            hours = stoi(hourStr);
            mins = stoi(minStr);
        } catch (const invalid_argument& e) {
            cerr << "Invalid time format." << endl;
            return 0;
        } catch (const out_of_range& e) {
            cerr << "Time values out of range." << endl;
            return 0;
        }

        time_t now = system_clock::to_time_t(system_clock::now());
        tm deadline_tm = *localtime(&now);
        deadline_tm.tm_hour = hours;
        deadline_tm.tm_min = mins;

        time_t deadlineTime = mktime(&deadline_tm);
        return deadlineTime;
    }

    duration<double> timeUntilDeadline() const {
        auto now = system_clock::now();
        auto deadlineTime = system_clock::from_time_t(parseDeadline(deadline));
        auto timeDiff = deadlineTime - now;
        return timeDiff;
    }

    void updateStatus(const string& newStatus) {
        status = newStatus;
    }
};

class TaskManager {
private:
    priority_queue<Task, vector<Task>, greater<Task>> pq;
    int maxTasks;

public:
    TaskManager(int max) : maxTasks(max) {}

    void addTask(const string& name, const string& description, const string& deadline) {
        if (pq.size() >= maxTasks) {
            cerr << "Maximum number of tasks reached. Cannot add more tasks." << endl;
            return;
        }
        Task task{name, description, deadline};
        pq.push(task);
    }

    void completeTask(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid task index." << endl;
            return;
        }

        priority_queue<Task, vector<Task>, greater<Task>> tempQueue;
        int i = 1;
        while (!pq.empty()) {
            if (i == index) {
                Task task = pq.top();
                task.updateStatus("Done");
                tempQueue.push(task);
            } else {
                tempQueue.push(pq.top());
            }
            pq.pop();
            ++i;
        }

        pq = tempQueue;
    }

    void doLateDone(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid task index." << endl;
            return;
        }

        priority_queue<Task, vector<Task>, greater<Task>> tempQueue;
        int i = 1;
        while (!pq.empty()) {
            if (i == index) {
                Task task = pq.top();
                task.updateStatus("Late Done");
                tempQueue.push(task);
            } else {
                tempQueue.push(pq.top());
            }
            pq.pop();
            ++i;
        }

        pq = tempQueue;
    }

    void printTasks() {
        cout << "Pending Tasks:" << endl;
        priority_queue<Task, vector<Task>, greater<Task>> pqCopy = pq;
        int index = 1;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            if (task.status == "Pending") {
                cout << index << ". Name: " << task.name << ", Description: " << task.description << ", Deadline: " << task.deadline << ", Status: " << task.status << endl;
                ++index;
            }
            pqCopy.pop();
        }
    }

    void showMissedTasks() {
        auto currentTime = system_clock::to_time_t(system_clock::now());
        priority_queue<Task, vector<Task>, greater<Task>> missedTasks;
        priority_queue<Task, vector<Task>, greater<Task>> pqCopy = pq;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            time_t deadlineTime = task.parseDeadline(task.deadline);
            if (currentTime > deadlineTime && task.status != "Done") {
                task.status = "Missed";
                missedTasks.push(task);
            }
            pqCopy.pop();
        }

        if (missedTasks.empty()) {
            cout << "No missed tasks." << endl;
            return;
        }

        cout << "Missed Tasks:" << endl;
        int index = 1;
        while (!missedTasks.empty()) {
            Task task = missedTasks.top();
            cout << index << ". Name: " << task.name << ", Description: " << task.description << ", Deadline: " << task.deadline << ", Status: " << task.status << endl;
            pq.push(task);
            missedTasks.pop();
            ++index;
        }
    }

    void saveToCSV(const string& filename) {
    ofstream file(filename, ios::trunc); // Truncate existing file or create new
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }

    file << "Task Name,Description,Deadline,Status\n";

    // Iterate through all tasks and find the last status for each task
    unordered_map<string, Task> lastStatusMap;
    priority_queue<Task, vector<Task>, greater<Task>> pqCopy = pq;
    while (!pqCopy.empty()) {
        Task task = pqCopy.top();
        if (!task.writtenToCSV) {
            lastStatusMap[task.name] = task;
        }
        pqCopy.pop();
    }

    // Write the last status of each task to the CSV file
    for (const auto& pair : lastStatusMap) {
        const Task& task = pair.second;
        file << task.name << "," << task.description << "," << task.deadline << "," << task.status << "\n";
    }

    file.close();
}

};

int main() {
    cout << "Enter maximum number of tasks: ";
    int maxTasks;
    cin >> maxTasks;
    cin.ignore();

    TaskManager taskManager(maxTasks);
    string filename = "Task_Manager.csv";

    while (true) {
        cout << "--------------------------------------------------" << endl;
        cout << "Press 1 to add New Task" << endl;
        cout << "Press 2 to complete a Task" << endl;
        cout << "Press 3 to display All Pending Tasks" << endl;
        cout << "Press 4 to show Missed Tasks" << endl;
        cout << "Press 5 to do Late Done for Missed Task" << endl;
        cout << "Press 6 to Exit from program" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

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
                taskManager.printTasks();
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
