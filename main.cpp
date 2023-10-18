#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <utility>
#include <sstream>

using namespace std;

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string RESET = "\033[0m"; // Reset to default color


struct Task {
    string name;
    uint16_t priority;
    uint16_t taskTime;  // время поступления задачи
    uint16_t durationTime;
    uint16_t type;

    Task() : name(""), priority(0), taskTime(0), durationTime(0), type(0) {}

};

struct TaskList {
    Task taskValues;
    TaskList *next = nullptr;
};

struct Processor {
    std::string name;
    Task *currentTask;
    int16_t supportedTaskType;
    bool isFree;

    Processor(std::string name, int16_t taskType) :
            name(std::move(name)), currentTask(new Task()), supportedTaskType(taskType), isFree(true) {
    }
};


const int STACK_SIZE = 10;  // Maximum stack size
Task staticStack[STACK_SIZE];
int staticStackTop = -1;

TaskList *dynamicQueueFront = nullptr;
TaskList *dynamicQueueRear = nullptr;


Processor processor("Processor1", 1);
Processor processor2("Processor2", 2);
Processor processor3("Processor3", 3);


void showTask(Task &task) {
    cout << "Задача: " << task.name << " Приоритет выполнения: " << task.priority
         << " Момент поступления: " << task.taskTime << " Время исполнения: " << task.durationTime << " Тип: "
         << task.type << endl;
}
void showTaskList(TaskList *taskList) {
    TaskList *current = taskList;
    while (current != nullptr) {
        cout << "Задача: " << current->taskValues.name << " Приоритет выполнения: " << current->taskValues.priority
             << " Момент поступления: " << current->taskValues.taskTime << " Время исполнения: " <<
             current->taskValues.durationTime << endl;
        current = current->next;
    }
}
void showStack() {
    for (int i = 0; i <= staticStackTop; i++) {
        Task task = staticStack[i];
        cout << "Задача: " << task.name << " Приоритет выполнения: " << task.priority
             << " Момент поступления: " << task.taskTime << " Время исполнения: " << task.durationTime  << " Тип: " << task.type << endl;
    }
}
void showProcessorStats(const Processor &processorToView) {

    if (!processorToView.isFree) {
        cout << BLUE << "Содержимое процессора " << processorToView.name << RESET << endl;
        showTask(*processorToView.currentTask);
    } else
        cout << BLUE << "Процессор " << processorToView.name << " свободен" << RESET << endl;
}





Task *generateRandomTask(int taskIndex, int taskOrder) {
    Task *task = new Task;
    task->name = "Task: " + std::to_string(taskIndex);
    task->priority = 1 + rand() % 3;
    task->taskTime = taskOrder;
    task->durationTime = 1 + rand() % 7;
    task->type = 1 + rand() % 3;
    return task;
}
void addToTaskList(Task *task, TaskList *&head, TaskList *&current) {
    TaskList *newTaskNode = new TaskList{*task, nullptr};

    if (head == nullptr) {
        head = newTaskNode;
        current = head;
    } else {
        current->next = newTaskNode;
        current = newTaskNode;
    }
}
TaskList *generateTaskList() {
    const int numTasks = 10;
    TaskList *head = nullptr;
    TaskList *current = nullptr;

    // Инициализация генератора случайных чисел с использованием текущего времени
    std::srand(static_cast<unsigned>(std::time(0)));
    for (int i = 0; i < numTasks; i++) {
        Task *task = generateRandomTask(i + 1, i + 1);
        addToTaskList(task, head, current);
    }

    return head;
}


void getFromStack(Task &processor, bool &emptyStack, bool &isFree) {
    if (staticStackTop >= 0) {
        processor = staticStack[staticStackTop];
        isFree = false;
        staticStackTop -= 1;
        if (staticStackTop == -1) {
            emptyStack = true;
        }
    }
}
void pushToStack(Task *incomingTask, bool &emptyStack) {
    if (staticStackTop < STACK_SIZE - 1) {
        staticStackTop = staticStackTop + 1;
        staticStack[staticStackTop] = *incomingTask;
        emptyStack = false;
    }
}


void pushToQueue(TaskList &incomingTask, bool &emptyQueue, bool &allTasksGone) {

    TaskList *newTaskNode = new TaskList{incomingTask.taskValues, nullptr};
    if (dynamicQueueRear == nullptr) {
        dynamicQueueFront = dynamicQueueRear = newTaskNode;
    } else {
        dynamicQueueRear->next = newTaskNode;
        dynamicQueueRear = newTaskNode;
    }
    emptyQueue = false;
    if (incomingTask.next == nullptr) {
        allTasksGone = true;
        return;
    }
    incomingTask = *incomingTask.next;
}
void getFromQueue(Task &processor, bool &emptyQueue, bool &processorIsFree) {
    if (dynamicQueueFront != nullptr) {
        processor = dynamicQueueFront->taskValues;
        TaskList *temp = dynamicQueueFront;
        dynamicQueueFront = dynamicQueueFront->next;
        delete temp;
        if (dynamicQueueFront == nullptr) {
            dynamicQueueRear = nullptr;
            emptyQueue = true;
        }
        processorIsFree = false;
    }
}


