#include <iostream>


class Parent {
    public:
        std::string name;
        int age;

        Parent(std::string name, int age) {
            this->name = name;
            this->age = age;
        }

        bool canVote() const {
            return this->isAdult();
        }

        friend std::ostream& operator<<(std::ostream& os, const Parent& parent) {
            os << "Name: " << parent.name << ", Age: " << parent.age;
            os << ", Can vote: " << std::boolalpha << parent.canVote();
            return os;
        }

    private:
        bool isAdult() const {
            return this->age >= 18;
        }
};

struct Child : public Parent {
    Parent* parent;

    Child(std::string name, int age, Parent* parent) : Parent(name, age) {
        this->parent = parent;
    }

    friend std::ostream& operator<<(std::ostream& os, const Child& child) {
        os << "Name: " << child.name << ", Age: " << child.age;
        os << ", Parent: " << child.parent->name << ", Parent age: " << child.parent->age;
        os << ", Can vote: " << std::boolalpha << child.canVote();
        return os;
    }
};


int main() {
    Parent person1 = Parent("John", 32);
    Child person2 = Child("Jane", 15, &person1);

    std::cout << person1 << std::endl;
    std::cout << person2 << std::endl;

    return 0;
}