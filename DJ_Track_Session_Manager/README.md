# DJ Session Management System - README

## Project Overview
This is a C++ project that implements a DJ Session Management system with support for audio track management, playlists, caching, and mixing operations.

## Project Structure
```
Skeleton/
├── bin/                    # Compiled executables and configuration files
│   └── dj_config.txt      # Configuration file for DJ settings
├── include/               # Header files (.h)
├── src/                   # Source files (.cpp)
├── .devcontainer/         # Development container configuration
├── Makefile              # Build system configuration
└── README.md             # This file
```

## Prerequisites

### Option 1: Using Dev Container (Recommended)
This project includes a `.devcontainer` configuration that provides a complete development environment with all necessary tools pre-installed. If you're using Visual Studio Code with the Dev Containers extension, simply open the project and it will set everything up for you.

### Option 2: University Lab Computers
All required tools are pre-installed on the lab computers at BGU. You can use those directly without any setup.

### Option 3: Local Installation
If working on your own machine, you'll need:
- A C++ compiler (g++ recommended)
- Make build tool
- Linux/Unix environment (or WSL on Windows)
- Optional: valgrind (for memory leak detection)
- Optional: gdb (for debugging)

You can install these on Ubuntu/Debian with:
```bash
make install-deps
```

## Getting Started

### 1. Getting the Project Files

This project is hosted in a **Git repository**. Think of a Git repository (or "repo") as a shared folder in the cloud that contains all the project files and tracks their history.

#### What is Git?
Git is a version control system - like a powerful "undo" system for code. It lets you:
- Download the project files (called "cloning")
- Get updates if the instructors fix bugs or add clarifications (called "pulling")
- Track what you've changed

#### Getting the Code (Cloning)
To get a copy of the project on your computer, you need to **clone** the repository. You'll receive a repository URL from your instructor (it looks like `https://github.com/...`).

**Using VS Code**:
1. Press `Ctrl+Shift+P` to open the Command Palette
2. Type "Git: Clone" and select it
3. Paste the repository URL provided by your instructor
4. Choose where to save the project on your computer
5. Click "Open" when prompted

You only need to clone once! After that, you have all the files locally.

### 2. Understanding the Build System
This project uses **Make**, a build automation tool that compiles your code. The `Makefile` contains instructions for how to build the project.

Think of Make as a recipe book for building your program. Instead of manually compiling each file, Make reads the `Makefile` and knows exactly which files to compile and in what order.

### 3. Building the Project

To compile the entire project, open a terminal in the `Skeleton` directory and run:
```bash
make
```

This command will:
- Create the `bin/` directory if it doesn't exist
- Compile all `.cpp` files from the `src/` directory
- Link them together
- Create an executable called `dj_manager` in the `bin/` directory

For a debug build (useful when developing):
```bash
make debug
```

For an optimized release build:
```bash
make release
```

### 4. Cleaning Build Files

To remove all compiled files and start fresh:
```bash
make clean
```

### 5. Running the Program

After building, the program requires both the `-I` (interactive) and `-A` (all playlists) flags:

**Running All Playlists**:
```bash
./bin/dj_manager -I -A
```
This runs the system in automatic mode, processing all available playlists sequentially.

Or use the convenient test target:
```bash
make test
```

**Note**: The `-I` flag enables interactive mode, while the `-A` flag processes all playlists automatically. Both flags are required for proper operation.

### 6. Checking for Memory Leaks

To run the program with valgrind memory leak detection:
```bash
make test-leaks
```

## Main Components

- **AudioTrack**: Base class for audio files
- **MP3Track/WAVTrack**: Specific audio format implementations
- **Playlist**: Manages collections of tracks
- **LRUCache**: Implements Least Recently Used caching strategy
- **CacheSlot**: Individual cache entry management
- **DJSession**: Main session management
- **DJControllerService**: Handles DJ control operations
- **DJLibraryService**: Manages music library
- **MixingEngineService**: Handles audio mixing operations
- **ConfigurationManager**: Manages application settings
- **SessionFileParser**: Parses session configuration files

## Configuration

Edit `bin/dj_config.txt` to modify DJ session settings before running the program.

## Common Make Commands

- `make` or `make all` - Build the entire project
- `make debug` - Build with debug information for development
- `make release` - Build optimized version for production
- `make clean` - Remove all compiled files
- `make test` - Build and run the program
- `make test-leaks` - Run with valgrind to check for memory leaks
- `make install-deps` - Install required development tools (Ubuntu/Debian)
- `make help` - Display all available commands with descriptions

## Student Workflow

The recommended workflow for completing this assignment:

1. **Build with debug info**: `make debug`
2. **Run the program**: `make test`
3. **Find and fix TODOs** in the code
4. **Check for memory leaks**: `make test-leaks`
5. **Repeat** steps 3-4 until all issues are resolved!

## Troubleshooting

**Build Errors**: If you get compilation errors:
1. Make sure all required files are present in `src/` and `include/`
2. Check that your compiler is properly installed: `g++ --version`
3. Try running `make clean` first, then `make`
4. Read the error messages carefully - they usually point to the problem

**Permission Errors**: If you can't execute the program:
```bash
chmod +x ./bin/dj_manager
```

**"Command not found" errors**: 
- If `make` is not found, you need to install it (or use the dev container/lab computers)
- If `valgrind` is not found for memory testing, run `make install-deps`

## Development Tips

1. After modifying any `.cpp` or `.h` file, run `make` to rebuild
2. The build system automatically detects which files changed and only recompiles those
3. Always test after making changes by rebuilding and running the program
4. Use `make debug` during development for better error messages
5. Run `make test-leaks` frequently to catch memory issues early

## Getting Updates from Instructors

During the assignment's period, your instructors may push updates, bug fixes, or clarifications to the assignment repository. While we hope there won't be any updates needed, it's good to know how to get them just in case.

### Checking for and Getting Updates

#### Using VS Code Interface:
1. Open the Source Control panel (click the branch icon in the left sidebar or press `Ctrl+Shift+G`)
2. Click the "..." menu (three dots) at the top
3. Select **"Fetch"** to check if updates are available (this doesn't change your files yet)
4. If updates are available, select **"Pull"** to download and apply them

#### Using Terminal:
Open the terminal (`` Ctrl+` ``) and run:
```bash
# Check if there are any updates
git fetch

# If updates exist, download and apply them
git pull
```

### When to Check for Updates
- At the start of each work session (just to be safe)
- If your instructor announces an update via email or the course website
- If you encounter unexpected errors that classmates don't have

### What if There Are Conflicts?
If you've modified files and there are updates, Git will usually merge them automatically. However, if there's a conflict (you and the instructor changed the same lines), Git will ask for help. In this case:
1. Don't panic - this is rare
2. Contact your course instructor or TA
3. They'll help you resolve the conflict

**Note**: You don't need to commit, push, or create branches for this assignment. Your main interaction with Git is just cloning once and occasionally pulling updates. Focus on writing your C++ code!

---

For questions or issues, please contact the TA in charge on the assignment.