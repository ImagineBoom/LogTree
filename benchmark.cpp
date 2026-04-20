#include "module_log_v8.2.h"
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

using namespace module_log;
using namespace std::chrono;

std::string format_duration(double seconds) {
    if (seconds < 1e-6) {
        return std::to_string(seconds * 1e9) + " ns";
    } else if (seconds < 1e-3) {
        return std::to_string(seconds * 1e6) + " µs";
    } else if (seconds < 1) {
        return std::to_string(seconds * 1e3) + " ms";
    } else {
        return std::to_string(seconds) + " s";
    }
}

int main() {
    const size_t TOTAL_NODES = 1000000;  // 1M total nodes
    
    std::cout << "\n" << std::string(100, '=') << "\n";
    std::cout << "LogTree V8.1 - Realistic End-to-End Benchmark\n";
    std::cout << "Scenario: 1 Create + 1M Add/Array calls + 1 Format\n";
    std::cout << std::string(100, '=') << "\n\n";
    
    // ===== Step 1: CREATE =====
    std::cout << "📦 Step 1: Creating tree...\n";
    auto create_start = high_resolution_clock::now();
    auto tree = LogTree::create("Root", 0x01, 8);
    auto create_end = high_resolution_clock::now();
    double create_time = duration_cast<nanoseconds>(create_end - create_start).count() / 1e9;
    std::cout << "   Time: " << format_duration(create_time) << "\n\n";
    
    // ===== Step 2: ADD/ARRAY APIs (1M total calls) =====
    std::cout << "➕ Step 2: Adding 1M nodes (mixed APIs)...\n";
    
    size_t count_simple = 0;
    size_t count_standard = 0;
    size_t count_str_decoded = 0;
    size_t count_num_decoded = 0;
    size_t count_dual_value = 0;
    size_t count_array_auto = 0;
    size_t count_array_indexed = 0;
    size_t count_dual_array = 0;
    
    auto add_start = high_resolution_clock::now();
    
    for (size_t i = 0; i < TOTAL_NODES; ++i) {
        switch (i % 8) {
            case 0:  // Simple node
                tree.add("Root", "Simple");
                ++count_simple;
                break;
                
            case 1:  // Standard node
                tree.add("Root", "Standard", i * 0x10, 16);
                ++count_standard;
                break;
                
            case 2:  // String decoded
                tree.add("Root", "StrDec", i, 8, "OK");
                ++count_str_decoded;
                break;
                
            case 3:  // Numeric decoded
                tree.add("Root", "NumDec", i, 8, i * 2, 16);
                ++count_num_decoded;
                break;
                
            case 4:  // Dual value
                tree.add("Root", "Dual", i, "Decoded", i * 2);
                ++count_dual_value;
                break;
                
            case 5:  // Array auto
                tree.add_array_element_auto("Root", "AutoArr", static_cast<float>(i) * 1.5f, i, 16);
                ++count_array_auto;
                break;
                
            case 6:  // Array indexed
                tree.add_array_element("Root", "IdxArr", i / 8, static_cast<float>(i) * 2.0f, i, 16);
                ++count_array_indexed;
                break;
                
            case 7:  // Dual array (adds 2 elements per call)
                tree.add_dual_array_elements(
                    "Root",
                    "Input", static_cast<float>(i) * 1.0f, i, 16,
                    "Output", static_cast<float>(i) * 2.0f, i * 2, 16
                );
                ++count_dual_array;
                break;
        }
    }
    
    auto add_end = high_resolution_clock::now();
    double add_time = duration_cast<nanoseconds>(add_end - add_start).count() / 1e9;
    double add_per_node = (add_time / TOTAL_NODES) * 1e9;
    double add_throughput = TOTAL_NODES / add_time / 1e6;
    
    std::cout << "   Distribution:\n";
    std::cout << "     Simple nodes:      " << count_simple << "\n";
    std::cout << "     Standard nodes:    " << count_standard << "\n";
    std::cout << "     String decoded:    " << count_str_decoded << "\n";
    std::cout << "     Numeric decoded:   " << count_num_decoded << "\n";
    std::cout << "     Dual value:        " << count_dual_value << "\n";
    std::cout << "     Array (auto):      " << count_array_auto << "\n";
    std::cout << "     Array (indexed):   " << count_array_indexed << "\n";
    // Calculate total nodes (root + all added)
    size_t total_nodes_created = 1 + count_simple + count_standard + count_str_decoded + 
                                  count_num_decoded + count_dual_value + count_array_auto + 
                                  count_array_indexed + count_dual_array * 2;
    
    std::cout << "     Dual array calls:  " << count_dual_array << " (" << count_dual_array * 2 << " elements)\n";
    std::cout << "     ─────────────────────────────────\n";
    std::cout << "     Total nodes:       " << total_nodes_created << " (including root)\n";
    std::cout << "   Time: " << format_duration(add_time) << "\n";
    std::cout << "   Per node: " << format_duration(add_time / TOTAL_NODES) << "\n";
    std::cout << "   Throughput: " << std::to_string((int)add_throughput) << "M ops/sec\n\n";
    
    // ===== Step 3: FORMAT =====
    std::cout << "📤 Step 3: Formatting output...\n";
    auto fmt_start = high_resolution_clock::now();
    std::string output = tree.format();
    auto fmt_end = high_resolution_clock::now();
    double fmt_time = duration_cast<nanoseconds>(fmt_end - fmt_start).count() / 1e9;
    double fmt_per_node = (fmt_time / total_nodes_created) * 1e9;
    double fmt_throughput = total_nodes_created / fmt_time / 1e6;
    
    std::cout << "   Nodes formatted: " << total_nodes_created << "\n";
    std::cout << "   Output size: " << output.size() / 1024 << " KB (" 
              << output.size() / 1024 / 1024 << " MB)\n";
    std::cout << "   Time: " << format_duration(fmt_time) << "\n";
    std::cout << "   Per node: " << format_duration(fmt_time / total_nodes_created) << "\n";
    std::cout << "   Throughput: " << std::to_string((int)fmt_throughput) << "M nodes/sec\n\n";
    
    // ===== SUMMARY =====
    double total_time = create_time + add_time + fmt_time;
    
    std::cout << std::string(100, '=') << "\n";
    std::cout << "📊 PERFORMANCE SUMMARY\n";
    std::cout << std::string(100, '=') << "\n\n";
    
    std::cout << std::left << std::setw(30) << "Operation"
              << std::right << std::setw(15) << "Time"
              << std::setw(20) << "Percentage"
              << "\n";
    std::cout << std::string(100, '-') << "\n";
    
    std::cout << std::left << std::setw(30) << "Create (1x)"
              << std::right << std::setw(15) << format_duration(create_time)
              << std::setw(20) << std::to_string((int)(create_time / total_time * 100)) + "%"
              << "\n";
    
    std::cout << std::left << std::setw(30) << "Add/Array (1M calls)"
              << std::right << std::setw(15) << format_duration(add_time)
              << std::setw(20) << std::to_string((int)(add_time / total_time * 100)) + "%"
              << "\n";
    
    std::cout << std::left << std::setw(30) << "Format (1x)"
              << std::right << std::setw(15) << format_duration(fmt_time)
              << std::setw(20) << std::to_string((int)(fmt_time / total_time * 100)) + "%"
              << "\n";
    
    std::cout << std::string(100, '-') << "\n";
    std::cout << std::left << std::setw(30) << "TOTAL"
              << std::right << std::setw(15) << format_duration(total_time)
              << std::setw(20) << "100%"
              << "\n\n";
    
    std::cout << "Key Metrics:\n";
    std::cout << "  • Total nodes created: " << total_nodes_created << "\n";
    std::cout << "  • Average time per node: " << format_duration(total_time / total_nodes_created) << "\n";
    std::cout << "  • Overall throughput: " << std::to_string((int)(total_nodes_created / total_time / 1e6)) << "M nodes/sec\n";
    std::cout << "  • Output size: " << output.size() / 1024 / 1024 << " MB\n\n";
    
    std::cout << std::string(100, '=') << "\n";
    std::cout << "✅ Benchmark completed!\n";
    std::cout << std::string(100, '=') << "\n\n";
    
    return 0;
}
