#include "Constants.h"
#include "Util.h"
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

    using clock = std::chrono::high_resolution_clock;
    auto previousTime = clock::now();

    try {
        size_t iterations = 0;
        while (!glfwWindowShouldClose(mainWindow)) {
            std::cout << ++iterations << ". Iteration\n";

            auto currentTime = clock::now();
            std::chrono::duration<float> elapsed = currentTime - previousTime;
            float frameTime = elapsed.count();
            previousTime = currentTime;

            glfwPollEvents();
            game.update(frameTime);

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