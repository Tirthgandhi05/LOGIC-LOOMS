#include<iostream>
#include<string>
#include<vector>

using namespace std;


struct Task{

    string name;
    string description;
    int deadline;

    void disp(){
        cout<<"Task Name:- "<<name<<endl;
        cout<<"Description for the task:- "<<description<<endl;
        cout<<"Deadline for the task(in hours):- "<<deadline<<endl;
    }

};

class taskmanager{

    private:

    vector<Task> list;

    public:

    taskmanager() { 

    }

    void addTask(string& name,string description, int deadline){
        Task task;
        task.name=name;
        task.description=description;
        task.deadline=deadline;
        list.push_back({task.name,task.description,task.deadline});
    }

    void reminder(int currenthour){

        for(Task& task : list){
            if(task.deadline<currenthour){
                cout<<"Reminder: Your deadline for the task " <<task.name<<" is approaching soon. No. of hours left: "<<currenthour-task.deadline<<endl;
                // cout<<"No. of hours left: "<<currenthour-task.deadline<<endl;
            }
            else{
            cout<<"No. of hours for the task " <<task.name<<" left: "<<task.deadline-currenthour<<endl;
        }
        }
    }

    void show(){

        for(Task& task : list){
            task.disp();
        
        }
    }
    
};



int main()
{

    taskmanager t1;
    int n;

    cout<<"Enter no of tasks you want to enter :-";
    cin>>n;

    string name[n], description[n],a;
    int deadline[n];
    int currenthour;
    currenthour=18;

   
    for(int i=0;i<n;i++){


    cout<<"Enter Task Name:- ";
    cin>>name[i];
    cout<<"Enter description for the task:- ";
    cin>>description[i];
    cout<<"Enter Deadline for the task(in hours):- ";
    cin>>deadline[i];

    t1.addTask(name[i],description[i],deadline[i]);
    }

    cout<<"Displaying Your Entries :-"<<endl;
    t1.show();
    t1.reminder(currenthour);
    
    return 0;
}
