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

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void setColor(int color)
{
#ifndef _WIN32
    cout << "\033[" << color << "m";
#endif
}

void typeText(const string &text)
{
    for (char c : text)
    {
        cout << c<< flush;
        this_thread::sleep_for(milliseconds(50));
    }
}
int flag = 0;
class Task
{
public:
    string name;
    string description;
    string deadline;
    string status;
    int priority;

    Task(const string &na, const string &des, const string &dl, int pri, const string &stat = "Pending")
        : name(na), description(des), deadline(dl), status(stat), priority(pri) {}

    bool operator<(const Task &other) const
    {
        return priority < other.priority;
    }
};

class TaskManager
{
private:
    priority_queue<Task> pq;
    string filename;
    bool headerWritten;

public:
    TaskManager(const string &file) : filename(file), headerWritten(false)
    {
        loadFromCSV();
    }

    void addTask(const string &name, const string &description, const string &deadline, int priority)
    {
        regex deadlineRegex("([01]?[0-9]|2[0-3]):[0-5][0-9]");
        if (!regex_match(deadline, deadlineRegex))
        {
            cerr << "\u001b[1mInvalid deadline format. Please use the format 'hh:mm'.\u001b[0m" << endl;
            flag = 1;
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
            cerr << "\u001b[1mInvalid task index.\u001b[0m" << endl;
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

    void showMissedTasks() const
    {
        auto currentTime = system_clock::to_time_t(system_clock::now());
        priority_queue<Task> missedTasks;
        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty())
        {
            Task task = pqCopy.top();
            pqCopy.pop();
            if (task.status != "Done" && task.status != "Late Done")
            {
                time_t deadlineTime = parseDeadline(task.deadline);
                if (currentTime > deadlineTime)
                {
                    task.status = "Missed";
                    missedTasks.push(task);
                }
            }
        }

        if (missedTasks.empty())
        {
            typeText("\u001b[1mNo missed tasks.\u001b[0m");
            cout << endl;
            return;
        }
    
         typeText("\u001b[4m\u001b[1mMissed Tasks:\u001b[0m\n");cout<<endl;
       
        int index = 1;
        while (!missedTasks.empty())
        {
            Task task = missedTasks.top();
            cout << index << ". \u001b[1m\u001b[33mName:\u001b[0m " << task.name << ", \u001b[1m\u001b[33mDescription:\u001b[0m " << task.description
                 << ", \u001b[1m\u001b[33mDeadline: \u001b[0m" << task.deadline << ", \u001b[1m\u001b[33mPriority: \u001b[0m" << task.priority
                 << ", \u001b[1m\u001b[33mStatus: \u001b[0m"
                 << "\u001b[1m\u001b[31mMissed\u001b[0m" << endl;
            missedTasks.pop();
            ++index;
        }
    }

    void suggestNextTask() const
    {
        vector<Task> tasks;
        priority_queue<Task> pqCopy = pq;

        while (!pqCopy.empty())
        {
            tasks.push_back(pqCopy.top());
            pqCopy.pop();
        }

        sort(tasks.begin(), tasks.end(), [this](const Task &a, const Task &b)
             {
            time_t aDeadlineTime = parseDeadline(a.deadline);
            time_t bDeadlineTime = parseDeadline(b.deadline);
            return aDeadlineTime < bDeadlineTime; });

        if (tasks.empty())
        {
            typeText("\u001b[1mNo tasks to suggest.\u001b[0m");
            cout << endl;
            return;
        }

        typeText("\u001b[4m\u001b[1mYou should do the following task first:\u001b[0m\n"); 
        cout << endl;
        Task nextTask = tasks.front();
        cout << "\u001b[1m\u001b[33mName: \u001b[0m" << nextTask.name << ", \u001b[1m\u001b[33mDescription: \u001b[0m" << nextTask.description
             << ", \u001b[1m\u001b[33mDeadline: \u001b[0m" << nextTask.deadline << ", \u001b[1m\u001b[33mPriority: \u001b[0m" << nextTask.priority << endl;
    }

    vector<Task> remainder() const
    {
        auto currentTime = system_clock::to_time_t(system_clock::now());
        vector<Task> reminders;

        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty())
        {
            Task task = pqCopy.top();
            pqCopy.pop();
            time_t deadlineTime = parseDeadline(task.deadline);
            if (deadlineTime >= currentTime && deadlineTime <= currentTime + 3600)
            {
                reminders.push_back(task);
            }
        }

        return reminders;
    }
    void removeTask(int index)
    {
        if (index < 1 || index > pq.size())
        {
            cerr << "\u001b[1mInvalid task index.\u001b[0m" << endl;
            return;
        }

        priority_queue<Task> tempQueue;
        int i = 1;
        while (!pq.empty())
        {
            Task task = pq.top();
            pq.pop();
            if (i != index)
            {
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
            cerr << "\u001b[1mInvalid task index.\u001b[0m" << endl;
            return;
        }

        regex deadlineRegex("([01]?[0-9]|2[0-3]):[0-5][0-9]");
        if (!regex_match(newDeadline, deadlineRegex))
        {
            cerr << "\u001b[1mInvalid deadline format. Please use the format 'hh:mm'.\u001b[0m" << endl;
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
    void loadFromCSV()
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            // cerr << "\u001b[1mError: Unable to open file for reading.\u001b[0m" << endl;
            return;
        }

      
        file.seekg(0, ios::end);
        if (file.tellg() == 0)
        {
            file.close();
            return; 
        }
        file.seekg(0, ios::beg);

        string line;
        getline(file, line);
        if (line != "Task Name,Description,Deadline,Priority,Status")
        {
            // cerr << "Header not found. Adding header..." << endl;
            file.close();
            ofstream outFile(filename, ios::app);
            if (!outFile.is_open())
            {
                // cerr << "Error: Unable to open file for writing." << endl;
                return;
            }
            outFile << "Task Name,Description,Deadline,Priority,Status\n";
            outFile.close();
            return;
        }
        while (getline(file, line))
        {
            stringstream ss(line);
            string name, description, deadline, status;
            int priority;
            getline(ss, name, ',');
            getline(ss, description, ',');
            getline(ss, deadline, ',');
            ss >> priority;
            ss.ignore();
            getline(ss, status, ',');

            if (status.empty() || (status != "Done" && status != "Late Done" && status != "Pending"))
            {
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
            cerr << "\u001b[1mError: Unable to open file for writing.\u001b[0m" << endl;
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

    void saveToCSV()
    {
        ofstream file(filename, ios::trunc);
        if (!file.is_open())
        {
            cerr << "\u001b[1mError: Unable to open file for writing.\u001b[0m" << endl;
            return;
        }

        file << "Task Name,Description,Deadline,Priority,Status\n";

        priority_queue<Task> pqCopy = pq;
        while (!pqCopy.empty())
        {
            Task task = pqCopy.top();
            file << task.name << "," << task.description << "," << task.deadline << ","
                 << task.priority << "," << task.status << "\n";
            pqCopy.pop();
        }

        file.close();
    }

    time_t parseDeadline(const string &dl) const
    {
        stringstream ss(dl);
        int hour, min;
        char colon;
        ss >> hour >> colon >> min;

        time_t now = time(nullptr);
        tm *deadline_tm = localtime(&now);
        deadline_tm->tm_hour = hour;
        deadline_tm->tm_min = min;
        deadline_tm->tm_sec = 0;

        return mktime(deadline_tm);
    }

public:
    void displayTasks() const
    {
        typeText("\u001b[4m\u001b[1mTasks:\u001b[0m\n");cout << endl;
        priority_queue<Task> pqCopy = pq;
        int index = 1;
        while (!pqCopy.empty())
        {
            Task task = pqCopy.top();
            cout << index << ". \u001b[1m\u001b[33mName:\u001b[0m " << task.name << ", \u001b[1m\u001b[33mDescription:\u001b[0m " << task.description
                 << ", \u001b[1m\u001b[33mDeadline:\u001b[0m " << task.deadline << ", \u001b[1m\u001b[33mPriority: \u001b[0m" << task.priority
                 << ", \u001b[1m\u001b[33mStatus:\u001b[0m ";
                   
                 if(task.status == "Pending") cout<<"\u001b[1m\u001b[31mPending\u001b[0m"<< endl;
                else if(task.status == "Done") cout<<"\u001b[1m\u001b[32mDone\u001b[0m"<< endl;
                else if(task.status == "Late Done") cout<<"\u001b[1m\u001b[36mLate Done\u001b[0m"<< endl;
            pqCopy.pop();
            ++index;
        }
    }
};

int main()
{
    cout << "\u001b[1mEnter filename to save tasks: \u001b[0m";
    string filename;
    getline(cin, filename);

    TaskManager taskManager(filename);
    cout << "\u001b[1m+------------------------------------------------+\u001b[0m" << endl;

    while (true)
    {
        cout << "\u001b[1m+------------------------------------------------+\u001b[0m" << endl;
        vector<Task> reminders = taskManager.remainder();
        if (reminders.empty())
        {
            cout << "\u001b[1mNo tasks with deadlines within 1 hour.\u001b[0m" << endl;
        }
        else
        {
            cout << "\u001b[1mTasks with deadlines within 1 hour:\u001b[0m" << endl;
            for (const auto &task : reminders)
            {
                cout << "\u001b[1m\u001b[33mName\u001b[0m: " << task.name << ", \u001b[1m\u001b[33mDescription\u001b[0m: " << task.description << ", \u001b[1m\u001b[33mDeadline\u001b[0m: " << task.deadline << ", \u001b[1m\u001b[33mPriority\u001b[0m: " << task.priority << endl;
                // cout << "Name: " << task.name << ", Description: " << task.description << ", Deadline: " << task.deadline << ", Priority: " << task.priority << endl;

            }
        }

        cout << "\u001b[1m+------------------------------------------------+\u001b[0m" << endl;
        cout << "\u001b[1m| Press 1 to Add New Task                        |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 2 to Complete a Task                     |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 3 to Show Missed Tasks                   |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 4 to Get A Summary of Your To-Do List    |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 5 to Display All Tasks                   |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 6 to Modify Deadline of a Task           |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 7 to Remove a Task                       |\u001b[0m" << endl;
        cout << "\u001b[1m| Press 8 to Exit From Program                   |\u001b[0m" << endl;
        cout << "\u001b[1m+------------------------------------------------+\u001b[0m" << endl;
        cout << "\u001b[1mEnter your choice: \u001b[0m";

        int choice;
        while (!(cin >> choice))
        {
            cout << "\u001b[1mInvalid input. Please enter a valid integer.\u001b[0m" << endl;

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\u001b[1mEnter your choice: \u001b[0m";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();

        switch (choice)
        {
        case 1:
        {
            while (true)
            {
                typeText("\u001b[1mEnter Task Name (or type 'exit' to stop adding tasks): \u001b[0m");
                string name;
                getline(cin, name);
                if (name == "exit")
                {
                    break;
                }
                cout << "\u001b[1mEnter Task Description:\u001b[0m ";
                string description;
                getline(cin, description);
                cout << "\u001b[1mEnter Task Deadline (hh:mm): \u001b[0m";
                string deadline;
                getline(cin, deadline);
                cout << "\u001b[1mEnter Task Priority:\u001b[0m ";
                int priority;
                while (!(cin >> priority))
                {
                    cout << "\u001b[1mInvalid input. Please enter a valid integer.\u001b[0m" << endl;

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\u001b[1mEnter your priority: \u001b[0m";
                }
                cin.ignore();

                taskManager.addTask(name, description, deadline, priority);
                if (flag)
                    cout << "\u001b[1mTask not added.\u001b[0m" << endl;
                else
                    cout << "\u001b[1mTask added successfully.\u001b[0m" << endl;
                flag = 0;
            }
            break;
        }

        case 2:
        {
            taskManager.displayTasks();
            typeText("\u001b[1mEnter the index of the task to process:\u001b[1m");
            int index;
            while (!(cin >> index))
            {
                cout << "\u001b[1mInvalid input. Please enter a valid integer.\u001b[0m" << endl;

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\u001b[1mEnter your index:\u001b[0m ";
            }
            cin.ignore();
            taskManager.processTask(index);
            cout << "\u001b[1mTask processed.\u001b[0m" << endl;
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
            typeText("\u001b[1mEnter the index of the task to modify deadline:\u001b[0m");
            int index;
            cin >> index;
            cin.ignore();
            typeText("\u001b[1mEnter the new deadline (hh:mm):\u001b[0m ");
            string newDeadline;
            getline(cin, newDeadline);
            taskManager.modifyDeadline(index, newDeadline);
            break;
        }

        case 7:
        {
            taskManager.displayTasks();
            typeText("\u001b[1mEnter the index of the task to remove:\u001b[0m ");
            int index;
            cin >> index;
            taskManager.removeTask(index);
            cout << "\u001b[1mTask removed.\u001b[0m" << endl;
            break;
        }

        case 8:
            typeText("\u001b[1mExiting...\u001b[0m");
            cout << endl;
            typeText("\u001b[1mThank you for using our program. \u001b[0m");
            cout << endl;
            cout << "\u001b[1m\nx - x - x - x - x - x - x - x - x - x \u001b[1m" << endl;
            return 0;

        default:
            cout << "\u001b[1mInvalid choice, please try again.\u001b[1m\n" << endl;
            break;
        }
        this_thread::sleep_for(seconds(2));
    }
}
