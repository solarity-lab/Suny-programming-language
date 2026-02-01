# 1 Suny Runtime

The Suny Runtime, known as CSuny, is the core of the Suny system. 
It executes Suny programs after the source code is translated into bytecode.
CSuny iterates through the bytecode instruction stream and executes each opcode sequentially.
The runtime focuses exclusively on opcode execution and does not handle source-level concerns.
CSuny is written in C to achieve high performance and precise control over memory, execution flow, and system resources.
This design enables Suny to communicate efficiently with low-level system layers.
A CSuny runtime instance executes one program at a time, sequentially.
Runtime instances in Suny do not share state.
Each runtime instance maintains its own execution context, including global variables and runtime-managed objects.

## 1.1 Responsibilities

The CSuny runtime is responsible for managing and controlling program execution at runtime.
Its primary responsibilities include:

- Executing bytecode instructions sequentially
- Managing the runtime execution state
- Resolving and accessing variables during execution
- Creating and managing runtime values and objects
- Handling function calls and returns
- Invoking native functions exposed by the host system
- Detecting and reporting runtime errors
The runtime operates strictly at the execution level and does not perform source-level analysis or validation.

## 1.2 Runtime Instance

A CSuny runtime instance represents an isolated execution environment for a Suny program.

Each runtime instance maintains its own execution context, including global variables, runtime-managed objects, and internal state required for program execution. Runtime instances do not share state with one another.

A runtime instance executes one program at a time, sequentially. While multiple runtime instances may exist within the same host process, each instance operates independently and does not execute programs concurrently.

Runtime instances are created explicitly by the host environment and remain active for the duration of program execution. Once execution completes, the runtime instance can be safely destroyed and all associated runtime state is released.

## 1.3 High-level Execution Flow

At a high level, the execution of a Suny program within CSuny follows a linear and deterministic flow.

```
Suny Source Code
        ↓
Bytecode Translation
        ↓
Runtime Initialization
        ↓
Opcode Execution
        ↓
Program Termination / Runtime Error
```

After the source code is translated into bytecode, a CSuny runtime instance is initialized.
The runtime then iterates through the bytecode instruction stream and executes each opcode sequentially.

Execution continues until the program reaches its end or a runtime error occurs, at which point the runtime terminates execution and releases all associated runtime state.

---

# 2 Runtime Lifecycle

## 2.1 Runtime Creation

A CSuny runtime is created explicitly by the host environment before program execution begins.

During runtime creation, a new runtime instance is allocated and prepared to serve as an isolated execution environment. At this stage, no program code is executed, and no bytecode is evaluated.

Each runtime creation results in a fresh and independent runtime instance, ensuring that execution state, global variables, and runtime-managed objects are not shared with any other runtime instance.

Runtime creation is a prerequisite for program execution and must occur before any bytecode can be executed.

## 2.2 Initialization

During initialization, the CSuny runtime prepares all internal structures required for program execution.

This phase establishes the execution environment but does not yet begin opcode execution.

Initialization typically includes:

- Setting up the runtime execution stack used for function calls and control flow
- Initializing the global execution context
- Preparing structures for local execution scopes
- Allocating and initializing the runtime-managed heap
- Loading the program bytecode into the runtime
- Initializing garbage collection or memory tracking mechanisms
- Preparing internal lookup structures such as label or instruction maps

Once initialization is complete, the runtime is ready to begin executing bytecode instructions.

## 2.3 Program Execution

After initialization is complete, the CSuny runtime begins program execution.

During this phase, the runtime processes the bytecode instruction stream one opcode at a time, executing instructions sequentially in a deterministic order defined by the program’s control flow.

Execution is driven by a central execution loop. In each iteration, the runtime fetches the current opcode, dispatches it to the appropriate handler, and updates the internal execution state, including the instruction pointer, runtime stack, and active execution contexts. Function calls, control flow transitions, and value manipulations are handled entirely within the runtime.

Opcode dispatch follows a structured control flow, typically implemented using a loop-based dispatch mechanism such as a switch-based opcode handler. Execution continues until the runtime reaches the end of the instruction stream or encounters a runtime error that causes execution to terminate.

## 2.4 Runtime Termination

Program execution in CSuny terminates when the runtime reaches the end of the bytecode instruction stream or encounters a condition that halts execution.
Termination can occur through several mechanisms:

- Normal termination: The runtime completes all bytecode instructions successfully and reaches the end of the program
- Explicit termination: An explicit exit or halt opcode instructs the runtime to stop execution
- Runtime error: A runtime error is detected during opcode execution, causing immediate termination
- Host interruption: The host environment forcibly terminates the runtime instance

Upon termination, the runtime enters a cleanup phase where all execution state is finalized. This includes:

- Finalizing the current execution context
- Releasing all runtime-managed objects and heap-allocated memory
- Clearing the execution stack
- Releasing bytecode instruction streams and associated metadata
- Invoking garbage collection or memory cleanup mechanisms to reclaim all runtime resources

