#include "module_log_v8.2.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace module_log;

void print_separator() {
    std::cout << std::string(100, '=') << "\n";
}

void print_test_header(int num, const std::string& api_name, const std::string& description) {
    std::cout << "\n" << std::string(100, '=') << "\n";
    std::cout << "TEST #" << std::setw(2) << num << " | " << api_name << " - " << description << "\n";
    std::cout << std::string(100, '=') << "\n";
}

void print_input(const std::string& code) {
    std::cout << "\n📝 INPUT:\n";
    std::cout << code << "\n";
}

void print_output(const std::string& output) {
    std::cout << "\n📤 OUTPUT:\n";
    std::cout << output;
    if (!output.empty() && output.back() != '\n') {
        std::cout << "\n";
    }
}

int main() {
    std::cout << "\n";
    print_separator();
    std::cout << "LogTree V8.2 - COMPLETE API COVERAGE TEST\n";
    std::cout << "Testing ALL CREATE and ADD APIs with all overloads\n";
    print_separator();
    
    int test_num = 0;
    
    // ========================================================================
    // PART 1: CREATE API (6 overloads)
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 1: CREATE API - All 6 Overloads\n";
    print_separator();
    
    // CREATE #1: create(name, raw, width)
    test_num++;
    print_test_header(test_num, "CREATE", "create(name, raw, width)");
    print_input(R"(auto tree = LogTree::create("System", 0x100, 9);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("System", 0x100, 9);
        std::string output = tree.format();
        print_output(output);
    }
    
    // CREATE #2: create(name)
    test_num++;
    print_test_header(test_num, "CREATE", "create(name) - simple root");
    print_input(R"(auto tree = LogTree::create("Root");
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        std::string output = tree.format();
        print_output(output);
    }
    
    // CREATE #3: create(name, raw, width, decoded_string)
    test_num++;
    print_test_header(test_num, "CREATE", "create(name, raw, width, decoded_string)");
    print_input(R"(auto tree = LogTree::create("Status", 0x01, 8, "OK");
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Status", 0x01, 8, "OK");
        std::string output = tree.format();
        print_output(output);
    }
    
    // CREATE #4: create(name, raw, rwidth, dec, dwidth)
    test_num++;
    print_test_header(test_num, "CREATE", "create(name, raw, rwidth, dec, dwidth) - numeric decode");
    print_input(R"(auto tree = LogTree::create("Sensor", 0xFF, 8, 255, 8);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Sensor", 0xFF, 8, 255, 8);
        std::string output = tree.format();
        print_output(output);
    }
    
    // CREATE #5: create(name, raw1, name2, raw2) - dual values
    test_num++;
    print_test_header(test_num, "CREATE", "create(name, raw1, name2, raw2) - dual values");
    print_input(R"(auto tree = LogTree::create("Dual", 0xAB, "Value2", 0xCD);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Dual", 0xAB, "Value2", 0xCD);
        std::string output = tree.format();
        print_output(output);
    }
    
    // CREATE #6: create(name, value128)
    test_num++;
    print_test_header(test_num, "CREATE", "create(name, value128) - 128-bit value");
    print_input(R"(unsigned __int128 val128 = 0xFFFFFFFFFFFFFFFFULL;
val128 = (val128 << 64) | 0xFFFFFFFFFFFFFFFFULL;
auto tree = LogTree::create("BigNum", val128);
std::string output = tree.format();)");
    {
        unsigned __int128 val128 = 0xFFFFFFFFFFFFFFFFULL;
        val128 = (val128 << 64) | 0xFFFFFFFFFFFFFFFFULL;
        auto tree = LogTree::create("BigNum", val128);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ========================================================================
    // PART 2: ADD API (5 overloads)
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 2: ADD API - All 5 Overloads\n";
    print_separator();
    
    // ADD #1: add(parent, name)
    test_num++;
    print_test_header(test_num, "ADD", "add(parent, name) - simple node");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Child");
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Child");
        std::string output = tree.format();
        print_output(output);
    }
    
    // ADD #2: add(parent, name, raw, width)
    test_num++;
    print_test_header(test_num, "ADD", "add(parent, name, raw, width)");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Data", 0x42, 8);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Data", 0x42, 8);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ADD #3: add(parent, name, raw, width, decoded_string)
    test_num++;
    print_test_header(test_num, "ADD", "add(parent, name, raw, width, decoded_string)");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Status", 0x01, 8, "Active");
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Status", 0x01, 8, "Active");
        std::string output = tree.format();
        print_output(output);
    }
    
    // ADD #4: add(parent, name, raw, rwidth, dec, dwidth)
    test_num++;
    print_test_header(test_num, "ADD", "add(parent, name, raw, rwidth, dec, dwidth) - numeric decode");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Sensor", 0x64, 8, 100, 8);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Sensor", 0x64, 8, 100, 8);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ADD #5: add(parent, name, raw1, name2, raw2) - dual values
    test_num++;
    print_test_header(test_num, "ADD", "add(parent, name, raw1, name2, raw2) - dual values");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Pair", 0xAB, "Second", 0xCD);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Pair", 0xAB, "Second", 0xCD);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ========================================================================
    // PART 3: ARRAY APIs
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 3: ARRAY APIs - add_array_element & add_array_element_auto\n";
    print_separator();
    
    // ADD_ARRAY_ELEMENT #1: Manual index
    test_num++;
    print_test_header(test_num, "ADD_ARRAY_ELEMENT", "add_array_element(parent, array_name, index, float, hex, width)");
    print_input(R"(auto tree = LogTree::create("Data");
tree.add_array_element("Data", "samples", 0, 1.0f, 0x01, 8);
tree.add_array_element("Data", "samples", 1, 2.0f, 0x02, 8);
tree.add_array_element("Data", "samples", 2, 3.0f, 0x03, 8);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Data");
        tree.add_array_element("Data", "samples", 0, 1.0f, 0x01, 8);
        tree.add_array_element("Data", "samples", 1, 2.0f, 0x02, 8);
        tree.add_array_element("Data", "samples", 2, 3.0f, 0x03, 8);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ADD_ARRAY_ELEMENT_AUTO #2: Auto index
    test_num++;
    print_test_header(test_num, "ADD_ARRAY_ELEMENT_AUTO", "add_array_element_auto(parent, array_name, float, hex, width)");
    print_input(R"(auto tree = LogTree::create("Metrics");
tree.add_array_element_auto("Metrics", "cpu", 1.5f, 0x3F00, 16);
tree.add_array_element_auto("Metrics", "cpu", 2.5f, 0x4000, 16);
tree.add_array_element_auto("Metrics", "cpu", 3.5f, 0x4060, 16);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Metrics");
        tree.add_array_element_auto("Metrics", "cpu", 1.5f, 0x3F00, 16);
        tree.add_array_element_auto("Metrics", "cpu", 2.5f, 0x4000, 16);
        tree.add_array_element_auto("Metrics", "cpu", 3.5f, 0x4060, 16);
        std::string output = tree.format();
        print_output(output);
    }
    
    // Multiple arrays with auto index
    test_num++;
    print_test_header(test_num, "ADD_ARRAY_ELEMENT_AUTO", "Multiple arrays with auto indexing");
    print_input(R"(auto tree = LogTree::create("System");
tree.add_array_element_auto("System", "temp", 25.5f, 0x41CB0000, 32);
tree.add_array_element_auto("System", "temp", 26.0f, 0x41D00000, 32);
tree.add_array_element_auto("System", "voltage", 3.3f, 0x40533333, 32);
tree.add_array_element_auto("System", "voltage", 3.4f, 0x4059999A, 32);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("System");
        tree.add_array_element_auto("System", "temp", 25.5f, 0x41CB0000, 32);
        tree.add_array_element_auto("System", "temp", 26.0f, 0x41D00000, 32);
        tree.add_array_element_auto("System", "voltage", 3.3f, 0x40533333, 32);
        tree.add_array_element_auto("System", "voltage", 3.4f, 0x4059999A, 32);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ========================================================================
    // PART 4: DUAL ARRAY API
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 4: DUAL ARRAY API - add_dual_array_elements\n";
    print_separator();
    
    // ADD_DUAL_ARRAY_ELEMENTS #1: Basic usage
    test_num++;
    print_test_header(test_num, "ADD_DUAL_ARRAY_ELEMENTS", "add_dual_array_elements(parent, name1, f1, h1, w1, name2, f2, h2, w2)");
    print_input(R"(auto tree = LogTree::create("Network");
tree.add_dual_array_elements(
    "Network",
    "input", 12.5f, 0xABCD, 16,
    "output", 25.0f, 0x1234, 16
);
tree.add_dual_array_elements(
    "Network",
    "input", 15.0f, 0xDCBA, 16,
    "output", 30.0f, 0x4321, 16
);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Network");
        tree.add_dual_array_elements(
            "Network",
            "input", 12.5f, 0xABCD, 16,
            "output", 25.0f, 0x1234, 16
        );
        tree.add_dual_array_elements(
            "Network",
            "input", 15.0f, 0xDCBA, 16,
            "output", 30.0f, 0x4321, 16
        );
        std::string output = tree.format();
        print_output(output);
    }
    
    // ADD_DUAL_ARRAY_ELEMENTS #2: Different bit widths
    test_num++;
    print_test_header(test_num, "ADD_DUAL_ARRAY_ELEMENTS", "Dual array with different bit widths");
    print_input(R"(auto tree = LogTree::create("IO");
tree.add_dual_array_elements(
    "IO",
    "rx", 1.0f, 0xFF, 8,
    "tx", 2.0f, 0xFFFF, 16
);
tree.add_dual_array_elements(
    "IO",
    "rx", 3.0f, 0xAA, 8,
    "tx", 4.0f, 0xAAAA, 16
);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("IO");
        tree.add_dual_array_elements(
            "IO",
            "rx", 1.0f, 0xFF, 8,
            "tx", 2.0f, 0xFFFF, 16
        );
        tree.add_dual_array_elements(
            "IO",
            "rx", 3.0f, 0xAA, 8,
            "tx", 4.0f, 0xAAAA, 16
        );
        std::string output = tree.format();
        print_output(output);
    }
    
    // ========================================================================
    // PART 5: COMBINED USAGE
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 5: COMBINED USAGE - Mixed APIs\n";
    print_separator();
    
    // Combined test
    test_num++;
    print_test_header(test_num, "COMBINED", "Mix of CREATE, ADD, and ARRAY APIs");
    print_input(R"(auto tree = LogTree::create("Root", 0x01, 8);
tree.add("Root", "Simple");
tree.add("Root", "WithValue", 0x42, 8);
tree.add("Root", "Decoded", 0x01, 8, "OK");
tree.add_array_element_auto("Root", "metrics", 1.5f, 0x3F00, 16);
tree.add_array_element_auto("Root", "metrics", 2.5f, 0x4000, 16);
tree.add_dual_array_elements(
    "Root",
    "in", 10.0f, 0xAA, 8,
    "out", 20.0f, 0xBB, 8
);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root", 0x01, 8);
        tree.add("Root", "Simple");
        tree.add("Root", "WithValue", 0x42, 8);
        tree.add("Root", "Decoded", 0x01, 8, "OK");
        tree.add_array_element_auto("Root", "metrics", 1.5f, 0x3F00, 16);
        tree.add_array_element_auto("Root", "metrics", 2.5f, 0x4000, 16);
        tree.add_dual_array_elements(
            "Root",
            "in", 10.0f, 0xAA, 8,
            "out", 20.0f, 0xBB, 8
        );
        std::string output = tree.format();
        print_output(output);
    }
    
    // Chain add test
    test_num++;
    print_test_header(test_num, "CHAIN ADD", "Chaining multiple add() calls");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "A", 0x01, 8)
    .add("Root", "B", 0x02, 8)
    .add("Root", "C", 0x03, 8)
    .add("C", "D", 0x04, 8);
std::string output = tree.format();)");
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "A", 0x01, 8)
            .add("Root", "B", 0x02, 8)
            .add("Root", "C", 0x03, 8)
            .add("C", "D", 0x04, 8);
        std::string output = tree.format();
        print_output(output);
    }
    
    // ========================================================================
    // PART 6: SUBTREE TESTS - format_from()
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 6: SUBTREE TESTS - format_from() API\n";
    print_separator();
    
    // Simple subtree
    test_num++;
    print_test_header(test_num, "FORMAT_FROM", "Simple subtree from 'Subtree' node");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Subtree");
