#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TOTAL_MEMORY 500
#define BAR_WIDTH 50

typedef struct {
    int processId;
    int offset;
    int length;
    bool inUse;
} MemorySegment;

MemorySegment memLayout[100];
int segmentCount = 0;

void displayMemoryStatus() {
    printf("\n[Memory Allocation Table]\n");
    printf("%-10s %-10s %-10s\n", "Status", "Size", "Offset");
    for (int i = 0; i < segmentCount; i++) {
        if (memLayout[i].inUse) {
            printf("%-10s %-10d %-10d\n", ("PID " + (char[10]){0}, sprintf((char[10]){0}, "%d", memLayout[i].processId), memLayout[i].length, memLayout[i].offset));
        } else {
            printf("%-10s %-10d %-10d\n", "FREE", memLayout[i].length, memLayout[i].offset);
        }
    }

    printf("\n[Memory Visualization]\n|");
    int cursor = 0;
    for (int i = 0; i < segmentCount; i++) {
        int blank = memLayout[i].offset - cursor;
        int blankUnits = (blank * BAR_WIDTH) / TOTAL_MEMORY;
        for (int j = 0; j < blankUnits; j++) printf(" ");

        int units = (memLayout[i].length * BAR_WIDTH) / TOTAL_MEMORY;
        char symbol = memLayout[i].inUse ? '#' : '.';
        for (int j = 0; j < units; j++) printf("%c", symbol);

        cursor = memLayout[i].offset + memLayout[i].length;
    }
    if (cursor < TOTAL_MEMORY) {
        int remaining = ((TOTAL_MEMORY - cursor) * BAR_WIDTH) / TOTAL_MEMORY;
        for (int j = 0; j < remaining; j++) printf(" ");
    }

    printf("|\nLegend: [# = Allocated] [.] = Free]\n");
    printf("-----------------------------\n");
}

void allocateProcess(int processId, int reqSize) {
    // Check for duplicate PID
    for (int k = 0; k < segmentCount; k++) {
        if (memLayout[k].inUse && memLayout[k].processId == processId) {
            printf("Process %d already exists.\n", processId);
            return;
        }
    }

    int offset = 0;

    for (int i = 0; i <= segmentCount; i++) {
        int gap = (i == segmentCount) ? TOTAL_MEMORY - offset : memLayout[i].offset - offset;

        if (gap >= reqSize) {
            for (int j = segmentCount; j > i; j--) {
                memLayout[j] = memLayout[j - 1];
            }
            memLayout[i].processId = processId;
            memLayout[i].offset = offset;
            memLayout[i].length = reqSize;
            memLayout[i].inUse = true;
            segmentCount++;
            printf("Allocated: PID %d | Size %d | Offset %d\n", processId, reqSize, offset);
            return;
        }

        if (i < segmentCount) {
            offset = memLayout[i].offset + memLayout[i].length;
        }
    }

    printf("Failed to allocate PID %d (Size %d): Not enough space.\n", processId, reqSize);
}

void deallocateProcess(int processId) {
    for (int i = 0; i < segmentCount; i++) {
        if (memLayout[i].inUse && memLayout[i].processId == processId) {
            memLayout[i].inUse = false;
            printf("Deallocated process %d.\n", processId);
            return;
        }
    }
    printf("Process %d not found.\n", processId);
}

void compactMemory() {
    int compactOffset = 0;

    for (int i = 0; i < segmentCount; i++) {
        if (memLayout[i].inUse) {
            if (memLayout[i].offset != compactOffset) {
                memLayout[i].offset = compactOffset;
            }
            compactOffset += memLayout[i].length;
        }
    }

    int newCount = 0;
    for (int i = 0; i < segmentCount; i++) {
        if (memLayout[i].inUse) {
            memLayout[newCount++] = memLayout[i];
        }
    }
    segmentCount = newCount;

    printf("Memory compacted.\n");
}

int main() {
    printf("Dynamic Memory Management Simulation\n");

    allocateProcess(10, 120);   // 0–120
    allocateProcess(20, 160);   // 120–280
    allocateProcess(30, 130);   // 280–410
    displayMemoryStatus();

    deallocateProcess(20);      // Free PID 20
    displayMemoryStatus();

    allocateProcess(40, 190);   // Should fail pre-compaction
    displayMemoryStatus();

    compactMemory();            // Compact memory
    displayMemoryStatus();

    allocateProcess(40, 190);   // Should succeed after compaction
    displayMemoryStatus();

    return 0;
}
