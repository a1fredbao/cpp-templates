---
title: Binary Alarm
documentation_of: ./src/alfred/data_structure/binary-alarm.hpp
---

## Description

A data structure supporting increasing single position values and alerting when the sum of a subset of elements exceeds a predefined threshold. It is 0-indexed.

## Usage Example

```cpp
#include "binary-alarm.hpp"
#include <vector>
#include <iostream>

int main() {
    BinaryAlarm<long long, 20> ba(5);
    int alarm_id = ba.monitor({0, 1, 2}, 10); // alert when a[0] + a[1] + a[2] >= 10
    ba.increase(0, 4);
    ba.increase(1, 5);
    auto alarms = ba.fetch(); // Empty
    ba.increase(2, 3);
    alarms = ba.fetch(); // Contains alarm_id
    std::cout << "Alarm triggered: " << alarms[0] << "\n";
    return 0;
}
```

## Main Features
- Incremental value updates on single positions.
- Alerting when the sum of a subset of indexes exceeds a threshold.
- Amortized efficient rebuilding using binary structures.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `monitor` | Registers a set of indices to monitor with a threshold | $O(K \log K)$ where $K$ is the size of the set |
| `increase` | Increases the value of a single position | $O(V \log K)$ amortized |
| `fetch` | Fetches all triggered alarms and clears the list | $O(1)$ |

## Notes
- `V` represents the base-2 logarithm of the maximum modification or alarm threshold.
- Rebuilding is done lazily when updates are close to triggering the threshold.