tree.add("Subtree", "Node1");
tree.add("Subtree", "Node2");
std::string output = tree.format_from("Subtree");)" );
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Subtree");
        tree.add("Subtree", "Node1");
        tree.add("Subtree", "Node2");
        std::string output = tree.format_from("Subtree");
        print_output(output);
    }
    
    // Deep nested subtree
    test_num++;
    print_test_header(test_num, "FORMAT_FROM", "Deep nested subtree (4 levels)");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Level1_A")
    .add("Level1_A", "Level2_A1")
    .add("Level2_A1", "Level3_A1a")
    .add("Level3_A1a", "Level4_A1a1");
tree.add("Level1_A", "Level2_A2")
    .add("Level2_A2", "Level3_A2a");
std::string output = tree.format_from("Level1_A");)" );
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Level1_A")
            .add("Level1_A", "Level2_A1")
            .add("Level2_A1", "Level3_A1a")
            .add("Level3_A1a", "Level4_A1a1");
        tree.add("Level1_A", "Level2_A2")
            .add("Level2_A2", "Level3_A2a");
        std::string output = tree.format_from("Level1_A");
        print_output(output);
    }
    
    // Wide tree subtree
    test_num++;
    print_test_header(test_num, "FORMAT_FROM", "Wide tree with many siblings");
    print_input(R"(auto tree = LogTree::create("Parent");
tree.add("Parent", "Child1");
tree.add("Parent", "Child2");
tree.add("Parent", "Child3");
tree.add("Parent", "Child4");
tree.add("Parent", "Child5");
tree.add("Child3", "Grandchild3a");
tree.add("Child3", "Grandchild3b");
std::string output = tree.format_from("Child3");)" );
    {
        auto tree = LogTree::create("Parent");
        tree.add("Parent", "Child1");
        tree.add("Parent", "Child2");
        tree.add("Parent", "Child3");
        tree.add("Parent", "Child4");
        tree.add("Parent", "Child5");
        tree.add("Child3", "Grandchild3a");
        tree.add("Child3", "Grandchild3b");
        std::string output = tree.format_from("Child3");
        print_output(output);
    }
    
    // Subtree with arrays
    test_num++;
    print_test_header(test_num, "FORMAT_FROM", "Subtree containing arrays and dual arrays");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "Config");
