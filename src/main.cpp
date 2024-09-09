#include "Constants.h"
#include "engine.h"

DEF main() -> int {
    Engine app;

    try {
        fprintf(stdout, "\nStarting application run.\n");
        app.run();
        fprintf(stdout, "Finished application run.\n");
    } catch (const std::exception &e) {
        std::cerr << e.what() << "";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
