#include<iostream>
#include<string.h>
using namespace std;

class Task{
    
    string description; 
    string name;    
    int priority;       
    
public:
    
    Task(){
        description="empty";
        name="empty";
        priority=-1;
    }
    
    Task(Task &t){
        this->description = t.getDes();
        this->name = t.getName();
        this->priority = t.getPriority();
    }
    
    ~Task(){}
    
    string getDes(){ 
        
        return description; 
        
    }
    string getName(){
        
        return name; 
        
    }
    int getPriority(){
        
        return priority;
        }
    
    void setDes(string description){ 
        
        this->description = description; 
        
    }
    void setName(string name){ 
        
        this->name = name; 
        
    }
    void setPriority(int priority){
        
        this->priority = priority;
    }
    
    void inputTask(){
        
        int p;
        cout<<"Enter Task priority:"<<endl;
        cout<<"Enter 4 for Highest priority and 1 for Lowest priority"<<endl;

//       setActivityUsingPriority(p);
        while (1) {
            cin>>p;
            cout<<"Enter Task Name: "<<endl;
            cin>>name;
            // cout<<endl;
            if(p==4||p==3||p==2||p==1){
                setPriority(p);
                setTaskUsingPriority(p,name);
                break;
            }else{
                cout<<"Please enter proper priority !!"<<endl;
            }
        }
        cout<<"Enter task description"<<endl;
        cin>>description;
        // getline(cin,description);
        cout<<endl;
    }
    
    void setTaskUsingPriority(int priority, string name){
        
        if (priority==4) {
            setName(name);
        }else if (priority==3) {
            setName("name");
        }else if(priority==2){
            setName("name");
        }else {
            setName("name");
        }
    }
    
    void printTask(){
        
        cout<<"-----------------------------"<<endl;
        cout<<"Task Name: "<<this->name<<endl;
        cout<<"Description: "<<this->description<<endl;
        cout<<"Priority of Task(4-> Highest && 1-> Lowest): "<<this->priority<<endl;
        cout<<"-----------------------------"<<endl;
    }
};

class TaskList{
    
    private: 
    
    string author;
    Task* tasks;
    int maxTasks;
    int numberOfTasks;
    
public:

    TaskList(){
        author="empty";
        tasks=NULL;
    }
    
    TaskList(int maxTasks, string author){
        this->maxTasks=maxTasks;
        numberOfTasks=0;
        tasks = new Task[maxTasks];
        this->author=author;
    }
    
    ~TaskList(){
        delete tasks;
    }
    
      string getAuthor(){ 
        
          return author;
          
          }
    int getPendingTasks(){ 
        
        return numberOfTasks; 
        
    }
    int getMaxTasks(){ 
        
        return maxTasks;
        
    }
    
    void setProfessor(string author){ 
        
        this->author = author; 
        
    }
    void setMaxTasks(int maxTasks){ 
        
        this->maxTasks = maxTasks; 
        
    }
    
    bool LeafNode(int position){
        
        if((2*position+1)>numberOfTasks){
            
            return true;
            
        }
        else{
            
            return false;
        }
    }
    
    int rightChildNode(int position){
        
        return (2*position + 2);
    }
    
    int leftChildNode(int position){
        
        return (2*position + 1);
    }
    
    int parent(int position){
        
        int parent = (int)((position-1)/2);
        return parent;
    }
    
   
    void addTask(const Task& task){
        
        if(numberOfTasks == maxTasks){
            cout<<"OVERFLOW";
        }
        
        else{
            numberOfTasks++;
            tasks[numberOfTasks-1] = task;
            shiftUp(numberOfTasks-1);
        }
        
    }
    
   
    Task removeTask(){
        Task priority;
        priority = extractMax();
        return priority;
    }

    void shiftUp(int position){
        
        while((position>=0)&&((tasks[parent(position)].getPriority())<(tasks[(position)].getPriority()))){
            
            Task temp = tasks[position];
            tasks[position] = tasks[parent(position)];
            tasks[parent(position)] = temp;
            position = parent(position);
            
        }
        
    }
    
    void shiftDown(int position){
        
        int maxIndex = position;
        int l = leftChildNode(position);
        int r = rightChildNode(position);
        
        if((l<numberOfTasks)&&((tasks[l].getPriority())>(tasks[maxIndex].getPriority())))
            maxIndex = l;
        
        if((r<numberOfTasks)&&((tasks[r].getPriority())>(tasks[maxIndex].getPriority())))
            maxIndex = r;
        
        if(position != maxIndex){
            
            Task temp = tasks[position];
            tasks[position] = tasks[maxIndex];
            tasks[maxIndex] = temp;
            shiftDown(maxIndex);
        }
    }
    
    
    Task extractMax(){
        
        if(numberOfTasks==0){
            Task empty;
            return empty;
        }
        
        else{
            Task result = tasks[0];
            tasks[0] = tasks[numberOfTasks-1];
            numberOfTasks--;
            shiftDown(0);
            return result;
        }
    }
         void printTasks(){
        
        Task *temp = new Task[numberOfTasks];
        Task *priority = new Task[numberOfTasks];
;
        int i=0,k=numberOfTasks-1,j = numberOfTasks;
        
        for(i=0;i<j;i++){
            temp[i] = tasks[i];}
        
        for(k=0;k<j;k++){
            priority[k] = extractMax();
        }
        
        cout<<"------------------------------------------------------------"<<endl;
        cout<<"PENDING"<<endl;
        for(i=0;i<j;i++){
            priority[i].printTask();
            tasks[i] = temp[i];
        }
        cout<<"------------------------------------------------------------"<<endl;
        numberOfTasks = j;
    }
};

int main(){
    
    
    int maxTasks;
    string author;
    
    cout<<"Enter maximum number of tasks: ";
    cin>>maxTasks;
    cout<<endl;
    
    cout<<"Enter name of Author: ";
    cin>>author;
    cout<<endl;
    
    TaskList taskList(maxTasks,author);
    Task task;
    
    while(1){
        
        cout<<"--------------------------------------------------"<<endl;
        cout<<"Press 1 to add New Task"<<endl;
        cout<<"Press 2 to complete highest Priority Task"<<endl;
        cout<<"Press 3 to display All Pending Tasks"<<endl;
        cout<<"Press 4 Exit from program"<<endl;
        
        int choice;
        cout<<"Enter your choice"<<endl;
        cin>>choice;
        
        switch (choice) {
            case 1:
                task.inputTask();
                taskList.addTask(task);
                break;
             
            case 2:
                task = taskList.removeTask();
                if(task.getPriority()!=-1){
                    task.printTask();
                    cout<<"REMOVED"<<endl;
                }else{
                    cout<<"There was no task pending"<<endl;}
                break;
                
            case 3:
                taskList.printTasks();
                break;
                
            case 4:
            
                cout<<"EXITING..."<<endl;
                // return 0;
                break;
                
            default:
                break;
        }

    }
    
}

