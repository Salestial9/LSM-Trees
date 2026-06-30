# LSM Tree Visualizer using WebAssembly

An interactive **Log-Structured Merge Tree (LSM Tree)** simulator built using **C++**, **WebAssembly (WASM)**, **JavaScript**, and **HTML**. The project demonstrates how modern storage engines manage writes efficiently using Memtables, SSTables, Bloom Filters, and Compaction while executing the core storage logic inside WebAssembly.

## Features

* Interactive visualization of an LSM Tree
* Core storage engine implemented in C++
* Compiled to WebAssembly using Emscripten
* Insert and retrieve key-value pairs
* Automatic Memtable flushing after threshold is reached
* SSTable creation and management
* Bloom Filter implementation for efficient key lookups
* Manual compaction of SSTables
* Browser-based UI with real-time visualization

## Project Structure

```
lsm_project/
│── lsm_wasm.cpp              # Core LSM Tree implementation
│── lsm.js                    # Generated JavaScript bindings
│── lsm.wasm                  # Compiled WebAssembly module
│── lsm2_wasm_connected.html  # Interactive frontend
│── server.py                 # Local HTTP server
│── build.sh                  # Build script
```

## Technologies Used

* C++
* WebAssembly (WASM)
* Emscripten
* HTML
* CSS
* JavaScript
* Python (Local HTTP Server)

## How It Works

1. New key-value pairs are inserted into the Memtable.
2. When the Memtable reaches its threshold, it is flushed to disk as an SSTable.
3. Each SSTable maintains a Bloom Filter for fast negative lookups.
4. Read operations first check the Memtable and then SSTables using Bloom Filters.
5. Multiple SSTables can be merged through compaction to reduce storage overhead and improve lookup performance.

## Build Instructions

### Prerequisites

* Emscripten SDK
* Python 3

### Compile the WebAssembly Module

```bash
chmod +x build.sh
./build.sh
```

### Run the Local Server

```bash
python server.py
```

Open the browser and navigate to:

```
http://localhost:8000/lsm2_wasm_connected.html
```


## Future Improvements

* Multiple storage levels (L0, L1, L2)
* Configurable Bloom Filter parameters
* Delete operations with Tombstones
* Range queries
* Performance benchmarking dashboard
* Persistent storage using IndexedDB