tree.add_array_element_auto("Config", "settings", 1.0f, 0x01, 8);
tree.add_array_element_auto("Config", "settings", 2.0f, 0x02, 8);
tree.add("Config", "Advanced");
tree.add_dual_array_elements(
    "Advanced",
    "input", 10.0f, 0xAA, 8,
    "output", 20.0f, 0xBB, 8
);
std::string output = tree.format_from("Config");)" );
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "Config");
        tree.add_array_element_auto("Config", "settings", 1.0f, 0x01, 8);
        tree.add_array_element_auto("Config", "settings", 2.0f, 0x02, 8);
        tree.add("Config", "Advanced");
        tree.add_dual_array_elements(
            "Advanced",
            "input", 10.0f, 0xAA, 8,
            "output", 20.0f, 0xBB, 8
        );
        std::string output = tree.format_from("Config");
        print_output(output);
    }
    
    // Edge case: empty parent subtree
    test_num++;
    print_test_header(test_num, "FORMAT_FROM", "Edge case - empty parent (no children)");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "EmptyParent");
tree.add("Root", "Other");
std::string output = tree.format_from("EmptyParent");)" );
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "EmptyParent");
        tree.add("Root", "Other");
        std::string output = tree.format_from("EmptyParent");
        print_output(output);
    }
    
    // ========================================================================
    // PART 7: COMPLEX REAL-WORLD SCENARIOS
    // ========================================================================
    
    std::cout << "\n\n";
    print_separator();
    std::cout << "PART 7: COMPLEX REAL-WORLD SCENARIOS\n";
    print_separator();
    
    // Network monitoring system
    test_num++;
    print_test_header(test_num, "COMPLEX", "Network monitoring system with interfaces, protocols, statistics");
    print_input(R"(auto tree = LogTree::create("NetworkMonitor", 0x01, 8, "Active");
tree.add("NetworkMonitor", "Interfaces");
tree.add_dual_array_elements("Interfaces", "eth0_rx", 1250.5f, 0xABCD, 16, "eth0_tx", 980.3f, 0xDCBA, 16);
tree.add_dual_array_elements("Interfaces", "wlan0_rx", 450.8f, 0x1234, 16, "wlan0_tx", 380.5f, 0x4321, 16);
tree.add("NetworkMonitor", "Protocols");
tree.add("Protocols", "TCP", 0x06, 8, "Connected");
tree.add("Protocols", "UDP", 0x11, 8, "Active");
tree.add("NetworkMonitor", "Statistics");
tree.add_array_element_auto("Statistics", "packets_sent", 15000.0f, 0x3A98, 16);
tree.add_array_element_auto("Statistics", "packets_recv", 14800.0f, 0x39D0, 16);
std::string output = tree.format();)" );
    {
        auto tree = LogTree::create("NetworkMonitor", 0x01, 8, "Active");
        tree.add("NetworkMonitor", "Interfaces");
        tree.add_dual_array_elements("Interfaces", "eth0_rx", 1250.5f, 0xABCD, 16, "eth0_tx", 980.3f, 0xDCBA, 16);
        tree.add_dual_array_elements("Interfaces", "wlan0_rx", 450.8f, 0x1234, 16, "wlan0_tx", 380.5f, 0x4321, 16);
        tree.add("NetworkMonitor", "Protocols");
        tree.add("Protocols", "TCP", 0x06, 8, "Connected");
        tree.add("Protocols", "UDP", 0x11, 8, "Active");
        tree.add("NetworkMonitor", "Statistics");
        tree.add_array_element_auto("Statistics", "packets_sent", 15000.0f, 0x3A98, 16);
        tree.add_array_element_auto("Statistics", "packets_recv", 14800.0f, 0x39D0, 16);
        std::string output = tree.format();
        print_output(output);
    }
    
    // CPU architecture
    test_num++;
    print_test_header(test_num, "COMPLEX", "CPU architecture with cores, registers, cache hierarchy");
    print_input(R"(auto tree = LogTree::create("CPU", 0x00, 8, "Intel");
tree.add("CPU", "Core0");
tree.add("Core0", "Registers");
tree.add("Registers", "EAX", 0xFFFFFFFF, 32);
tree.add("Registers", "EBX", 0x00000000, 32);
tree.add("Core0", "Cache");
tree.add("Cache", "L1_Data", 0x8000, 16, "32KB");
tree.add("Cache", "L2", 0x40000, 19, "256KB");
tree.add("Cache", "L3", 0x800000, 24, "8MB");
tree.add("Core0", "Pipeline");
tree.add_array_element_auto("Pipeline", "stage_latency", 0.5f, 0x3F00, 16);
tree.add_array_element_auto("Pipeline", "stage_latency", 0.8f, 0x3F4C, 16);
std::string output = tree.format();)" );
    {
        auto tree = LogTree::create("CPU", 0x00, 8, "Intel");
        tree.add("CPU", "Core0");
        tree.add("Core0", "Registers");
        tree.add("Registers", "EAX", 0xFFFFFFFF, 32);
        tree.add("Registers", "EBX", 0x00000000, 32);
        tree.add("Core0", "Cache");
        tree.add("Cache", "L1_Data", 0x8000, 16, "32KB");
        tree.add("Cache", "L2", 0x40000, 19, "256KB");
        tree.add("Cache", "L3", 0x800000, 24, "8MB");
        tree.add("Core0", "Pipeline");
        tree.add_array_element_auto("Pipeline", "stage_latency", 0.5f, 0x3F00, 16);
        tree.add_array_element_auto("Pipeline", "stage_latency", 0.8f, 0x3F4C, 16);
        std::string output = tree.format();
        print_output(output);
    }
    
    // File system tree
    test_num++;
    print_test_header(test_num, "COMPLEX", "File system tree with directories and files");
    print_input(R"(auto tree = LogTree::create("/");
tree.add("/", "home");
tree.add("home", "user");
tree.add("user", "Documents");
tree.add("Documents", "report.pdf", 0x01, 8, "File");
tree.add("Documents", "data.csv", 0x02, 8, "File");
tree.add("/", "etc");
tree.add("etc", "config.ini", 0x05, 8, "Config");
tree.add("/", "var");
tree.add("var", "log");
tree.add("log", "syslog", 0x07, 8, "Log");
tree.add_array_element_auto("log", "entries", 100.0f, 0x64, 8);
std::string output = tree.format();)" );
    {
        auto tree = LogTree::create("/");
        tree.add("/", "home");
        tree.add("home", "user");
        tree.add("user", "Documents");
        tree.add("Documents", "report.pdf", 0x01, 8, "File");
        tree.add("Documents", "data.csv", 0x02, 8, "File");
        tree.add("/", "etc");
        tree.add("etc", "config.ini", 0x05, 8, "Config");
        tree.add("/", "var");
        tree.add("var", "log");
        tree.add("log", "syslog", 0x07, 8, "Log");
        tree.add_array_element_auto("log", "entries", 100.0f, 0x64, 8);
        std::string output = tree.format();
        print_output(output);
    }
    
    // Edge cases
    test_num++;
    print_test_header(test_num, "COMPLEX", "Edge cases - empty parent, single child, deep chain");
    print_input(R"(auto tree = LogTree::create("Root");
tree.add("Root", "EmptyParent");
tree.add("Root", "SingleChild", 0x42, 8);
tree.add("Root", "A");
tree.add("A", "B");
tree.add("B", "C");
tree.add("C", "D");
tree.add("D", "E");
std::string output = tree.format();)" );
    {
        auto tree = LogTree::create("Root");
        tree.add("Root", "EmptyParent");
        tree.add("Root", "SingleChild", 0x42, 8);
        tree.add("Root", "A");
        tree.add("A", "B");
        tree.add("B", "C");
        tree.add("C", "D");
        tree.add("D", "E");
        std::string output = tree.format();
        print_output(output);
    }
    
    print_separator();
    std::cout << "✅ TOTAL API TESTS: " << test_num << "\n";
    print_separator();
    std::cout << "\nAPI Coverage Summary:\n";
    std::cout << "  ✓ CREATE API:              6 overloads tested\n";
    std::cout << "  ✓ ADD API:                 5 overloads tested\n";
    std::cout << "  ✓ ADD_ARRAY_ELEMENT:       Manual index tested\n";
    std::cout << "  ✓ ADD_ARRAY_ELEMENT_AUTO:  Auto index + multiple arrays tested\n";
    std::cout << "  ✓ ADD_DUAL_ARRAY_ELEMENTS: Basic + different widths tested\n";
    std::cout << "  ✓ FORMAT_FROM:             Simple + deep + wide + arrays + edge cases\n";
    std::cout << "  ✓ COMPLEX SCENARIOS:       Network + CPU + FileSystem + EdgeCases\n";
    std::cout << "\nAll APIs covered with complete input/output display!\n\n";
    
    return 0;
}
