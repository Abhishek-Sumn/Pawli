# Pawli - Desktop Pet 🐾

Pawli is a cute, animated desktop companion that lives on your screen. Written in C++ using native Windows APIs and Direct2D, it's lightweight, efficient, and adorable.


## ✨ Features

- **Animated & Alive**: Walks, sleeps, jumps, and reacts to your clicks.
- **Multi-Monitor Roaming**: Can walk across all your connected displays.
- **Physics**: Pick her up and drag her around; she has gravity!
- **Zero-BG Interface**: Uses layered windows for a seamless, transparent look.
- **Smart Sleep**: Goes to sleep when you're idle.
- **Efficient**: Hand-crafted C++ engine (~1% CPU usage).

## 🎮 Controls

| Action | Control |
| :--- | :--- |
| **Interact / React** | Left Click |
| **Move / Drag** | Right Click & Drag |
| **Exit** | Right Click x 5 times |

## 📥 Installation

1. Download the latest release.
2. Run `Install-Pawli.bat`.
3. Follow the prompts (Y/N).
4. Pawli will launch and also start automatically with Windows (optional).

## 🛠️ Building from Source

**Requirements:**
- Visual Studio 2022 (with "Desktop development with C++" workload)
- Windows 10/11 SDK

**Steps:**
1. Clone the repository
   ```bash
   git clone https://github.com/yourusername/Pawli.git
   ```
2. Open `Developer Command Prompt for VS 2022`.
3. Navigate to the project directory.
4. Run the build script:
   ```cmd
   build.bat
   ```
5. Run the executable:
   ```cmd
   bin\Pawli.exe
   ```

## 📂 Project Structure

- `Core/`: Window management, Rendering (Direct2D), App loop.
- `Pet/`: Logic for the pet (State machine, Physics, AI).
- `Actions/`: Individual behaviors (Walk, Sleep, Jump, React).
- `Assets/`: Sprite loading and atlas management.
- `Desktop/`: Multi-monitor detection and virtual workspace handling.

## 🤝 Contributing

Contributions are welcome! Feel free to open issues or submit PRs for new animations, behaviors, or optimizations.

## 📄 License

MIT License. Free to use and modify.
