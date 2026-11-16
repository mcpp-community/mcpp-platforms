import std;

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    try {
        throw std::runtime_error("caught");
    } catch (const std::exception& e) {
        std::println("platforms ok: {} {}", e.what(), args);
    }
    return 7;
}
