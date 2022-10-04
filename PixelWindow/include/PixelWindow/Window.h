#pragma once

#include <GLFW/glfw3.h>

#include <vector>
#include <functional>

namespace pw {

    namespace internal {
        class Glfw {
        private:
        public:
            Glfw();
            ~Glfw();
        };
    }

    class Window {
    private:
        const static internal::Glfw glfw;

        GLFWwindow* handle = nullptr;

        std::vector<std::function<void(int, int)>> windowResizeCallbacks;
        std::vector<std::function<void(void)>> windowRefreshCallbacks;
        std::vector<std::function<void(int, int, int)>> windowMouseCallbacks;

        static void windowSizeCallback(GLFWwindow* window, int width, int height);
        static void windowRefreshCallback(GLFWwindow* window);
        static void windowMouseCallback(GLFWwindow* window, int button, int action, int modes);

    public:
        explicit Window(int width, int height, const char* title);
        ~Window();

        Window(const Window& other) = delete;
        Window(Window&& other) noexcept;

        Window& operator=(const Window& other) = delete;
        Window& operator=(Window&& other) noexcept;

        friend void swap(Window& w1, Window& w2) noexcept;

        bool isActive() const noexcept;
        void pollEvents() const noexcept;
        void makeCurrent() const noexcept;
        void swapBuffers() const noexcept;
        void forceClose() noexcept;

        void addResizeCallback(std::function<void(int,int)> callback);
        void addRefreshCallback(std::function<void(void)> callback);
        void addMouseCallback(std::function<void(int, int, int)> callback);
    };

}
