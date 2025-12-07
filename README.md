# Compiler Design Laboratory Programs 🖥️

![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-blue)

This repository contains implementation of various compiler design concepts and algorithms as part of laboratory coursework.

## 📚 Table of Contents
- [Program List](#-program-list)
- [Features](#-features)
- [Prerequisites](#-prerequisites)
- [Installation](#-installation)
- [Usage](#-usage)
- [Project Structure](#-project-structure)
- [Contributing](#-contributing)
- [License](#-license)

## 🧾 Program List
- **Lexical Analyzer** (`token.c`)
- **Recursive Descent Parser** (`Rec_Descent_Parser.c`)
- **LR(1) Parser** (`LR1Parser.c`)
- **Shift-Reduce Parser** (`shift_reduce.c`)
- **Operator Precedence Parser** (`operator.c`)
- **Code Generation** (`codegen.c`)
- **Left Recursion Elimination** (`leftrecur.c`)
- **First & Follow Calculation** (`firstfol.c`)
- **Top-Down Parsing** (`topdown.c`)

## 🚀 Features
- Implementation of fundamental compiler phases
- Sample input/output files for testing
- Pre-compiled executables for quick verification
- Modular code structure for easy extension

## 📋 Prerequisites
- GCC compiler
- Basic understanding of compiler design concepts

## 💻 Installation
```bash
# Clone the repository
git clone https://github.com/PVSohan/COMPILER-DESIGN.git

# Compile any program (example)
gcc LR1Parser.c -o LR1Parser
```

## 🛠️ Usage
```bash
# Run a parser (example)
./LR1Parser input.txt
```

## 📂 Project Structure
```
COMPILER-DESIGN/
├── src/               # Source code files
│   ├── token.c        # Lexical analyzer
│   ├── LR1Parser.c    # LR(1) parser implementation
│   └── ...           # Other source files
├── input.txt          # Sample input
├── output.txt         # Sample output
└── executables/       # Pre-built binaries
```

## 🤝 Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.
