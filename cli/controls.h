#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <conio.h>
#endif

class Controls {
public:
    static void init() {
#ifdef __linux__
        setRawMode(true);
#endif
    }

    static void close() {
#ifdef __linux__
        setRawMode(false);
#endif
    }

    static char getch() {
#ifdef __linux__
        return getchar();
#endif
#ifdef _WIN32
        return static_cast<char>(_getch());
#else
        return '\0'; // Fallback
#endif
    }

private:
#ifdef __linux__
    static void setRawMode(bool enable) {
        static struct termios oldt;
        struct termios newt;

        if (enable) {
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        } else {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
    }
#endif
};
