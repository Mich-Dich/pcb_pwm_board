#!/bin/bash
# Setup script for STM32CubeProgrammer CLI from existing CubeIDE installation

set -e  # Exit on any error

# Try to find CubeIDE installation directory
CUBEIDE_HOME=""
if [ -d "$HOME/STM32CubeIDE_2.0.0" ]; then
    CUBEIDE_HOME="$HOME/STM32CubeIDE_2.0.0"
elif [ -d "/opt/STM32CubeIDE_2.0.0" ]; then
    CUBEIDE_HOME="/opt/STM32CubeIDE_2.0.0"
else
    echo "STM32CubeIDE 2.0 not found in default locations."
    read -p "Please enter the full path to your STM32CubeIDE installation: " CUBEIDE_HOME
    if [ ! -d "$CUBEIDE_HOME" ]; then
        echo "Directory does not exist. Exiting."
        exit 1
    fi
fi

echo "Using CubeIDE at: $CUBEIDE_HOME"

# Find the CubeProgrammer plugin directory
CUBEPROG_PLUGIN=$(find "$CUBEIDE_HOME/plugins" -maxdepth 1 -type d -name "com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.*" | head -n 1)

if [ -z "$CUBEPROG_PLUGIN" ]; then
    echo "STM32CubeProgrammer plugin not found in $CUBEIDE_HOME/plugins"
    exit 1
fi

echo "Found plugin at: $CUBEPROG_PLUGIN"

# The tools are inside the plugin's "tools" directory
CUBEPROG_TOOLS="$CUBEPROG_PLUGIN/tools"
if [ ! -d "$CUBEPROG_TOOLS" ]; then
    echo "Tools directory not found: $CUBEPROG_TOOLS"
    exit 1
fi

CUBEPROG_BIN="$CUBEPROG_TOOLS/bin"
if [ ! -f "$CUBEPROG_BIN/STM32_Programmer_CLI" ]; then
    echo "STM32_Programmer_CLI not found in $CUBEPROG_BIN"
    exit 1
fi

echo "CLI tool found at: $CUBEPROG_BIN/STM32_Programmer_CLI"

# Add to PATH in ~/.bashrc if not already present
if ! grep -q "export PATH=\$PATH:$CUBEPROG_BIN" ~/.bashrc; then
    echo "export PATH=\$PATH:$CUBEPROG_BIN" >> ~/.bashrc
    echo "Added $CUBEPROG_BIN to PATH in ~/.bashrc"
else
    echo "PATH entry already exists."
fi

# Copy udev rules
UDEV_RULES_SRC="$CUBEPROG_TOOLS/Drivers/rules"
if [ -d "$UDEV_RULES_SRC" ]; then
    echo "Copying udev rules from $UDEV_RULES_SRC"
    sudo cp "$UDEV_RULES_SRC"/*.rules /etc/udev/rules.d/
    sudo udevadm control --reload-rules
    sudo udevadm trigger
else
    echo "Udev rules directory not found. You may need to configure them manually."
fi

# Add user to required groups
for group in dialout plugdev; do
    if groups "$USER" | grep -q "\b$group\b"; then
        echo "User already in group $group."
    else
        echo "Adding user $USER to group $group..."
        sudo usermod -a -G "$group" "$USER"
    fi
done

echo "Setup complete. You may need to log out and back in for group changes to take effect."
echo "After that, you can run 'STM32_Programmer_CLI --version' to verify."
