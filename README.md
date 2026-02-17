# Wordle-Cpp

![Screenshot](example.png)

## Overview

**Wordly-C++** is a desktop clone of the popular `Wordle` game, built using **C++20** and the **raylib** library. The game challenges you to guess a hidden 5-letter word in 6 attempts with real-time color feedback.

## Features

* **Dynamic Dictionary:** Loads almost 6000 popular 5-letter English words.
* **Anti-Cheat System:** Only valid dictionary words are accepted as guesses.
* **Customizable UI:** Change colors and themes via an external configuration file.
* **Cross-platform:** Easy to build using CMake.
* **Autoplay Mode:** Intelligent AI solver to assist or play for you.
* **User Statistics:** Tracks `current streak`, `total wins`, `win distribution`, `total lost games`, and `best streak`.

---

## 🤖 Autoplay Algorithm

The game features an intelligent solver that attempts to crack the code using a **Heuristic Filtering Strategy**:

1.  **Initial Seed:** The bot starts with a randomly selected valid 5-letter word to ensure variety in every session.
2.  **Constraint Analysis:** After each guess, the bot analyzes the feedback:
    * 🟩 **Fixed Constraints:** Locks in letters identified in the correct positions.
    * 🟨 **Existential Constraints:** Retains letters that must be present but at different indices.
    * ⬜ **Exclusionary Constraints:** Filters out any words containing "gray" letters.
3.  **Dictionary Pruning:** The bot performs a real-time sweep of the dictionary, removing all words that violate the current constraints and picking the next candidate from the remaining pool.



---

## Getting Started

### Prerequisites
* A C++20 compatible compiler (GCC, Clang, MSVC).
* CMake 3.10 or higher.
* Git.

### Installation & Build
```bash
# Clone the repository
git clone [https://github.com/quaslir/Wordly-C-](https://github.com/quaslir/Wordly-C-)
cd Wordly-C-

# Clone json parsing library
git clone https://gitlab.com/main845804/json-parser-c

# Initialize raylib submodule
git submodule update --init --recursive

# Build the project
mkdir build && cd build
cmake ..
make

# Run the game
./Wordle-Cpp

```

**⚠️ Important** In the root directory, there are two important files: `history.json` and `sgb-words.txt`. 

* The first will be created automatically and stores history data; deleting this file will cause all your history to be reset to zero. 
* The second file is the main dictionary; if you delete this file or move it to another directory, you will need to pass the name of the dictionary via the command line:

```bash
./Wordle-Cpp "../PATH_TO_DICTIONARY"
```
In case a path is not provided, the game will not launch.
---
## Configuration

You can customize the visual experience by editing the `config.conf` file. The game looks for this file in the src directory.

| Key | Value Example | Description |
| :--- | :--- | :--- |
| **BG_COLOR** | `BLACK`, `RAYWHITE` | Background color |
| **GRID_COLOR** | `GREEN`, `GRAY` | Color of the letter cells |
| **TEXT_COLOR** | `BLUE`, `WHITE` | Main UI text color |
| **HARD_MODE** | `TRUE`, `FALSE` | Harder playing mode (you must use a particular letter if it has previously been revealed)|
| **AUTOPLAY** | `TRUE`, `FALSE` | A simple bot will attempt to guess the hidden word |

**Example `config.conf`:**
```conf
BG_COLOR=BLACK
GRID_COLOR=GREEN
TEXT_COLOR=BLUE
HARD_MODE=FALSE
AUTOPLAY=FALSE
```
## Controls

| Input | Action |
| :--- | :--- |
| **[A-Z]** | Type letters |
| **[Backspace]** | Remove last letter |
| **[Enter]** | Submit word for validation |
| **[Mouse]** | Navigate "Play Again" or "Exit" buttons |

## How It Works

Wordly-C++ uses a strict validation logic. To prevent **"vowel-spamming"** (e.g., typing `AEIOU`), every entry is checked against the internal dictionary. If a word doesn't exist in the database, it won't be accepted.

### Feedback System


* 🟩 **Green**: Letter is in the word and in the correct spot.
* 🟨 **Yellow**: Letter is in the word but in the wrong spot.
* ⬜ **Gray**: Letter is not in the word at all.

---
*Created by **Karl** as a Computer Science practice project.*