Once termination and cleanup are complete, the runtime instance can be safely destroyed. The runtime guarantees that all resources associated with the terminated program are released, ensuring no memory leaks or dangling references remain.
After termination, the same runtime instance may be reinitialized to execute a new program, or it may be permanently destroyed by the host environment.

---

# 3 Garbage Collector

The CSuny runtime employs an automatic garbage collector to manage memory allocation and deallocation of runtime-managed objects. The garbage collector is responsible for tracking object lifetimes, identifying unreachable objects, and reclaiming memory without requiring explicit deallocation by the program.

The garbage collector operates as an integral component of the runtime and is invoked automatically during program execution. It ensures that memory used by objects no longer accessible to the program is safely reclaimed and returned to the runtime heap.

CSuny's garbage collector is designed to balance performance with memory efficiency, minimizing pause times while ensuring deterministic memory reclamation within the constraints of a single-threaded execution model.

## 3.1 Collection Algorithm

Unlike many modern programming languages that employ automatic garbage collection with tracing algorithms such as mark-and-sweep or generational collection, the Suny garbage collector is based on a manual reference counting model. The runtime does not automatically trace object reachability or perform periodic collection cycles. Instead, memory management is explicit and deterministic, with responsibility delegated to the program and the host environment.

This design choice prioritizes predictability and control over convenience. Programs and API consumers are expected to explicitly manage object lifetimes by tracking references and signaling when objects are no longer needed. While this approach requires more careful attention from developers, it provides fine-grained control over memory usage and eliminates the unpredictability of automatic garbage collection pauses.

### Reference Counting Model

Every object allocated in the Suny runtime maintains an internal reference count. This reference count is typically stored in the object's garbage collector metadata structure and accessed via `obj->gc->ref_count`. The reference count represents the number of active references to the object at any given point in time.

When an object is first created, its reference count is initialized to an appropriate value, typically 1, indicating that the creator holds a reference to the object. As references to the object are created, stored, or passed to other parts of the program, the reference count must be incremented. Conversely, when a reference is no longer needed, goes out of scope, or is overwritten, the reference count must be decremented.

When the reference count of an object reaches zero, the object is considered unreachable and eligible for collection. At this point, the object's memory can be safely reclaimed by the garbage collector, and all resources associated with the object are released.

### Memory Management Functions

Suny provides three core functions for explicit memory management and reference counting. These functions form the foundation of the manual memory management model and must be used correctly to prevent memory leaks, dangling pointers, or premature deallocation.

#### `_SUNYINREF`

The `_SUNYINREF` function increments the reference count of an object by one. This function must be called whenever a new reference to the object is created, stored, or retained. Common scenarios where `_SUNYINREF` should be used include:

- Storing an object reference in a data structure such as an array, list, or map
- Assigning an object reference to a variable that will outlive the current scope
- Passing an object reference to a function that retains ownership beyond the call
- Capturing an object reference in a closure or callback

Failing to call `_SUNYINREF` when creating a new reference can result in premature deallocation if the original reference is released while the new reference is still in use.

#### `_SUNYDEREF`

The `_SUNYDEREF` function decrements the reference count of an object by one. This function must be called whenever a reference to the object is no longer needed, goes out of scope, or is being replaced. Common scenarios where `_SUNYDEREF` should be used include:

- A local variable holding an object reference goes out of scope
- An object reference stored in a data structure is being removed or replaced
- A function that borrowed an object reference is returning and no longer needs the reference
- An object reference is being overwritten with a new value

Failing to call `_SUNYDEREF` when a reference is no longer needed can result in memory leaks, as the object's reference count will never reach zero and its memory will never be reclaimed.

#### `MOVETOGC`

The `MOVETOGC` function transfers ownership of an object to the garbage collector and attempts to reclaim the object's memory if its reference count has reached zero. This function is typically called after decrementing an object's reference count to finalize its lifecycle and trigger cleanup if appropriate.

If the object's reference count is greater than zero when `MOVETOGC` is called, the object remains alive and no memory is reclaimed. However, if the reference count has reached zero, the garbage collector reclaims the object's memory, invokes any necessary destructors or finalizers, and releases all associated resources.

`MOVETOGC` serves as the bridge between manual reference counting and the garbage collector's memory reclamation process. It is the primary mechanism by which unreachable objects are identified and deallocated.

### Automatic Reference Management in API Functions

While explicit reference counting is required when directly manipulating objects, most API functions provided by the Suny runtime manage reference counts automatically on behalf of the caller. This design reduces the burden on developers when using standard library functions and runtime-provided APIs.

For example, when passing an object to a Suny API function that stores or retains the object, the function will typically call `_SUNYINREF` internally to increment the reference count. Similarly, when a function releases an object or replaces a stored reference, it will call `_SUNYDEREF` to decrement the reference count.

As a result, developers using Suny API functions do not need to manually adjust reference counts for objects passed to or returned from these functions, as the runtime handles reference management transparently. However, when creating objects, storing them in custom data structures, or manipulating them outside the scope of standard APIs, explicit reference counting remains necessary.

