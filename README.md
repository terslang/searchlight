# Searchlight

Searchlight is a work-in-progress search engine. This repository contains the source code for the entire project.

## Components

Currently, the main component of Searchlight is the web crawler. Other components, such as the indexer and query processor, will be added in the future.

- [Crawler](./crawler/README.md)

## Building the Project

To build the project, you need to have the following dependencies installed:

- CMake (version 3.25 or higher)
- A C++ compiler that supports C++20
- cURL
- [ada-url/ada](https://github.com/ada-url/ada)

Once you have the dependencies installed, you can build the project using the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

## License

This project is licensed under the AGPL-3.0-only license. See the [LICENSE](./LICENSE) file for more details.
