#include "Constants.h"
#include "engine.h"
#include "game.h"

DEF main() -> int {
    Engine engine;

    try {
        fprintf(stdout, "\nTrying to initialize application.\n");
        engine.initialize();
        fprintf(stdout, "Finished application Initialization.\n");
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    GLFWwindow *mainWindow = engine.getWindow();
    Game game(&engine);

    fprintf(stdout, "\nStarting mainloop.\n");

    try {
        size_t iterations = 0;
        while (!glfwWindowShouldClose(mainWindow)) {
            std::cout << ++iterations << ". Iteration\n";

            glfwPollEvents();

            game.handleInput();

            // TODO: Use a mutex to avoid dirty reads

            TIMED_EXECUTION(engine.drawFrame)
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // Clean up the main window
    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return EXIT_SUCCESS;
}