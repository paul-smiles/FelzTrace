# Coding Style Guide

This document defines the coding style conventions for the FelzTrace project.

## Naming Conventions

### Types (Classes, Structs, Enums)
**Convention:** PascalCase

```cpp
class RequirementStore;
struct ConfigData;
enum class ReturnCode;
```

### Methods and Functions
**Convention:** camelCase

```cpp
void addRequirement(const std::string& id);
ReturnCode parseCli(int argc, const char* argv[]);
```

### Variables (Local, Member, Parameters)
**Convention:** camelCase

```cpp
int returnCode = 0;
std::string userId;
const char* argv[];
```

**Member Variables:** Prefix with `m_` to distinguish from local variables

```cpp
class MyClass
{
private:
    std::string m_name;
    int m_count;
};
```

### Files
**Convention:** snake_case

```
requirement_store.cpp
requirement_store.h
test_cli.cpp
```

### Namespaces
**Convention:** lowercase

```cpp
namespace felztrace
{
    // code
}
```

## Additional Guidelines

- Use `#pragma once` for header guards
- Prefer `enum class` over unscoped enums
- Mark interface methods as `virtual` and pure virtual with `= 0`
- Always provide virtual destructors for base classes
- Use meaningful, descriptive names
