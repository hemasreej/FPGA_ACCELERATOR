#### 04-07-2026 
installment of required tools for the development of the project 
python
cmake
ninja
greenpak
open ocd on chip debugger
gdb  GNU Debugger

`openOCD is the software that connects to the microcontroller, then GDB is the software that lets you control and inspect your program while it's running.`

while project making for the ocd
`cd $env:USERPROFILE\.pico-sdk\openocd\0.12.0+dev\scripts`
& $env:USERPROFILE\.pico-sdk\openocd\0.12.0+dev\openocd.exe `
  -f interface/cmsis-dap.cfg `
  -f target/rp2040.cfg`

## 05/07/2026

**Done:** Hello World attempt on Vicharak — `printf` over USB serial

**Notes:**
- `sleep_ms()` — blocking delay (simple, good for bringup)
- Non-blocking timing — subtract intervals with `to_ms_since_boot(get_absolute_time())`

### Q&A

**Q: Can I use `std::cout` in a `.cpp` file with Pico SDK?**

**A:** You can write C++, but use `printf` for output. A `.cpp` file only enables C++ features — it does not auto-route `cout` anywhere. Pico has no desktop console; output goes through Pico stdio → USB/UART → serial monitor on PC.

**Q: Why `printf` and not `cout`?**

**A:** `printf` works with `stdio_init_all()` + `pico_enable_stdio_usb(1)` out of the box. `cout` needs extra code to redirect to USB/UART. Not worth it for this project.

**Q: What is the minimum working print setup?**

**A:**
```cpp
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    printf("Hello\n");
    sleep_ms(1000);
}
```

**Q: `sleep_ms` vs interval subtraction?**

**A:** `sleep_ms(1000)` — blocks the CPU for 1s; fine for Hello World. Interval subtraction — check `now - last >= 1000` in a loop without blocking; needed later for FreeRTOS tasks that must not freeze.

## next is freertos adding into the project folder
`git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git --depth 1`

---

## Study Task — Understand FreeRTOS Kernel (do before wiring CMake)

**Goal:** Explain how the scheduler works in your own words. No coding yet — read and answer.

### Part 1 — Big picture (30 min)

Draw this on paper and label what runs on RP2040:

```
main()  →  xTaskCreate()  →  vTaskStartScheduler()
                                    ↓
                            [scheduler picks task]
                                    ↓
                    Task A runs → delay/block → Task B runs → ...
```

**Learn these 6 words:**

| Term | One-line meaning |
|------|------------------|
| Task | A function that never returns; runs in a loop |
| Scheduler | Picks which task runs on the CPU |
| Tick | Timer interrupt every 1 ms (`configTICK_RATE_HZ`) |
| Preemption | Higher-priority task can interrupt lower one |
| Context switch | Save Task A state, load Task B state |
| Idle task | Lowest-priority task when nothing else is ready |

---

### Part 2 — Files to open in your repo (read in order)

| # | File | What to look for |
|---|------|------------------|
| 1 | `FreeRTOS-Kernel/include/FreeRTOS.h` | Core types, `pdTRUE`/`pdFALSE`, includes |
| 2 | `FreeRTOS-Kernel/include/task.h` | `xTaskCreate`, `vTaskDelay`, `vTaskStartScheduler` |
| 3 | `FreeRTOS-Kernel/tasks.c` | Search `vTaskStartScheduler` — what happens at start |
| 4 | `FreeRTOS-Kernel/list.c` | Tasks stored as linked lists (ready, blocked, delayed) |
| 5 | `FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include/portmacro.h` | **Your Pico port** — tick, critical sections |
| 6 | `freertos/FreeRTOSConfig.h` | *(you will write this)* — heap size, tick rate, priorities |

**RP2040 note:** Pico uses the port under `portable/ThirdParty/GCC/RP2040/` — not ARM_CM0 generic.

---

### Part 3 — Answer these (write in learnings.md Q&A)

1. What happens to `main()` after `vTaskStartScheduler()`? Can you put `while(true)` in main?
2. What is `vTaskDelay(500)` doing inside the kernel? (hint: task state → blocked)
3. Why must ISR code use `xQueueSendFromISR` not `xQueueSend`?
4. What does `portYIELD_FROM_ISR()` do and when is it needed?
5. What is `configTOTAL_HEAP_SIZE` and what happens if malloc fails?
6. RP2040 has 2 cores — which core runs your tasks with this port?

---

### Part 4 — Map to your project (report)

| Your future task | FreeRTOS piece |
|------------------|----------------|
| Heartbeat LED blink | 1 task, `vTaskDelay`, priority 1 |
| UART CLI | task blocks on `xQueueReceive` |
| Controller | priority 3, preempts logger/CLI |
| FPGA bridge access | binary semaphore between tasks |
| `tasks` CLI command | `vTaskList`, `vTaskGetRunTimeStats` |

---

### Part 5 — Mini exercise (no board needed)

Write **pseudocode only** (not real C++) for:

```
vHeartbeatTask:
  forever:
    toggle LED
    wait 500 ms
