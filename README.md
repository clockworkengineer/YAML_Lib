# YAML_Lib

YAML_Lib is a lightweight, high-performance C++ library designed to parse and manipulate YAML files. It enables seamless conversion between YAML text and an internal YAML object representation that can be interrogated, modified, and saved back to text.

## Features

- **Parsing**: Read and parse YAML strings or files into an internal YAML object.
- **Exception Handling**: Automatically identifies syntactical errors during parsing and throws exceptions for invalid YAML.
- **Stringification**: Convert the internal YAML object back to a YAML-formatted string.
- **Pretty Printing**: Generate human-readable, well-formatted YAML outputs using `print` and formatting methods like `setIndent`.
- **Modification**: Traverse and manipulate the YAML object structure programmatically using provided APIs.
- **Extensible Interfaces**:
  - **Custom Parsing**: Use the `ISource` interface for parsing YAML from your custom input sources.
  - **Custom Output**: Write YAML data to custom destinations using the `IDestination` interface.
- **Action Methods**: Traverse the YAML structure and apply custom operations using the `IAction` interface.

## Getting Started

### Dependencies

This library requires:
- C++20 or newer
- A compatible compiler (e.g., GCC, Clang, MSVC)

### Installation

To include **YAML_Lib** in your project, clone the repository and integrate it into your build system:

```bash
git clone https://github.com/your-repo/YAML_Lib.git
```

Make sure to link the library in your CMake or build script.

#### Example: Using CMake
```cmake
add_subdirectory(YAML_Lib)
target_link_libraries(your_project_name PRIVATE YAML_Lib)
```

### Usage

1. **Parse YAML**
    ```cpp
    #include "YAML_Lib.h"

    YAML::Node root = YAML::Parse("example.yaml");
    ```

2. **Manipulate Data**
   Use the YAML object to access or modify data programmatically:
    ```cpp
    auto value = root["key"].as<std::string>();
    root["new_key"] = 42;
    ```

3. **Stringify and Save**
   Convert the modified YAML object into a string or save it to a file:
    ```cpp
    std::string output = root.Stringify();
    root.SaveToFile("output.yaml");
    ```

### Examples

Several usage examples are provided in the `examples` folder. These examples demonstrate:
- Parsing YAML files
- Modifying the YAML object tree
- Saving YAML back to file
- Implementing custom input/output sources with `ISource` and `IDestination`

## Documentation

For detailed documentation and the complete YAML specification, visit the [official YAML website](https://yaml.org/spec/1.2.2/).

## Contributing

1. Fork the repository.
2. Create a branch: `git checkout -b feature-name`
3. Commit your changes: `git commit -m "Added a new feature"`
4. Push the changes: `git push origin feature-name`
5. Submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

Special thanks to the developers of the YAML specification and contributors to the C++ ecosystem.

---
Happy coding!
```
Feel free to adjust paths, repository links, or additional project-specific information as necessary.
