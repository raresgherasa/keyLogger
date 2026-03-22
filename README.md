## Keylogger

> [!CAUTION]
> **Ethical & Legal Disclaimer**  
> This project is designed strictly for educational purposes, personal monitoring, or catching yourself sleep-typing embarrassing emails to your boss. **Do not use this to steal your roommate's World of Warcraft password.** You are solely responsible for any misuse of this software. Stay lawful, stay cool.

## Features
- **Ultra-Sensitive Logging**: 200Hz polling rate (5ms loops) so you never miss a beat.
- **Auto-Repeat Logic**: Fully handles holding down keys (like `W` for gaming or `A` for screaming internally).
- **Stealth Mode (`--stealth`)**: Run it as a background daemon on Linux without leaving a trace in your terminal.
- **Custom Outputs**: Choose where to dump your logs. Default is `dat.txt`.
- **Graceful Shutdown**: Intercepts `Ctrl+C` to ensure all file buffers are safely flushed to disk before exiting. No more corrupted logs!

## 🛠️ Building the Project

### Prerequisites
You'll need a C++17 compiler, CMake, and SFML (System, Window modules). 
On Debian/Ubuntu:
```bash
sudo apt update
sudo apt install libsfml-dev cmake g++
```

### Build Steps
```bash
git clone https://github.com/raresgherasa/keyLogger.git
cd keyLogger
cmake -B build
cmake --build build
```

## 🚀 Usage

Run it normally in the console:
```bash
./build/keyLogger
```

Run it and save the log to a super-secret file:
```bash
./build/keyLogger --output top_secret_logs.txt
```

Run it in **Stealth Mode** (it detaches from your terminal and runs quietly in the background):
```bash
./build/keyLogger --stealth
```

To stop Stealth Mode, you'll need to find the process ID (PID) and politely ask it to leave:
```bash
pkill keyLogger
```