### Responsibilities and Tradeoffs

The manual reference counting model places the responsibility for memory correctness on the programmer. Developers must carefully track object lifetimes, ensuring that reference counts are incremented and decremented correctly at all times. Incorrect reference counting can lead to several classes of errors:

- **Memory leaks**: Failing to decrement reference counts results in objects that are never deallocated, consuming memory indefinitely.
- **Use-after-free**: Decrementing reference counts prematurely can result in objects being deallocated while references to them are still in use, leading to undefined behavior.
- **Double-free**: Decrementing reference counts multiple times for the same reference can cause an object to be freed multiple times, corrupting memory.

Despite these challenges, the manual reference counting model offers significant advantages:

- **Deterministic deallocation**: Objects are reclaimed immediately when their reference count reaches zero, providing predictable memory usage patterns.
- **No garbage collection pauses**: The runtime does not perform periodic tracing or sweeping, eliminating unpredictable latency spikes.
- **Fine-grained control**: Developers have precise control over when objects are retained and released, enabling optimization of memory usage for performance-critical applications.

This tradeoff between control and safety is a deliberate design decision in Suny, reflecting the runtime's focus on predictability, performance, and low-level system integration.

# 4 Value and Type

## 4.1 Overview

The Suny runtime defines a set of fundamental data types that programs can use to represent and manipulate values. These types form the foundation of Suny's type system and provide the building blocks for all data structures and computations within the runtime.

Suny supports the following primitive and composite data types:

- **Boolean**: Represents logical true and false values
- **Number**: Represents numeric values, including both integers and floating-point numbers
- **String**: Represents sequences of characters and text data
- **List**: Represents ordered collections of values
- **Function**: Represents callable functions and closures
- **Class**: Represents user-defined types and class definitions
- **Userdata**: Represents opaque data managed by the host environment or native code

Each type has its own internal representation, memory layout, and set of operations. The runtime enforces type safety and provides mechanisms for type checking, type conversion, and type validation during program execution.

## 4.2 Type System Architecture

The Suny type system is designed around explicit type representation and runtime type checking. Every value in the runtime carries type information that identifies its type and enables the runtime to perform type-specific operations safely.

Values in Suny are typically represented as tagged unions or structures that contain:
- A type tag identifying the value's type
- The actual data payload appropriate for that type
- Metadata for garbage collection (reference count, GC flags)

This representation enables efficient type dispatch and ensures that operations are performed only on compatible types.

## 4.3 Boolean Type

### 4.3.1 Description

The Boolean type represents logical truth values. A Boolean value can be either `true` or `false`, corresponding to the logical states of truth and falsehood.

### 4.3.2 Representation

Booleans are typically represented as integer values, with `0` representing `false` and `1` representing `true`. The runtime may optimize Boolean storage by using a single bit or byte depending on platform and alignment requirements.

### 4.3.3 Operations

Common operations on Boolean values include:
- Logical AND, OR, NOT
- Equality and inequality comparison
- Type conversion to and from other types

### 4.3.4 Type Conversion

When converted to other types:
- To Number: `false` becomes `0`, `true` becomes `1`
- To String: `false` becomes `"false"`, `true` becomes `"true"`

## 4.4 Number Type

### 4.4.1 Description

The Number type represents numeric values. Suny supports two categories of numbers:
- **Integer**: Whole numbers without fractional components
- **Float**: Floating-point numbers with fractional components

### 4.4.2 Representation

Numbers are represented using standard C numeric types:
- Integers are typically represented as `int64_t` or `int32_t` depending on platform
- Floats are typically represented as `double` (64-bit IEEE 754 floating-point)

The runtime may distinguish between integer and float representations internally for performance optimization, but both are considered part of the unified Number type from the program's perspective.

### 4.4.3 Operations

Common operations on Number values include:
- Arithmetic operations: addition, subtraction, multiplication, division, modulo
- Comparison operations: less than, greater than, equality
- Bitwise operations (for integers): AND, OR, XOR, shift
- Mathematical functions: absolute value, power, square root

### 4.4.4 Integer and Float Distinction

While both integers and floats are considered Numbers, the runtime maintains internal distinctions:
- Integer operations produce integer results when possible
- Operations involving at least one float produce float results
- Division of two integers may produce a float if the result is not exact
- Explicit conversion functions allow programs to control integer/float conversion

### 4.4.5 Type Conversion

When converted to other types:
- To Boolean: `0` becomes `false`, all other values become `true`
- To String: Numbers are converted to their decimal string representation

## 4.5 String Type

### 4.5.1 Description

The String type represents sequences of characters and text data. Strings are immutable in Suny—once created, their contents cannot be modified. Operations that appear to modify strings actually create new string objects.

### 4.5.2 Representation

Strings are represented as sequences of bytes, typically encoded in UTF-8. The runtime stores strings with the following structure:
- Length field indicating the number of bytes
- Pointer to character data buffer
- Null terminator for C compatibility (if applicable)
- Hash value for efficient string comparison (optional)

