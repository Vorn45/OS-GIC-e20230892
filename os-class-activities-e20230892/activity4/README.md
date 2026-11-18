# Class Activity 4 — Shared File API

- **Student Name:** Piseth Panhavorn
- **Student ID:** e20230892
- **Partner Name:** [Partner Name]
- **Partner Student ID:** [Partner ID]
- **Server Machine Owner:** [Your Name or Partner Name]
- **Server IP Address:** [Example: 192.168.1.25]

---

## Task 1: C++ Before Mutex

![C++ before mutex](screenshots/cpp_before_mutex.png)

- Expected score after 20 total client requests: 20
- Actual score: [Record actual score from your run]
- What happened: Multiple client requests arrived concurrently, and the server spawned a separate thread for each connection without any locking around `update_score()`. Because each thread read `shared_score.txt`, slept for 200 ms, incremented its local copy, and wrote back to the file, several threads read the same stale score and overwrote each other's updates (lost updates due to a race condition).

---

## Task 2: C++ After Mutex

![C++ after mutex](screenshots/cpp_after_mutex.png)

- Expected score after 20 total client requests: 20
- Actual score: [Record actual score from your run — should be 20]
- What changed after adding mutex: Adding `std::lock_guard<std::mutex> lock(file_mutex);` inside `update_score()` ensured mutual exclusion so only one server thread could read, sleep, increment, and write `shared_score.txt` at a time. Other threads waited until the lock was released, eliminating lost updates and consistently producing the final score of 20.

---

## Task 3: Java Before Synchronized

![Java before synchronized](screenshots/java_before_synchronized.png)

- Expected score after 20 total client requests: 20
- Actual score: [Record actual score from your run]
- What happened: Each incoming client socket was handled in a new thread calling the unsynchronized `updateScore()` method. Concurrent threads read the same value from `shared_score.txt` before any thread finished sleeping and writing back, causing overlapping read-modify-write operations and lost updates (resulting in a final score less than 20).

---

## Task 4: Java After Synchronized

![Java after synchronized](screenshots/java_after_synchronized.png)

- Expected score after 20 total client requests: 20
- Actual score: [Record actual score from your run — should be 20]
- What changed after adding synchronized: Marking `updateScore()` as `private static synchronized` acquired the class-level intrinsic monitor lock (`ScoreServerSync.class`) before entering the method. Only one thread could execute the critical section at a time, serializing access to `shared_score.txt` and guaranteeing a consistent final score of 20.

---

## Questions

1. **Why should clients send requests to the server instead of writing the file directly?**

   > Letting multiple clients on different machines edit a shared file directly causes uncoordinated concurrent access, race conditions, and potential file corruption—plus clients may not even share direct filesystem access. Routing requests through a single server API centralizes ownership of `shared_score.txt` so that all reads and writes go through one controlled point where validation, access control, and synchronization can be enforced.

2. **Why does the server still have a race condition before mutex or synchronized?**

   > Even though only one server process accesses `shared_score.txt`, the server creates a separate thread per client (`std::thread(...).detach()` in C++ and `new Thread(...).start()` in Java). Those threads run concurrently and execute the non-atomic read-modify-write sequence (read file → sleep 200 ms → increment → write file) at the same time. Multiple threads read the same old value before another thread writes its incremented value back, leading to lost updates.

3. **In the C++ fixed version, what does `std::lock_guard<std::mutex>` protect?**

   > `std::lock_guard<std::mutex> lock(file_mutex)` protects the critical section inside `update_score()`—specifically the entire read-modify-write operation on `shared_score.txt` (reading the current score, sleeping, incrementing, writing the new score, and printing the log message). Using RAII, it acquires `file_mutex` when constructed at the start of `update_score()` and automatically releases it when `lock` goes out of scope at the end of the function (even if an exception occurs).

4. **In the Java fixed version, what does `synchronized` protect?**

   > The `synchronized` keyword on `private static synchronized int updateScore(String name)` protects the entire `updateScore()` method as a critical section by locking on the `ScoreServerSync.class` monitor object. Only one thread can hold this monitor and perform the read-sleep-increment-write sequence on `shared_score.txt` at any given time; all other threads calling `updateScore()` block until the current thread exits the method and releases the monitor.

5. **Why is the final score expected to be 20 when Student A sends 10 requests and Student B sends 10 requests?**

   > `shared_score.txt` starts at `0`, and each client request increments the stored score by `1`. With 10 requests from Student A and 10 requests from Student B (20 requests total), if every increment is applied atomically without any lost updates, the score transitions sequentially from `0` to `20`.

6. **What could happen if two separate servers update the same file at the same time?**

   > A race condition would happen again. Both `std::mutex` in C++ and Java's `synchronized` only synchronize threads within a **single process** (they live in that process's memory space). Two separate server processes have independent locks and cannot coordinate with each other using in-process mutexes alone, so they could read and overwrite `shared_score.txt` simultaneously, causing lost updates or corrupted file contents. To synchronize across separate processes, inter-process or OS-level mechanisms (such as OS file locks like `flock`/`fcntl`, named semaphores, or a database) would be required.

---

## Reflection

Both C++ (`std::mutex` with `std::lock_guard`) and Java (`synchronized`) solve the exact same underlying concurrency problem—enforcing mutual exclusion on a critical section so that concurrent read-modify-write operations do not interleave and lose updates—but they express it differently:

- **C++ (`std::mutex` + `std::lock_guard`)** is explicit and flexible: you declare a dedicated mutex object (`file_mutex`) and explicitly lock it for a chosen scope using an RAII guard (`std::lock_guard`), which guarantees automatic unlocking when leaving scope and lets you narrow the lock to only the exact lines that need protection.
- **Java (`synchronized`)** is built directly into the language and JVM via intrinsic monitor locks: adding a single keyword to a `static` method automatically locks on the class object for the entire method body without manually declaring a separate lock variable, making simple synchronization concise (though less granular unless using `synchronized (lockObj) { ... }` blocks or `java.util.concurrent.locks.ReentrantLock`).

Overall, this activity showed that centralizing a shared resource behind a single server is only the first step. Because real servers are multithreaded to handle concurrent clients, every shared resource accessed by multiple threads—whether in memory or on disk—still forms a critical section that must be explicitly synchronized to guarantee correctness and prevent subtle timing-dependent race conditions.
