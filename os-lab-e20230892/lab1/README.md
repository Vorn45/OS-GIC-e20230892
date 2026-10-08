# Lab 1 Repoe
rt: Introduction to Operating Systems

**Student Name:** Piseth Panhavorn  
**Student ID:** e20230892  
**Ubuntu username on the server:** gic-piseth-panhavorn  
**My values (oslab values lab1):** file1 = silver, file2 = lemon, count = 3  

---

## Task 1: Operating System Identification

* **Observations:** Running `uname -a` and `lsb_release -a` revealed the kernel build string and the active Linux distribution.
* **Kernel Version:** `5.15.0-generic` (identified from the 3rd field of `uname -a` / `uname -r`). The kernel manages core hardware resources, CPU scheduling, and memory.
* **Distribution Version:** `Ubuntu 22.04 LTS` (identified from `lsb_release -a`). The distribution bundles the Linux kernel with userland system utilities, libraries, and package managers.

---

## Task 2: Essential Linux File and Directory Commands

* **Workflow & Experience:** Created a working directory `task2_files` and tracked working directories with `pwd`. Created `silver.txt` and `lemon.txt` using `touch`, populated both with `echo`, and verified their text via `cat`. Copied `silver.txt` to `silver_copy.txt` using `cp`, renamed `lemon.txt` to `lemon_renamed.txt` using `mv`, and deleted the copy with `rm`.
* **Final `ls` output:** The final listing in `task2_file_commands.txt` shows only the original and renamed files:
  ```text
  lemon_renamed.txt
  silver.txt