### 4.5.3 String Interning

The runtime may employ string interning to optimize memory usage and comparison performance. Interned strings with identical contents share the same underlying memory, enabling constant-time equality comparison through pointer comparison.

### 4.5.4 Operations

Common operations on String values include:
- Concatenation: combining two or more strings
- Substring extraction: obtaining a portion of a string
- Length query: determining the number of characters
- Character access: retrieving individual characters by index
- Comparison: lexicographic ordering and equality testing
- Search: finding substrings or characters within a string

### 4.5.5 Type Conversion

When converted to other types:
- To Boolean: Empty string `""` becomes `false`, all other strings become `true`
- To Number: The runtime attempts to parse the string as a numeric literal; invalid formats may produce `0` or an error

## 4.6 List Type

### 4.6.1 Description

The List type represents ordered, mutable collections of values. Lists can contain values of any type, including mixed types within the same list. Lists support dynamic resizing and provide efficient indexed access.

### 4.6.2 Representation

Lists are typically represented as dynamic arrays with the following structure:
- Capacity field indicating allocated storage size
- Count field indicating current number of elements
- Pointer to data buffer containing element values

When a list's capacity is exceeded, the runtime reallocates a larger buffer and copies existing elements to maintain contiguous storage.

### 4.6.3 Operations

Common operations on List values include:
- Indexed access: retrieving or modifying elements by position
- Append: adding elements to the end of the list
- Insert: adding elements at specific positions
- Remove: deleting elements from the list
- Length query: determining the number of elements
- Iteration: traversing all elements in order
- Slicing: extracting sublists

### 4.6.4 Memory Management

List elements may be objects that require reference counting. When elements are added to a list, their reference counts are incremented. When elements are removed or the list is destroyed, reference counts are decremented appropriately.

### 4.6.5 Type Conversion

When converted to other types:
- To Boolean: Empty list `[]` becomes `false`, all other lists become `true`
- To String: The runtime may produce a string representation such as `"[1, 2, 3]"`

## 4.7 Function Type

### 4.7.1 Description

The Function type represents callable functions, including both user-defined functions written in Suny and native functions provided by the host environment. Functions are first-class values in Suny and can be passed as arguments, returned from other functions, and stored in data structures.

### 4.7.2 Representation

Functions are represented as objects containing:
- Pointer to bytecode or native code
- Parameter count and signature information
- Closure environment (captured variables from enclosing scopes)
- Function metadata (name, source location)

### 4.7.3 Closures

When a function references variables from enclosing scopes, it captures those variables in a closure. The closure maintains references to the captured variables, ensuring they remain accessible even after the enclosing scope has exited.

Captured variables are reference-counted to ensure correct memory management when closures outlive their defining scopes.

### 4.7.4 Operations

Common operations on Function values include:
- Invocation: calling the function with arguments
- Partial application: creating new functions with some arguments pre-filled
- Comparison: testing function equality (typically pointer equality)

### 4.7.5 Native Functions

Native functions are implemented in C or other host languages and exposed to Suny programs through the API. Native functions follow calling conventions that enable them to receive Suny values as arguments and return Suny values as results.

The runtime manages the transition between Suny bytecode execution and native code execution, handling parameter marshalling and result conversion automatically.

## 4.8 Class Type

### 4.8.1 Description

The Class type represents user-defined types and class definitions. Classes serve as templates for creating objects with specific fields and methods. Classes enable object-oriented programming patterns such as encapsulation, inheritance, and polymorphism.

### 4.8.2 Representation

Classes are represented as objects containing:
- Class name and metadata
- Field definitions (names and types)
- Method definitions (functions associated with the class)
- Constructor and destructor functions
- Parent class reference (for inheritance)

### 4.8.3 Object Instantiation

When a class is instantiated, the runtime allocates memory for an object instance, initializes its fields according to the class definition, and invokes the constructor function if defined.

Object instances maintain a reference to their class, enabling the runtime to perform type checking and method dispatch.

### 4.8.4 Methods and Field Access

Methods are functions associated with a class that receive the object instance as an implicit first parameter (often named `this` or `self`). The runtime handles method lookup and dispatch based on the object's class.

Field access is performed through offset-based addressing within the object's memory layout, ensuring efficient property access without name lookup overhead at runtime.

### 4.8.5 Inheritance

Classes may inherit from parent classes, acquiring their fields and methods. The runtime supports single inheritance, where each class has at most one direct parent class.

Method overriding enables derived classes to provide specialized implementations of inherited methods, with the runtime performing dynamic dispatch to invoke the correct method based on the object's actual type.

## 4.9 Userdata Type

### 4.9.1 Description

The Userdata type represents opaque data managed by the host environment or native code. Userdata enables Suny programs to work with external resources, native data structures, and platform-specific objects without exposing their internal representation to the runtime.

### 4.9.2 Representation

Userdata objects contain:
- Pointer to native data
- Type tag or metadata identifying the userdata type
- Optional destructor function for cleanup
- Reference count for memory management

