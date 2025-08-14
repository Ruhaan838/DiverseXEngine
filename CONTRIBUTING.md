# Contributing to DiverseXEngine

Thank you for your interest in contributing to **DiverseXEngine**!  
This guide will help you set up the development environment, especially with **Qt** and **CLion**.

---

## 1. Prerequisites

Before you start, ensure you have the following installed:

- **Qt 6.x** (with MinGW compiler support)
- **CMake** (comes with CLion)
- **CLion** IDE
- **MinGW** toolchain (if not included with CLion)

---

## 2. Installing Qt

1. Download and install the **Qt Online Installer** from:  
   [https://www.qt.io/download](https://www.qt.io/download)

2. During installation:
   - Select the **Qt 6.x.x** version you want to use.
   - Choose **MinGW** matching your compiler (e.g., `mingw_64`).
   - Ensure you also install **Qt Creator** (optional) and **Developer Tools**.

3. Verify the Qt installation path.  
   It should look like:

   ```
   C:/Qt/6.9.1/mingw_64
   ```

4. Inside the `mingw_64` folder, you should have:
   - `bin/` (contains `Qt6Core.dll`, `Qt6Widgets.dll`, etc.)
   - `lib/cmake/Qt6/` (contains `Qt6Config.cmake`)

---

## 3. Configuring CLion

### Step 1: Set Qt Path in CMake

In CLion:

1. Open **File > Settings > Build, Execution, Deployment > CMake**
2. In **CMake options**, add:

   ```cmake
   -DCMAKE_PREFIX_PATH="C:/Qt/6.9.1/mingw_64"
   ```

   *(Change the path if your Qt version/folder name differs)*

---

### Step 2: Add Qt to PATH for Runtime

To avoid the **`0xC0000135` Missing DLL** error:

1. Go to **Run > Edit Configurations**
2. Select your project target (`DiverseXEngine`)
3. In **Environment Variables**, add:

   ```
   PATH=C:\Qt\6.9.1\mingw_64\bin;%PATH%
   ```

4. Click **Apply** and **OK**.

---

## 4. Building the Project

1. Click the **Reload CMake Project** button in CLion.
2. Build with the green **Run** ▶️ button.
