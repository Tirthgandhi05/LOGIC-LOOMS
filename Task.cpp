#include <bits/stdc++.h>
#include "ExcelFormat.h" // Include the ExcelFormat library
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Task {
    string name;
    string description;
    chrono::time_point<chrono::system_clock> deadline; // Change type to time_point
};

class TaskManager {
private:
    vector<Task> list;

public:
    TaskManager() {}

    void addTask(const string& name, const string& description, const chrono::time_point<chrono::system_clock>& deadline) {
        Task task{name, description, deadline};
        list.push_back(task);
    }

    void sortTasksByDeadline() {
        sort(list.begin(), list.end(), [](const Task& a, const Task& b) {
            return a.deadline < b.deadline;
        });
    }

    void saveToExcel(const string& filename) {
        ExcelFormat::BasicExcel xls;
        xls.New(1); // Create a new Excel file with 1 sheet
        ExcelFormat::BasicExcelWorksheet* sheet = xls.GetWorksheet(0);

        // Set column headers
        sheet->Cell(0, 0)->SetString("Task Name");
        sheet->Cell(0, 1)->SetString("Description");
        sheet->Cell(0, 2)->SetString("Deadline");

        // Write task data to Excel
        for (size_t i = 0; i < list.size(); ++i) {
            sheet->Cell(i + 1, 0)->SetString(list[i].name.c_str());
            sheet->Cell(i + 1, 1)->SetString(list[i].description.c_str());
            // Convert deadline to string for Excel
            auto deadline_time_t = chrono::system_clock::to_time_t(list[i].deadline);
            string deadline_str = ctime(&deadline_time_t);
            sheet->Cell(i + 1, 2)->SetString(deadline_str.c_str());
        }

        // Save the Excel file
        xls.SaveAs(filename.c_str());
    }
};

int main() {
    TaskManager taskManager;
    int n;

    cout << "Enter the number of tasks you want to enter: ";
    cin >> n;

    string name, description;
    int year, month, day, hour, minute;

    for (int i = 0; i < n; ++i) {
        cout << "Enter Task Name: ";
        cin >> name;
        cout << "Enter description for the task: ";
        cin >> description;
        cout << "Enter Deadline for the task (YYYY MM DD HH MM): ";
        cin >> year >> month >> day >> hour >> minute;

        // Construct deadline time_point
        system_clock::time_point deadline = system_clock::from_time_t(0);
        deadline += hours(hour) + minutes(minute);

        taskManager.addTask(name, description, deadline);
    }

    taskManager.sortTasksByDeadline(); // Sort tasks by deadline
    string filename = "tasks_sorted.xlsx"; // Change the filename if needed
    taskManager.saveToExcel(filename);
    cout << "Tasks saved to Excel file: " << filename << endl;

    return 0;
}
