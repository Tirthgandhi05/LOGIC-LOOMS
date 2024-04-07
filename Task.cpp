#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <bits/stdc++.h>


#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace std::chrono;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void setColor(int color) {
#ifndef _WIN32
    cout << "\033[" << color << "m";
#endif
}

void typeText(const string& text) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(milliseconds(50));
    }
}

class Task {
public:
    string name;
    string description;
    string deadline;
    string status;
    int priority;

    Task(const string& na, const string& des, const string& dl, int pri, const string& stat = "Pending")
        : name(na), description(des), deadline(dl), status(stat), priority(pri) {}

    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

class TaskManager {
private:
    priority_queue<Task> pq;
    string filename;
    bool headerWritten;

public:
    TaskManager(const string& file) :  filename(file), headerWritten(false) {
        loadFromCSV(); 
    }

    void addTask(const string &name, const string &description, const string &deadline, int priority)
    {
        regex deadlineRegex("([01]?[0-9]|2[0-3]):[0-5][0-9]");
        if (!regex_match(deadline, deadlineRegex))
        {
            cerr << "Invalid deadline format. Please use the format 'hh:mm'." << endl;
            return;
        }

        Task task(name, description, deadline, priority);
        pq.push(task);
        saveToCSV(task);
    }

    void processTask(int index)
    {
        if (index < 1 || index > pq.size())
        {
            cerr << "Invalid task index." << endl;
            return;
        }

        priority_queue<Task> tempQueue;
        int i = 1;
        auto currentTime = system_clock::to_time_t(system_clock::now());
        while (!pq.empty())
        {
            Task task = pq.top();
            pq.pop();
            if (i == index)
            {
                if (task.status != "Done" && task.status != "Late Done")
                {
                    if (currentTime > parseDeadline(task.deadline))
                    {
                        task.status = "Late Done";
                    }
                    else
                    {
                        task.status = "Done";
                    }
                }
            }
            tempQueue.push(task);
            ++i;
        }

        pq = tempQueue;
        saveToCSV();
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
            typeText("No missed tasks.");cout << endl;
            return;
        }

       typeText("Missed Tasks:");cout << endl;
        int index = 1;
        while (!missedTasks.empty()) {
            Task task = missedTasks.top();
            cout << index << ". Name: " << task.name << ", Description: " << task.description
                 << ", Deadline: " << task.deadline << ", Priority: " << task.priority
                 << ", Status: " << task.status << endl;
            missedTasks.pop();
            ++index;
        }
    }

    void suggestNextTask() const {
        vector<Task> tasks;
        priority_queue<Task> pqCopy = pq;
        
        while (!pqCopy.empty()) {
            tasks.push_back(pqCopy.top());
            pqCopy.pop();
        }

        sort(tasks.begin(), tasks.end(), [this](const Task& a, const Task& b) {
            time_t aDeadlineTime = parseDeadline(a.deadline);
            time_t bDeadlineTime = parseDeadline(b.deadline);
            return aDeadlineTime < bDeadlineTime;
        });

        if (tasks.empty()) {
            typeText("No tasks to suggest.");cout << endl;
            return;
        }

        typeText("You should do the following task first:");
         cout << endl;
        Task nextTask = tasks.front();
        cout << "Name: " << nextTask.name << ", Description: " << nextTask.description
             << ", Deadline: " << nextTask.deadline << ", Priority: " << nextTask.priority << endl;
    }

    vector<Task> remainder() const {
        auto currentTime = system_clock::to_time_t(system_clock::now());
        vector<Task> reminders;

        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            pqCopy.pop();
            time_t deadlineTime = parseDeadline(task.deadline);
            if (deadlineTime >= currentTime && deadlineTime <= currentTime + 3600) { 
                reminders.push_back(task);
            }
        }

        return reminders;
    }
    void removeTask(int index) {
        if (index < 1 || index > pq.size()) {
            cerr << "Invalid task index." << endl;
            return;
        }

        priority_queue<Task> tempQueue;
        int i = 1;
        while (!pq.empty()) {
            Task task = pq.top();
            pq.pop();
            if (i != index) {
                tempQueue.push(task);
            }
            ++i;
        }

        pq = tempQueue;
        saveToCSV(); 
    }

    void modifyDeadline(int index, const string &newDeadline)
    {
        if (index < 1 || index > pq.size())
        {
            cerr << "Invalid task index." << endl;
            return;
        }

        regex deadlineRegex("([01]?[0-9]|2[0-3]):[0-5][0-9]");
        if (!regex_match(newDeadline, deadlineRegex))
        {
            cerr << "Invalid deadline format. Please use the format 'hh:mm'." << endl;
            return;
        }

        priority_queue<Task> tempQueue;
        int i = 1;
        while (!pq.empty())
        {
            Task task = pq.top();
            pq.pop();
            if (i == index)
            {
                task.deadline = newDeadline;
            }
            tempQueue.push(task);
            ++i;
        }

        pq = tempQueue;
        saveToCSV();
    }


