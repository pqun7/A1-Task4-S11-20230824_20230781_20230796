#include "VoleMachine.h"

int main() {
    MainUI ui;
    char choice;

    while (true) {
        ui.displayMenu();
        cout << "Enter choice: ";
        cin >> choice;
        ui.inputChoice(choice);
    }

    return 0;
}