Processor getProcessorByTaskList(const TaskList &incomingTask, bool emptyQueue) {
    if (emptyQueue) { // определяем по последнему элементу стека
        Task task = staticStack[staticStackTop];
        if (task.type == processor.supportedTaskType) return processor;
        if (task.type == processor2.supportedTaskType) return processor2;
        if (task.type == processor3.supportedTaskType) return processor3;
    } else {  // определяем по первому элементу из очереди
        if (processor.supportedTaskType == incomingTask.taskValues.type) return processor;
        else if (processor2.supportedTaskType == incomingTask.taskValues.type) return processor2;
        return processor3;
    }
}
void processorTick(Processor &processor) {
    if (!processor.isFree) {
        processor.currentTask->durationTime--;
    }

    if (processor.currentTask->durationTime <= 0) {
        processor.currentTask->durationTime = 0;
        processor.currentTask->priority = 0;
        processor.isFree = true;
    }
}
void setProcessorUpdate(Processor currentProcessor, Processor &pr1, Processor &pr2, Processor &pr3) {
    if (currentProcessor.supportedTaskType == pr1.supportedTaskType) pr1 = currentProcessor;
    else if (currentProcessor.supportedTaskType == pr2.supportedTaskType) pr2 = currentProcessor;
    else pr3 = currentProcessor;
}



void processorLoop(TaskList *&incomingTask) {
    bool emptyQueue = true; //Проверка пустоты Очереди
    bool emptyStack = true; //Проверка пустоты Стека
    bool allTasksGone = false;
    int timer = 1;
    while (!(emptyStack && emptyQueue && processor.isFree && processor2.isFree && processor3.isFree && allTasksGone)) {

        if (!allTasksGone && incomingTask->taskValues.taskTime == timer) {
            pushToQueue(*incomingTask, emptyQueue, allTasksGone);
        }
        Processor currentProcessor = getProcessorByTaskList(*dynamicQueueFront, emptyQueue);
        if (!emptyQueue) {

            if (dynamicQueueFront->taskValues.priority > currentProcessor.currentTask->durationTime ||
                currentProcessor.isFree) {
                if (currentProcessor.currentTask->durationTime > 0)
                    pushToStack(currentProcessor.currentTask, emptyStack);

                getFromQueue(*currentProcessor.currentTask, emptyQueue, currentProcessor.isFree);
                while (true) {
                    if (!emptyQueue) {

                        if (dynamicQueueFront->taskValues.priority > currentProcessor.currentTask->priority) {
                            pushToStack(currentProcessor.currentTask, emptyStack);
                            getFromQueue(*currentProcessor.currentTask, emptyQueue, currentProcessor.isFree);
                        } else
                            break;
                    } else
                        break;
                }
            }
        } else if (!emptyStack) {
            if (currentProcessor.isFree)
                getFromStack(*currentProcessor.currentTask, emptyStack, currentProcessor.isFree);

        }

        cout << endl << GREEN << "Идет " << timer << " такт" << RESET << endl;
        if (!allTasksGone) {
            cout << BLUE << "Входные задания" << RESET << endl;
            showTaskList(incomingTask);
        }
        if (!emptyStack) {
            cout << BLUE << "Содержимое стэка" << RESET << endl;
            showStack();
        }
        if (!emptyQueue) {
            cout << BLUE << "Содержимое очереди" << RESET << endl;
            showTaskList(dynamicQueueFront);
        }

        setProcessorUpdate(currentProcessor, processor, processor2, processor3);


        showProcessorStats(processor);
        showProcessorStats(processor2);
        showProcessorStats(processor3);

        processorTick(processor);
        processorTick(processor2);
        processorTick(processor3);
        timer++;
    }


}

TaskList *manualInput() {
    string input;
    int tick = 0;
    TaskList *head = nullptr;
    TaskList *current = nullptr;
    string divizionString = "\n--------------------------------------------------------------"
                            "--------------------------------------------------------------\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    while (input != "END") {
        cout << divizionString << "Введите задачу в формате (name priority taskTime durationTime type)\nПример:\nТест 1 3 2 3\n\n"
                "Вводите в порядке увеличения taskTime, type должен соответствовать числу из набора {1, 2, 3}.\nДля окончания ввода напишите `END`.\nВвод: ";

        std::getline(std::cin, input);

        if (input == "END") {
            break;  // Выход из цикла при вводе "END"
        }

        Task *task = new Task;  // Создаем новый экземпляр Task

        std::istringstream iss(input);

        if (iss >> task->name >> task->priority >> task->taskTime >> task->durationTime >> task->type) {
            if (task->taskTime <= tick) {
                cout << RED << "\n\n\nTask Time должен быть > " << tick << "!" << RESET << std::endl;
            } else if (task->type != 1 && task->type != 2 && task->type != 3) {
                cout << RED << "\n\n\nTask Type должен быть числом из {1, 2, 3}!" << RESET <<  std::endl;
            } else {
                // Все значения успешно считаны в структуру Task
                std::cout << GREEN << "\n\n\nСтруктура Task успешно заполнена:" << RESET;
                std::cout << BLUE << "\nИмя: " << task->name << ", ";
                std::cout << "Приоритет: " << task->priority << ", ";
                std::cout << "Время поступления задачи: " << task->taskTime << ", ";
                std::cout << "Длительность задачи: " << task->durationTime << ", ";
                std::cout << "Тип: " << task->type << "." << RESET;
                tick++;
                addToTaskList(task, head, current);
            }
        } else {
            std::cout << RED << "\n\n\nОшибка: Невозможно разобрать ввод." << RESET <<  std::endl;
        }
    }

    return head;
}


int main() {
    int type = 3;
    do {
        cout << "Введите тип взаимодействия:\n1 - автоматическое заполнение тактов\n2 - ручной ввод\nВыбор: ";
        cin >> type;
    } while (type != 1 and type != 2);

    TaskList *taskList = nullptr;

    if (type == 1) taskList = generateTaskList();
    else taskList = manualInput();

    processorLoop(taskList);
    return 0;
}
