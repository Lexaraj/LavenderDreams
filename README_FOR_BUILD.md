# Lavender Dreams - Windows Build Instructions

This guide will help you build the Lavender Dreams server on Windows using Visual Studio 2022 and CMake 4.0.0.

## Setup Instructions

### Step 1: Clone the Repository

First, clone the Lavender Dreams repository:

```powershell
cd C:\
git clone https://github.com/Lexaraj/LavenderDreams.git LavenderDreams
cd LavenderDreams
```

### Step 2: Download External Libraries

- Get ACE Wrappers from https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-8_0_5/ACE+TAO-8.0.5.zip

- Get TBB from https://github.com/uxlfoundation/oneTBB/releases/download/v2022.3.0/oneapi-tbb-2022.3.0-win.zip

- Extract the libraries into your root folder under `ACE_wrappers` and `tbb` respectively.

After extraction, your folder structure should look like:
```
C:\LavenderDreams\
├── src\                    (from git)
├── dep\                    (from git)
├── cmake\                  (from git)
├── sql\                    (from git)
├── contrib\                (from git)
├── CMakeLists.txt          (from git)
├── ACE_wrappers\           (from zip)
│   ├── ace\
│   └── lib\
├── tbb\                    (from zip)
│   ├── include\
│   └── lib\
```

### Step 3: Set Environment Variables

You need to tell CMake where to find ACE and TBB.

**Option A: Using PowerShell (Recommended - Permanent)**

Open PowerShell as Administrator and run:

```powershell
# Replace C:\LavenderDreams with your actual extraction path
setx ACE_ROOT "C:\LavenderDreams\ACE_wrappers"
setx TBB_ROOT "C:\LavenderDreams\tbb"
```

**Option B: Using System Properties GUI (Permanent)**

1. Press `Windows Key + Pause/Break` (or right-click "This PC" → Properties)
2. Click "Advanced system settings" in the right panel
3. Click "Environment Variables"
4. Under "System variables", click "New"
5. Add these two variables:
   - Variable name: `ACE_ROOT`
     Variable value: `C:\LavenderDreams\ACE_wrappers` (use your actual path)
   - Variable name: `TBB_ROOT`
     Variable value: `C:\LavenderDreams\tbb` (use your actual path)
6. Click OK on all dialogs

**Option C: Temporary (Just for This Session)**

In PowerShell:
```powershell
$env:ACE_ROOT="C:\LavenderDreams\ACE_wrappers"
$env:TBB_ROOT="C:\LavenderDreams\tbb"
```


### Step 4: Verify Your Setup

Open a **new** PowerShell window and run:

```powershell
cmake --version
git --version
echo $env:ACE_ROOT
echo $env:TBB_ROOT
```

You should see:
- CMake version 3.16 or higher
- Git version information
- The path to your ACE_wrappers folder
- The path to your tbb folder

If any of these are empty or show "not found", go back to the appropriate step.

---

## Building the Project

### Method 1: Using Visual Studio

#### Step 1: Open the Project

1. Open Visual Studio 2022
2. Click "Open a local folder"
3. Navigate to and select your `C:\LavenderDreams` folder

Visual Studio will automatically detect the CMake project.

#### Step 2: Configure CMake

1. Wait for Visual Studio to finish loading (you'll see "CMake generation finished" in the output)
2. If you see any errors about ACE or TBB not found:
   - Make sure environment variables are set correctly
   - Close and reopen Visual Studio
   - Or use Method 2 below to specify paths manually

#### Step 3: Build

1. At the top of Visual Studio, select **"x64-Release"** from the configuration dropdown
2. Go to menu: **Build → Build All** (or press `Ctrl+Shift+B`)
3. Wait for the build to complete (this may take 10-20 minutes on first build)

#### Step 4: Find Your Binaries

After successful build, your executables will be in:
```
C:\LavenderDreams\out\build\x64-Release\
```

Look for:
- `mangosd.exe` - Game server
- `realmd.exe` - Login/realm server

### Method 2: Using Command Line (More Control)

#### Step 1: Open PowerShell

Right-click on your project folder and select "Open in Windows Terminal" or "Open PowerShell window here"

Or open PowerShell and navigate:
```powershell
cd C:\LavenderDreams
```

#### Step 2: Create Build Directory

```powershell
mkdir build
cd build
```

#### Step 3: Configure with CMake

**Basic Configuration:**
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64
```

**If ACE_ROOT or TBB_ROOT aren't detected, specify them explicitly:**
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64 `
  -DACE_ROOT="C:\LavenderDreams\ACE_wrappers" `
  -DTBB_ROOT="C:\LavenderDreams\tbb"
```

**Optional: Specify custom install location:**
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_INSTALL_PREFIX="C:\LavenderDreams\server"
```

You should see output similar to:
```
-- Found ACE library: C:/LavenderDreams/ACE_wrappers/lib/ACE.lib
-- Found ACE headers: C:/LavenderDreams/ACE_wrappers
-- Memory allocation: TBB/Release
-- Configuring done
-- Generating done
```

#### Step 4: Build the Project

**Build Release version (recommended):**
```powershell
cmake --build . --config Release
```

**Or build with Visual Studio:**
```powershell
# This opens the solution in Visual Studio
start MaNGOS.sln
```

Then in Visual Studio:
1. Select **Release** configuration (dropdown at top)
2. Select **x64** platform
3. Build → Build Solution (F7)

#### Step 5: Install (Optional)

This copies the executables and DLLs to your install directory:
```powershell
cmake --install . --config Release
```

#### Step 6: Find Your Binaries

After build, executables will be in one of these locations:

**If you built without install:**
```
C:\LavenderDreams\build\src\mangosd\Release\mangosd.exe
C:\LavenderDreams\build\src\realmd\Release\realmd.exe
```

**If you ran install:**
```
C:\LavenderDreams\server\mangosd.exe
C:\LavenderDreams\server\realmd.exe
```


