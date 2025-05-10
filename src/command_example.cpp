#include <iostream>
#include <memory>
#include <vector>
#include <stack>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <string>

// Базовый интерфейс для всех команд
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
};

// Получатель команды
class Device {
private:
    std::string name_;
    bool state_ = false;

public:
    Device(const std::string& name) : name_(name) {}
    
    void turnOn() {
        state_ = true;
        std::cout << name_ << " включен\n";
    }
    
    void turnOff() {
        state_ = false;
        std::cout << name_ << " выключен\n";
    }
    
    bool isOn() const { return state_; }
};

// Конкретные команды
class TurnOnCommand : public Command {
private:
    Device& device_;
    
public:
    TurnOnCommand(Device& device) : device_(device) {}
    
    void execute() override {
        device_.turnOn();
    }
    
    void undo() override {
        device_.turnOff();
    }
    
    std::string getDescription() const override {
        return "Включить " + std::to_string(device_.isOn());
    }
};

class TurnOffCommand : public Command {
private:
    Device& device_;
    
public:
    TurnOffCommand(Device& device) : device_(device) {}
    
    void execute() override {
        device_.turnOff();
    }
    
    void undo() override {
        device_.turnOn();
    }
    
    std::string getDescription() const override {
        return "Выключить " + std::to_string(device_.isOn());
    }
};

// Обработчик команд
class CommandManager {
private:
    std::stack<std::unique_ptr<Command>> history_;
    std::queue<std::unique_ptr<Command>> commandQueue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool shouldStop_ = false;
    
public:
    void addCommand(std::unique_ptr<Command> cmd) {
        std::lock_guard<std::mutex> lock(mutex_);
        commandQueue_.push(std::move(cmd));
        condition_.notify_one();
    }
    
    void processCommands() {
        while (true) {
            std::unique_ptr<Command> cmd;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this] { 
                    return shouldStop_ || !commandQueue_.empty(); 
                });
                
                if (shouldStop_ && commandQueue_.empty()) {
                    return;
                }
                
                cmd = std::move(commandQueue_.front());
                commandQueue_.pop();
            }
            
            std::cout << "Выполняется команда: " << cmd->getDescription() << std::endl;
            cmd->execute();
            history_.push(std::move(cmd));
        }
    }
    
    void undoLastCommand() {
        if (history_.empty()) {
            std::cout << "История команд пуста\n";
            return;
        }
        
        auto cmd = std::move(history_.top());
        history_.pop();
        std::cout << "Отмена команды: " << cmd->getDescription() << std::endl;
        cmd->undo();
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        shouldStop_ = true;
        condition_.notify_all();
    }
};

int main() {
    Device lamp("Лампа");
    CommandManager manager;
    
    // Запускаем обработчик команд в отдельном потоке
    std::thread worker([&manager]() {
        manager.processCommands();
    });
    
    // Добавляем команды в очередь
    manager.addCommand(std::make_unique<TurnOnCommand>(lamp));
    manager.addCommand(std::make_unique<TurnOffCommand>(lamp));
    
    // Даем время на выполнение команд
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Отменяем последнюю команду
    manager.undoLastCommand();
    
    // Останавливаем обработчик команд
    manager.stop();
    worker.join();
    
    return 0;
}
