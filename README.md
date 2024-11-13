# Xinu Operating Systems Design Project

This repository contains Project #1 for the ECE465/565 Operating Systems Design course, focusing on the Xinu operating system. The main objectives of this project include understanding the Xinu codebase, process management, writing test cases, and handling the process stack.

## Project Objectives:
- Familiarize with Xinu’s architecture and code structure.
- Explore process creation, management, and scheduling in Xinu.
- Implement solutions to coding problems involving process handling and control.
- Practice writing and executing comprehensive test cases to verify implementations.

## Key Coding Challenges:

### Problem 1: Cascading Termination
- **Objective**: Modify Xinu to support cascading termination for user processes while maintaining the integrity of system processes.
- **Implementation**:
  - Extend the `PCB` structure by adding a `user_process` flag to differentiate between user and system processes.
  - Modify the `kill` function to handle cascading termination when the `user_process` flag is `TRUE`.
  - Ensure correct termination of child processes upon the termination of their parent processes.
- **Testing**: Write a test case file (`main.kill`) that:
  - Spawns at least 15 user processes.
  - Validates process termination and cascading effects by selectively terminating processes and observing the changes in active processes.
  - Outputs the process tree structure before and after termination.

### Problem 2: Process Creation and Stack Handling (Fork System Call)
- **Objective**: Implement a `fork` system call similar to Unix’s `fork`, creating a child process that inherits properties from the parent.
- **Implementation**:
  - Create a new `fork.c` in the `system` folder.
  - Ensure the child process has the same properties (e.g., name, priority, stack) as the parent and initializes its `PCB` fields correctly.
  - Return the PID of the child to the parent and a predefined value (`NPROC`) to the child.
- **Challenges**:
  - Correct stack handling during process duplication.
  - Ensuring the child process resumes from the appropriate instruction after creation.
- **Testing**: Use the provided `main.fork` test cases to verify proper execution, checking for expected outputs and process behavior.

## Project Report and Submission
The project report includes:
- Answers to theoretical questions about Xinu’s process management.
- Descriptions of implemented solutions and modified/added files.
- An illustration of the process tree used for testing cascading termination.
- Results from running the test cases, ensuring that outputs align with project requirements.

## How to Run
1. Place test case files (`main.kill`, `main.fork`) in the `testcases` folder.
2. Compile the project:
   ```bash
   cd xinu/compile
   make clean
   make


---