### 4.9.3 Operations

Operations on Userdata are typically provided by native functions that understand the userdata's internal structure. The Suny runtime does not interpret or manipulate userdata contents directly.

Common patterns include:
- Native functions that accept userdata and perform operations on the underlying native object
- Conversion functions that create userdata from Suny values or extract Suny values from userdata
- Destructor functions that release native resources when userdata is garbage collected

### 4.9.4 Memory Management

Userdata objects participate in reference counting like other Suny objects. When a userdata object's reference count reaches zero, the runtime invokes its destructor function (if provided) before reclaiming memory.

This ensures that native resources such as file handles, network connections, or allocated memory are properly released when no longer needed.

### 4.9.5 Type Safety

The runtime provides mechanisms for type-checking userdata to ensure that native functions receive userdata of the expected type. Type tags or type metadata enable safe casting and validation before native operations are performed.

## 4.10 Type Checking and Validation

The runtime performs type checking during operation execution to ensure type safety. When an operation is performed on a value, the runtime verifies that the value's type is compatible with the operation.

Type checking occurs at runtime rather than during bytecode translation, reflecting Suny's dynamic typing model. Invalid type operations result in runtime errors that terminate execution.

The API provides functions for querying a value's type, performing type checks, and converting between types safely.

## 4.11 Type Conversion

The runtime supports both implicit and explicit type conversion:

**Implicit conversion** occurs automatically in contexts where a specific type is expected. For example, numbers may be implicitly converted to strings in string concatenation operations.

**Explicit conversion** is performed through dedicated conversion functions or opcodes that transform values from one type to another. Explicit conversions may fail if the source value cannot be meaningfully represented in the target type.

Conversion behavior is well-defined for all type pairs, ensuring predictable program behavior across different execution contexts.

# 5 The Application Program Interface

The Suny Application Program Interface (API) provides a comprehensive set of functions and utilities for interacting with the CSuny runtime from host environments and external programs. The API serves as the primary interface between host applications and the Suny execution environment, enabling programs to create runtime instances, execute bytecode, manipulate runtime objects, and integrate Suny functionality into larger systems.

The Suny API is designed with several key principles in mind:

- **Host integration**: The API enables seamless embedding of the Suny runtime into host applications written in C or other languages with C interoperability.
- **Resource control**: The API provides explicit control over runtime lifecycle, memory management, and execution state.
- **Type safety**: The API enforces type distinctions and provides functions for safe type conversion and validation.
- **Error handling**: The API includes comprehensive error reporting mechanisms to detect and communicate runtime errors to the host environment.
- **Performance**: The API is designed to minimize overhead and provide efficient access to runtime functionality.

The API is divided into several logical categories, each addressing a specific aspect of runtime interaction:

- Runtime management: Creating, initializing, executing, and destroying runtime instances
- Object creation and manipulation: Allocating and working with runtime-managed objects
- Type system integration: Creating and working with Suny types and values
- Function invocation: Calling Suny functions from host code and exposing host functions to Suny programs
- Memory management: Explicit reference counting and garbage collector interaction
- Error handling: Detecting, reporting, and recovering from runtime errors

All API functions follow consistent naming conventions, parameter ordering, and return value semantics to ensure predictability and ease of use. Functions that allocate or return objects typically return pointers that must be managed according to the reference counting rules described in section 3.2.

## 5.1 The Stack

The CSuny runtime maintains an execution stack to manage function calls, local variables, and control flow during program execution. The stack is a core component of the runtime's execution model and serves as the primary mechanism for managing execution context as the program transitions between functions, scopes, and control flow constructs.

The stack operates on a last-in-first-out (LIFO) principle. When a function is called, a new stack frame is pushed onto the stack, creating an isolated execution context for that function. When the function returns, its stack frame is popped from the stack, and execution resumes in the caller's context.

The stack is allocated and managed entirely by the runtime. Programs do not directly manipulate the stack structure, but the runtime's opcode execution logic interacts with the stack continuously to implement function calls, parameter passing, local variable storage, and return value handling.

### 5.1.2 Stack Structure

In Suny, the stack is not a standalone data structure. It is embedded within an `Sframe`, which is the fundamental execution frame used by the runtime and implemented in C.

An `Sframe` represents the complete execution state of a single function invocation. The value stack used for argument passing, temporary values, and return values is allocated as part of this frame.

In addition to the stack region, an `Sframe` also maintains:

- storage for local variables,
- references to global values,
- access to constant data associated with the function.

All runtime-visible values during function execution are stored and accessed through the `Sframe`. The lifetime of the stack is therefore bound to the lifetime of its owning `Sframe`.

### 5.1.3 Stack Ownership and Lifetime

- A new stack is created when a function is invoked.
- The stack exists only while the owning Sframe is active.
- When the function returns, the entire stack is discarded.
- Stack values must not be accessed after the frame is destroyed.

The runtime guarantees that stack cleanup is automatic and does not require manual memory management by user-level code.

