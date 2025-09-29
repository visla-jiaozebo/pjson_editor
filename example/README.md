# Example Project Using Installed PJsonEditor

This example demonstrates how to use PJsonEditor after it has been installed.

## Prerequisites

1. First, install PJsonEditor using the main installation instructions
2. Ensure PJsonEditor is properly installed and findable by CMake

## Building the Example

```bash
cd example
mkdir build && cd build
cmake ..
cmake --build .
./example
```

## If CMake Can't Find PJsonEditor

If you installed PJsonEditor to a custom location, you might need to help CMake find it:

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/pjsoneditor/installation ..
```

For example, if you installed with `--prefix=$HOME/local`:

```bash
cmake -DCMAKE_PREFIX_PATH=$HOME/local ..
```

## Expected Output

When you run the example, you should see:
```
PJsonEditor Example
Successfully linked with PJsonEditor library!
```

This confirms that the library was properly found and linked.