private:
    void loadFromCSV() {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for reading." << endl;
        return;
    }

    // Check if the file is empty
    file.seekg(0, ios::end);
    if (file.tellg() == 0) {
        file.close();
        return; // File is empty, no need to load tasks, just return
    }
    file.seekg(0, ios::beg); // Reset file pointer if not empty

    string line;
    getline(file, line);
    if (line != "Task Name,Description,Deadline,Priority,Status") {
        cerr << "Header not found. Adding header..." << endl;
        file.close();
        ofstream outFile(filename, ios::app);
        if (!outFile.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }
        outFile << "Task Name,Description,Deadline,Priority,Status\n";
        outFile.close();
        return;
    }
    while (getline(file, line)) {
        stringstream ss(line);
        string name, description, deadline, status;
        int priority;
        getline(ss, name, ',');
        getline(ss, description, ',');
        getline(ss, deadline, ',');
        ss >> priority;
        ss.ignore(); 
        getline(ss, status, ','); 

        if (status.empty() || (status != "Done" && status != "Late Done" && status != "Pending")) {
            status = "Pending";
        }

        pq.push(Task(name, description, deadline, priority, status));
    }

    file.close();
}

 void saveToCSV(const Task &task)
    {
        ofstream file(filename, ios::app);
        if (!file.is_open())
        {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        file.seekp(0, ios::end);
        if (file.tellp() == 0)
        {

            file << "Task Name,Description,Deadline,Priority,Status\n";
        }

        file << task.name << "," << task.description << "," << task.deadline << ","
             << task.priority << "," << task.status << "\n";

        file.close();
    }

    void saveToCSV() {
        ofstream file(filename, ios::trunc);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }

        file << "Task Name,Description,Deadline,Priority,Status\n";

        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            file << task.name << "," << task.description << "," << task.deadline << ","
                 << task.priority << "," << task.status << "\n";
            pqCopy.pop();
        }

        file.close();
    }

    time_t parseDeadline(const string& dl) const {
        stringstream ss(dl);
        int hour, min;
        char colon;
        ss >> hour >> colon >> min;

        time_t now = time(nullptr);
        tm* deadline_tm = localtime(&now);
        deadline_tm->tm_hour = hour;
        deadline_tm->tm_min = min;
        deadline_tm->tm_sec = 0;

        return mktime(deadline_tm);
    }

public:
    void displayTasks() const {
        cout << "Tasks:" << endl;
        priority_queue<Task> pqCopy = pq;
        int index = 1;
        while (!pqCopy.empty()) {
            Task task = pqCopy.top();
            cout << index << ". Name: " << task.name << ", Description: " << task.description
                 << ", Deadline: " << task.deadline << ", Priority: " << task.priority
                 << ", Status: " << task.status << endl;
            pqCopy.pop();
            ++index;
        }
    }
};

int main() {
    cout << "Enter filename to save tasks: ";
    string filename;
    getline(cin, filename);

    TaskManager taskManager(filename);
    cout << "--------------------------------------------------" << endl;
    

    while (true) {
        cout << "--------------------------------------------------" << endl;
        vector<Task> reminders = taskManager.remainder();
        if (reminders.empty()) {
            cout << "No tasks with deadlines within 1 hour." << endl;
        } 
        else {
            cout << "Tasks with deadlines within 1 hour:" << endl;
            for (const auto& task : reminders) {
                cout << "Name: " << task.name << ", Description: " << task.description<< ", Deadline: " << task.deadline << ", Priority: " << task.priority << endl;
            }
        }
        cout << "--------------------------------------------------" << endl;
        cout << "Press 1 to add New Task" << endl;
        cout << "Press 2 to complete a Task" << endl;
        cout << "Press 3 to show Missed Tasks" << endl;
        cout << "Press 4 to Get A Summary of Your To-Do List" << endl;
        cout << "Press 5 to Display All Tasks" << endl;
        cout << "Press 6 to Modify Deadline of a Task" << endl;
        cout << "Press 7 to Remove a Task" << endl;
        cout << "Press 8 to Exit from program" << endl;

        cout << "Enter your choice: ";

        int choice;
        while (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a valid integer." << endl;

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter your choice: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();

        switch (choice) {
            case 1: {
                while (true) {
                    typeText("Enter Task Name (or type 'exit' to stop adding tasks): ");
                    string name;
                    getline(cin, name);
                    if (name == "exit") {
                        break;
                    }
                    cout << "Enter Task Description: ";
                    string description;
                    getline(cin, description);
                    cout << "Enter Task Deadline (hh:mm): ";
                    string deadline;
                    getline(cin, deadline);
                    cout << "Enter Task Priority: ";
                    int priority;
                while (!(cin >> priority))
                {
                    cout << "Invalid input. Please enter a valid integer." << endl;

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Enter your priority: ";
                }
                    cin.ignore(); 
                    
                    taskManager.addTask(name, description, deadline, priority);
                    cout << "Task added successfully." << endl;
                }
                break;
            }

            case 2: {
                taskManager.displayTasks();
                typeText("Enter the index of the task to process: ");
                int index;
                while (!(cin >> index))
            {
                cout << "Invalid input. Please enter a valid integer." << endl;

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter your index: ";
            }
            cin.ignore();
                taskManager.processTask(index);
                cout << "Task processed." << endl;
                break;
            }

            case 3:
                taskManager.showMissedTasks();
                break;

            case 4:
                taskManager.suggestNextTask();
                break;

            case 5:
                taskManager.displayTasks();
                break;

            case 6:
        {
            taskManager.displayTasks();
            typeText("Enter the index of the task to modify deadline: ");
            int index;
            cin >> index;
            cin.ignore();
            typeText("Enter the new deadline (hh:mm): ");
            string newDeadline;
            getline(cin, newDeadline);
            taskManager.modifyDeadline(index, newDeadline);
            break;
        }

            case 7: {
                taskManager.displayTasks();
                typeText("Enter the index of the task to remove: ");
                int index;
                cin >> index;
                taskManager.removeTask(index);
                cout << "Task removed." << endl;
                break;
            }

            case 8:
                typeText("Exiting...");cout << endl;
                typeText("Thank you for using our programm");cout<<endl;
                return 0;

            default:
                cout << "Invalid choice, please try again." << endl;
                break;
        }
        this_thread::sleep_for(seconds(2));
    }
}
