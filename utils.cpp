#include "utils.h"

void curataCin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int citesteOptiune(const char *prompt) {
    int opt;
    std::cout << prompt;
    if (!(std::cin >> opt)) {
        if (std::cin.eof()) {
            return 0; // Exit gracefully on EOF
        }
        curataCin();
        return -1;
    }
    curataCin();
    return opt;
}

