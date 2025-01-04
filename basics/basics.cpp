#include <iostream>

#define ENDL std::endl

const int repeats = 5;

void printText() {
    std::string long_text = "Enter" " the \
number of repeats.";
    std::cout << long_text << ENDL;
}

int* createArray(int length=repeats) {
    int* list = new int[length];
    for (auto i = 0; i < length; i++) {
        list[i] = length - i;
    }

    return list;
}

void printArray(int* list, int length) {
    for (auto i = 0; i < length; i++) {
        std::cout << list[i] << " ";
    }
    std::cout << ENDL;
}

int main() {
    int userRepeats;

    printText();
    std::cin >> userRepeats;

    if (std::cin.fail()) {
        std::cout << "Enter a valid integer." << ENDL;
    } else {
        int* list = createArray(userRepeats);
        printArray(list, userRepeats);
    }

    return 0;
}