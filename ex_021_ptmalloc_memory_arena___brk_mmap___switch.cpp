#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <iomanip>

void inspect_chunk(const char* label, void* ptr) {
    if (!ptr) {
        std::cout << "--- " << label << " -> ALLOCATION FAILED ---\n\n";
        return;
    }
    uint64_t* hidden_header = reinterpret_cast<uint64_t*>(ptr) - 1;
    uint64_t raw_value = *hidden_header;
    
    uint64_t real_size = raw_value & ~7;
    bool prev_inuse = raw_value & 0x1;
    bool is_mmapped = raw_value & 0x2;
    bool non_main_arena = raw_value & 0x4;

    std::cout << "--- " << label << " ---\n";
    std::cout << "Raw Header Hex    : 0x" << std::hex << raw_value << std::dec << "\n";
    std::cout << "Ledger Size field : " << real_size << " bytes\n";
    std::cout << "IS_MMAPPED (0x2)  : " << (is_mmapped ? "**SET (mmap)**" : "**CLEARED (brk/heap)**") << "\n\n";
}

int main() {
    // 1. Exactly at the maximum payload boundary for 32 pages (131072 - 8)
    size_t boundary_payload = 131064; 
    void* p1 = std::malloc(boundary_payload);

    // 2. Drop down by one more 16-byte alignment step to be absolutely safe
    size_t safe_heap_payload = 131048; 
    void* p2 = std::malloc(safe_heap_payload);

    inspect_chunk("Size 131,064 (Max 32-page payload)", p1);
    inspect_chunk("Size 131,048 (Safe sub-threshold payload)", p2);

    std::free(p1);
    std::free(p2);
    return 0;
}
