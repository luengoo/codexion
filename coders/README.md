# 💽🖥💾💻CODEXION💻💾🖥💽

*Este proyecto ha sido creado como parte del currículum de 42 por alluengo*

## **Description**
Welcome to my Codexion 42 project!

This is a thread-managing focused program, using mutex and controling the data races in a process. In this project I realised how real programs work in the inside, using the resources such as libraries or variables at the same time.

Imagine a program is an office. Well, the *threads* can be interpreted as the people inside working all together in a single project. If one of them uses the printer, the next person that wants to use it must wait. That is, in programing, what is meant for the *MUTEX*.

Here, we use dongles and programmers. A X number of programmers seat in a round table. Each person have 1 dongle, but, to complete the compilation, they need 2. They must coordinate, prevent interblocking and starving.
Each person must compile, debug and refactor. While they compile, they hold the dongles, whenever they finish compile, the dongles are released. Each dongle has a cooldown time that coders must respect. The simulation stops whenever someone burnsout, that can happen if that coder doesn't compile on time.

The simulation also ends normally when every coder has compiled at least `number_of_compiles_required` times.

## **Instrucions**

### **Compilation**

```bash
make
```

### **Execution**

```bash
./codexion nb_coders time_to_burnout time_to_compile time_to_debug time_to_refactor nb_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory. `scheduler` must be `fifo` or `edf`.

### **Examples**

```bash
# 4 coders, burnout at 400ms, compile 200ms, debug 200ms, refactor 400ms, 5 compiles each, 0ms cooldown, FIFO
./codexion 4 400 200 200 400 5 0 fifo

# Same but EDF scheduler
./codexion 4 400 200 200 400 5 0 edf

# 1 coder (no deadlock possible)
./codexion 1 800 200 200 200 3 50 fifo
```

## **Resources**

- [POSIX Threads Programming – Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [Dining Philosophers Problem – Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [pthread_cond_timedwait – man page](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)
- [Earliest Deadline First scheduling – Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Binary heap – Wikipedia](https://en.wikipedia.org/wiki/Binary_heap)

### **AI usage**

Claude (Anthropic) was used to:
- Explain POSIX threading concepts (mutex, condition variables, timedwait)
- Review the deadlock prevention strategy (asymmetric dongle acquisition order)
- Clarify the EDF scheduling algorithm and tie-breaker rules
- Suggest the per-request condition variable pattern for fair wakeup

All generated code was reviewed, understood, and validated line by line before inclusion.

## **Blocking cases handled**

### **Deadlock prevention (Coffman's conditions)**
A deadlock requires four conditions: mutual exclusion, hold-and-wait, no preemption, and circular wait. We break the **circular wait** condition using the asymmetric acquisition strategy: all coders except the last one take their left dongle first then their right dongle. The last coder takes them in reverse order (right then left). This breaks the circular dependency chain that would otherwise form when every coder holds their left dongle and waits for their right one.

### **Starvation prevention**
Under FIFO scheduling, each coder is served strictly in arrival order, guaranteeing no starvation as long as parameters are feasible. Under EDF, coders with the earliest burnout deadline are served first. A liveness guarantee holds as long as `time_to_compile + 2 * dongle_cooldown < time_to_burnout`, because a coder will always eventually become the most urgent and be served.

### **Cooldown handling**
After a dongle is released, it is marked unavailable until `now + dongle_cooldown` milliseconds. The next waiter uses `pthread_cond_timedwait` with an absolute timeout equal to `available_at`, so it sleeps precisely until the cooldown expires without busy-waiting.

### **Precise burnout detection**
A dedicated monitor thread polls all coders' `last_compile_start` timestamps every 1 ms. When `now - last_compile_start[i] >= time_to_burnout`, the monitor immediately prints the burnout message and sets `running = 0`. The 1 ms polling interval ensures the message is displayed well within the required 10 ms tolerance.

### **Log serialization**
All `printf` calls are wrapped in `ft_log()` and `ft_log_burnout()`, which lock a shared `log_mutex` before writing. This guarantees that no two log lines ever interleave, even when many threads print simultaneously.

## **Thread synchronization mechanisms**

### `pthread_mutex_t`
- **`log_mutex`**: serializes all output. Locked around every `printf` call.
- **`running_mutex`**: protects the `running` flag shared between all threads and the monitor.
- **`dongle[i].mutex`**: protects each dongle's state (`in_use`, `available_at`, `queue`). Coders lock this mutex before pushing their request into the heap or checking dongle availability.

### `pthread_cond_t` (per-request condition variable)
Each `t_request` has its own `pthread_cond_t cond`. When a coder pushes its request into a dongle's heap, it calls `pthread_cond_wait(&req.cond, &dongle.mutex)` and sleeps. When the current dongle holder calls `dongle_release()`, it pops the next request from the heap and calls `broadcast(&next_req->cond)` — waking only the correct next coder. This prevents the thundering herd problem where all waiters would wake up simultaneously.

### `pthread_cond_timedwait`
Used when a coder is next in the queue but the dongle cooldown has not yet expired. Instead of busy-waiting, the coder computes the exact absolute `struct timespec` for `available_at` and sleeps until that moment. It is woken earlier if another signal arrives (e.g., if the simulation stops).

### **Race condition prevention example**
Without `log_mutex`:
- Thread 1 calls `printf("100 1 is comp)`
- Thread 2 calls `printf("100 2 is debugging\n")`
- Output: `100 1 is comp100 2 is debugging\niling\n` ← corrupted

With `log_mutex`, each `printf` is atomic from the perspective of other threads.

### **Monitor–coder communication**
The monitor sets `sim->running = 0` under `running_mutex`. Each coder checks `is_running()` (which locks `running_mutex`) between phases. This is the only communication channel: coders do not need to know why the simulation stopped, only that it did.

`finished = 0` is setted in the header too. This is made so the loop ends whenever all coders compile, debug and refractor, if not everything would end whenever the last compilation is done, without debuggin and refractoring.


## **Conclusion**

This exercise made me go back to C again. Since my last C project, I realised how much I have learnt in the process of migration to Python. I've been able to change easily, as I have my C bases pretty strongly attached to my progression.
Thanks for checking out my program!

*Signed by Álvaro Luengo*