## 5.2 Functions and Types

In Suny, the implementation is organized into several functional groups. Each group has a specific responsibility and provides a cohesive set of operations for a particular subsystem. Key groups include `Sframe` for stack frame management, global and local variable access; `Seval` for expression evaluation and bytecode execution; `Smem` for memory allocation and heap management; `Sobj` for object initialization and construction; and additional groups for type system operations, garbage collection, and error handling.

Below are the primary functional groups in the Suny runtime. Each group encapsulates related functionality and provides a distinct set of operations.

---

### `Sobj`

`Sobj` is the fundamental **value representation** in SUNY and forms the basis of all runtime computations.

Every value manipulated by the SUNY runtime—such as numbers, strings, booleans, functions, and objects—is represented internally as an `Sobj`. All execution structures, including the stack, locals, globals, constants, and heap references, store **pointers to `Sobj` instances**, not raw values.

`Sobj` serves as the **unified value abstraction** of the runtime. Arithmetic operations, comparisons, function calls, and data movement within the VM operate exclusively on `Sobj` references.

As a core runtime type, `Sobj` is tightly integrated with:

- The execution stack.
- Garbage collection.
- Scope management.
- Object lifetime tracking.

Correct handling of `Sobj` is therefore critical to the correctness, performance, and memory safety of the SUNY runtime.

#### **Sobj_make_number**

```c
SUNY_API struct Sobj* Sobj_make_number(double value);
```

Allocates and initializes a new numeric `Sobj` storing the given double value.
The returned object is managed by the SUNY runtime and participates in garbage collection.
Numeric `Sobj` instances are immutable.

#### **Sobj_make_bool**

```c
SUNY_API struct Sobj* Sobj_make_bool(int value);
```

Allocates and initializes a new boolean `Sobj`.
The provided integer value is interpreted according to SUNY boolean semantics, where `0` represents false and any non-zero value represents true.

#### **Sobj_shallow_copy**

```c
struct Sobj* Sobj_shallow_copy(struct Sobj* obj);
```

Creates a shallow copy of the given `Sobj`.

This function allocates a new `Sobj` instance whose internal structure is copied from the source object. For reference-type fields, the copy shares the same underlying references as the original object.

Notes:

- Primitive values (such as numbers and booleans) are copied by value.
- Reference fields (such as heap objects, tables, or strings) are not duplicated.
- Both the original object and the copied object reference the same underlying data.
- The returned object is managed by the SUNY runtime and participates in garbage collection.

#### **Sobj_deep_copy**

```c
struct Sobj* Sobj_deep_copy(struct Sobj* obj);
```

Creates a deep copy of the given `Sobj`.

This function allocates and initializes a new `Sobj` by recursively copying the contents of the source object. All owned data and nested objects are duplicated, ensuring that the returned object is completely independent of the original.

Unlike Sobj_shallow_copy, no internal references are shared between the source and the copied object.

Notes:

- Primitive types (numbers, booleans) are copied by value.
- Composite or reference types (tables, strings, objects, closures) are fully duplicated.
- Changes made to the copied object do not affect the original.
- Deep copying is more expensive than shallow copying and should be used only when full isolation is required.
- The returned Sobj is managed by the SUNY runtime and participates in garbage collection.

#### **Sobj_make_string**

```c
struct Sobj *Sobj_make_string(char* str, int size);
```

Allocates and initializes a new `Sobj` representing a string value.

This function creates a string-type `Sobj` by copying size bytes from the provided character buffer str. The resulting string object is immutable and managed by the SUNY runtime.

#### **Sobj_make_char**

```c
struct Sobj *Sobj_make_char(char chr);
```

Allocates and initializes a new `Sobj` representing a character value.

This function creates a character-type `Sobj` that stores a single char value. Character objects in SUNY are treated as immutable scalar values and are managed by the SUNY runtime.

### `Seval`

`Seval` is the **evaluation layer** of the SUNY runtime, responsible for performing computations and operations on `Sobj` values.

While `Sobj` defines what a value is, `Seval` defines how values interact.

All runtime operations—such as arithmetic (`+`, `-`, `*`, `/`), comparisons, logical operations, and object-level behaviors—are executed through the `Seval` system. `Seval` consumes one or more `Sobj` references, applies the requested operation according to SUNY semantics, and produces a resulting `Sobj`.

`Seval` operates exclusively on `Sobj` pointers and never on raw C values directly. This guarantees consistent behavior across:

- Different value types (number, string, bool, object, etc.)
- Garbage-collected memory
- Dynamic typing rules

Typical responsibilities of `Seval` include:

- Arithmetic evaluation (`+`, `-`, `*`, `/`, `%`)
- Comparison and equality checks
- Logical operations
- Object and value coercion (if supported)
- Dispatching operations based on `Sobj` type
- Error signaling for invalid operations (e.g. type mismatch)

Because all computation flows through `Seval`, it acts as a **semantic gatekeeper** of the SUNY runtime, enforcing language rules and ensuring that operations remain safe, predictable, and consistent.

