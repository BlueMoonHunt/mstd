# Changelog & Versioning Policy

## 2026-07-18 - Starting serious versioning for the library.

### 2.0.0 - complete refactor/standardization for str and file api. bug fix in darray (was not updating current size of available chunks)
### 1.0.4 - refactored base type macros and added Type struct
### 1.0.3 - added trace module
### 1.0.2 - removed of optional parameter in function feature (required c23 extension), refactored str8_slice functions and os_cmd
### 1.0.1 - fixed arena temp defination named incorrectly.
### 1.0.0 - mstd as of now is c11 lib somewhat compatible with c89 using some extensions for VA, initializers, ULL etc.

---

### A Note on Versioning
Development tracks API stability going forward, with public interfaces documented under strict backward-compatibility rules.
version [MAJOR, MINOR, PATCH]

### Change Condition
> MAJOR - Breaking changes require us to update major version of api.
> MINOR - Adding Major/Big modules in the library.
> PATCH - Adding small modules, refactors and bug fixes.