```

Then answer: which FreeRTOS API replaces `wait 500 ms`? (`vTaskDelay` + `pdMS_TO_TICKS`)

---

### Pass criterion

You are ready for CMake wiring when you can explain aloud:

> "I create tasks with `xTaskCreate`, start the scheduler with `vTaskStartScheduler`, and each task yields with `vTaskDelay` so other tasks can run."

Reply with your **Part 3 answers** (even rough) — I will correct them.

**Fix before coding:** rename `freertos/freeRTOSConfig.h` → `freertos/FreeRTOSConfig.h` (exact spelling matters for CMake).

--> freeRTOS never time slices between the tasks and also it uses preemptive scheduling

---

## Timer/Scheduler Callback Delay: Positive vs Negative

**Context:** SDKs that schedule repeating callbacks (e.g. `setInterval`-style APIs) often let you pass a signed delay value. The sign changes how the next call is timed.

### 1. Positive Delay (`+2000`)

**Behavior:** Delay from *end* to *start*.

The SDK waits 2000 ms **after** your callback function finishes executing before scheduling the next call.

**Effect:** If your callback takes 100 ms to run, the total repeat cycle becomes $2000\text{ ms} + 100\text{ ms} = 2100\text{ ms}$. This causes the timer to **drift** over time.

### 2. Negative Delay (`-2000`)

**Behavior:** Delay from *start* to *start* (targets a fixed period).

The SDK takes the absolute value ($|-2000| = 2000\text{ ms}$) and schedules the next call to occur exactly 2000 ms after the **start** of the current invocation — regardless of how long the callback took to run inside that window.

**Effect:** Eliminates timing drift and guarantees a strict, deterministic frequency (e.g. exactly $0.5\text{ Hz}$ for a $2000\text{ ms}$ period).

### Visual Comparison

Callback execution time = 100 ms:

```
Positive Delay (+2000 ms):
|-- Run Callback (100ms) --|------ Pause 2000ms ------|-- Run Callback (100ms) --|
|<------------------------ Total Period = 2100ms ------------------------>|

Negative Delay (-2000 ms):
|-- Run Callback (100ms) --|--- Pause 1900ms ---|-- Run Callback (100ms) --|
|<---------------------- Total Period = 2000ms ---------------------->|
```

### When to use which?

- **Negative delay (`-2000`)** — use when precision and strict timing intervals matter (e.g. LED blinking rates, sampling sensor data, PWM signals, audio processing, clock scheduling).
- **Positive delay (`+2000`)** — use when callback execution time varies significantly, or when you specifically want to guarantee a minimum rest period between callback executions to prevent overlap or resource starvation.

--> every freeRTOS task is an infinite loop running that never returns .. once the task is completed it will give the control back tot he scheduler