Ok, đây là **mục tài liệu hóa chuẩn** cho hàm này, đúng kiểu runtime/VM doc 👇

---

#### **Seval_add**

```c
SUNY_API struct Sobj* Seval_add(struct Sobj *obj1, struct Sobj *obj2);
```

Performs the **addition (`+`) operation** between two `Sobj` values according to SUNY runtime semantics.

`Seval_add` is part of the `Seval` evaluation layer and is responsible for dispatching the addition operation based on the runtime types of its operands.

Behavior:

The function evaluates operands in the following order:

1. **Null check**
   If either operand has type `NULL_OBJ`, a runtime error is raised:

   ```
   attempt to perform arithmetic on a null value
   ```

2. **String addition**
   If both operands are of type `STRING_OBJ`, the operation is delegated to:

   - `eval_string(obj1, obj2, __add)`

   This typically represents string concatenation.

3. **List addition**
   If both operands are of type `LIST_OBJ`, the operation is delegated to:

   - `eval_list(obj1, obj2, __add)`

   This may represent list concatenation or merging, depending on SUNY semantics.

4. **User-defined data**
   If either operand is of type `USER_DATA_OBJ`, the operation is delegated to:

   - `eval_userdata(obj1, obj2, __add)`

   This allows user-defined types to override or customize the `+` operator.

5. **Numeric addition (default)**
   If none of the above cases match, the operands are treated as numeric values and their underlying numeric fields are added:

   ```c
   obj1->value->value + obj2->value->value
   ```

   The result is wrapped into a new numeric `Sobj`.

Returns:

- A newly allocated `Sobj` containing the result of the addition.
- The returned object is managed by the SUNY runtime and participates in garbage collection.

Notes:

- `Seval_add` operates **only on `Sobj` references**, never on raw C values.
- Type dispatch is dynamic and resolved at runtime.
- This function acts as a central enforcement point for SUNY’s `+` operator semantics.
- Extending the `+` operator for new types typically requires updating the corresponding `eval_*` handler.

Nice, hàm này gọn và rất “VM-core” 👍
Đây là **mục tài liệu hóa tương ứng** cho `Seval_sub`, cùng style với `Seval_add`:

---

#### **Seval_sub**

```c
SUNY_API struct Sobj*
Seval_sub(struct Sobj *obj1, struct Sobj *obj2);
```

Performs the **subtraction (`-`) operation** between two `Sobj` values according to SUNY runtime semantics.

`Seval_sub` is part of the `Seval` evaluation layer and handles dynamic dispatch for the subtraction operator.

Behavior:

The evaluation proceeds as follows:

1. **Null check**
   If either operand has type `NULL_OBJ`, a runtime error is raised:

   ```
   attempt to perform arithmetic on a null value
   ```

2. **User-defined data dispatch**
   If either operand is of type `USER_DATA_OBJ`, the operation is delegated to:

   * `eval_userdata(obj1, obj2, __sub)`

   This allows user-defined types to customize the behavior of the subtraction operator.

3. **Numeric subtraction (default)**
   If no special case applies, both operands are treated as numeric values and subtraction is performed on their underlying numeric fields:

   ```c
   obj1->value->value - obj2->value->value
   ```

   The result is wrapped into a new numeric `Sobj`.

Returns:

- A newly allocated `Sobj` containing the result of the subtraction.
- The returned object is managed by the SUNY runtime and participates in garbage collection.

Notes:

- `Seval_sub` operates exclusively on `Sobj` references.
- Unlike `Seval_add`, subtraction does **not** define special behavior for strings or lists.
- Operator overloading is supported only through `USER_DATA_OBJ`.
- Type validation is enforced at runtime.

#### **Seval_mul**

```c
SUNY_API struct Sobj*
Seval_mul(struct Sobj *obj1, struct Sobj *obj2);
```

Performs the **multiplication (`*`) operation** between two `Sobj` values according to SUNY runtime semantics.

`Seval_mul` belongs to the `Seval` evaluation layer and dynamically dispatches the multiplication operator based on operand types.

Behavior:

The function evaluates operands in the following order:

1. **Null check**
   If either operand has type `NULL_OBJ`, a runtime error is raised:

   ```
   attempt to perform arithmetic on a null value
   ```

2. **String multiplication**
   If either operand is of type `STRING_OBJ`, the operation is delegated to:

   - `eval_string(obj1, obj2, __mul)`

   This may represent string repetition or another string-specific behavior defined by SUNY semantics.

3. **List multiplication**
   If either operand is of type `LIST_OBJ`, the operation is delegated to:

   - `eval_list(obj1, obj2, __mul)`

   This may represent list repetition or expansion.

4. **User-defined data dispatch**
   If either operand is of type `USER_DATA_OBJ`, the operation is delegated to:

   - `eval_userdata(obj1, obj2, __mul)`

   This enables operator overloading for custom data types.

