#include "Constants.h"
#include "engine.h"

#include <chrono> // Include for std::chrono::seconds
#include <thread> // Include for std::this_thread::sleep_for

DEF main() -> int {
    Engine app;

    try {
        fprintf(stdout, "\nTrying to initialize application.\n");
        app.initialize();
        fprintf(stdout, "Finished application Initialization.\n");
    } catch (const std::exception &e) {
        std::cerr << e.what() << "";
        return EXIT_FAILURE;
    }

    fprintf(stdout, "\nStarting mainloop.\n");
    try {
        app.mainLoop(true);
    } catch (const std::exception &e) {
        std::cerr << e.what() << "";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}