5. **Numeric multiplication (default)**
   If none of the above cases apply, both operands are treated as numeric values and their underlying numeric fields are multiplied:

   ```c
   obj1->value->value * obj2->value->value
   ```

   The result is wrapped into a new numeric `Sobj`.

Returns:

- A newly allocated `Sobj` containing the result of the multiplication.
- The returned object is managed by the SUNY runtime and participates in garbage collection.

Notes:

- `Seval_mul` supports polymorphic behavior for strings and lists.
- Operator dispatch is resolved dynamically at runtime.
- Numeric multiplication is the fallback behavior.
- Errors are raised immediately on invalid null operands.

#### **Seval_div**

```c
SUNY_API struct Sobj*
Seval_div(struct Sobj *obj1, struct Sobj *obj2);
```

Performs the **division (`/`) operation** between two `Sobj` values according to SUNY runtime semantics.

`Seval_div` is part of the `Seval` evaluation layer and is responsible for dispatching the division operator at runtime.

Behavior:

The evaluation proceeds as follows:

1. **Null check**
   If either operand has type `NULL_OBJ`, a runtime error is raised:

   ```
   attempt to perform arithmetic on a null value
   ```

2. **User-defined data dispatch**
   If either operand is of type `USER_DATA_OBJ`, the operation is delegated to:

   - `eval_userdata(obj1, obj2, __div)`

   This allows custom data types to define their own division behavior.

3. **Numeric division (default)**
   If no special case applies, both operands are treated as numeric values and division is performed on their underlying numeric fields:

   ```c
   obj1->value->value / obj2->value->value
   ```

   The result is wrapped into a new numeric `Sobj`.

Returns:

- A newly allocated `Sobj` containing the result of the division.
- The returned object is managed by the SUNY runtime and participates in garbage collection.

Notes:

- `Seval_div` does **not** perform an explicit division-by-zero check at this level.
- Division-by-zero behavior depends on the underlying numeric representation (e.g. IEEE-754 semantics).
- String and list types do not define division behavior by default.
- Operator overloading is supported only through `USER_DATA_OBJ`.

### `Sframe`


`Sframe` represents an **execution frame** in the SUNY runtime.

An `Sframe` encapsulates the complete runtime state required to execute a block of code, such as a function call, a script, or a nested execution context. Each frame is independent and forms part of a **frame chain** that models call stacks and lexical scope relationships.

At runtime, the SUNY VM always executes within a current `Sframe`.

---

#### Core Responsibilities

An `Sframe` is responsible for:

- Tracking instruction execution progress.
- Managing the operand stack.
- Holding references to local, global, heap, and constant values.
- Supporting closures and lexical environments.
- Integrating with garbage collection.
- Linking to parent frames for scope resolution.

---

#### Field Overview

##### Code & Control Flow

- `int f_code_index`
  Current instruction pointer (program counter) within `f_code`.

- `struct Scode *f_code`
  Pointer to the bytecode or executable code being evaluated.

- `struct Slabel_map *f_label_map`
  Mapping of labels to instruction offsets, used for jumps and control flow.

- `int code_line`
  Source-level line number corresponding to the current instruction (for debugging and error reporting).

---

##### Operand Stack

- `struct Sobj **f_stack`
  The operand stack used for evaluation.

- `int f_stack_index`
  Index of the current top of the stack.

- `int f_stack_size`
  Total allocated capacity of the stack.

The stack stores **pointers to `Sobj`**, not raw values.

---

##### Local, Global, Heap, and Constant Storage

Each of these areas is stored as an array of `Sobj*` with its own index and size:

- `f_locals`, `f_locals_index`, `f_locals_size`
  Local variables for the current execution frame.

- `f_globals`, `f_globals_index`, `f_globals_size`
  Global variables accessible from this frame.

- `f_heaps`, `f_heap_index`, `f_heap_size`
  Heap-allocated objects referenced by this frame.

- `f_consts`, `f_const_index`, `f_const_size`
  Constants (literals, compiled constants) used during execution.

---

##### Memory Management

- `struct Garbage_pool *gc_pool`
  Garbage collection pool associated with this frame.

This allows the runtime to track object lifetimes relative to frame execution.

---

##### Compilation & Symbol Resolution

- `struct ScompilerUnit *compiler`
  Reference to the compiler unit that produced this frame’s code.

- `struct Stable *table`
  Symbol table used for name resolution and scope lookup.

---

##### Closures & Environment

- `struct Sobj *f_obj`
  Object bound to this frame, typically used for closures or method contexts.

- `struct Senvi *envi`
  Lexical environment captured by the frame.

- `struct Sframe *parent`
  Pointer to the parent frame, forming the call stack and scope chain.

---

#### Summary

`Sframe` is the **central execution unit** of the SUNY runtime.
It binds together code, data, control flow, memory management, and scope into a single structure.

All SUNY execution—from top-level scripts to deeply nested function calls—ultimately occurs within an `Sframe`.

#### **Sframe_push**

```c
struct Sobj *Sframe_push(struct Sframe *frame, struct Sobj *obj);
```