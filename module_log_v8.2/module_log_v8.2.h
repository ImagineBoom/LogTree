#pragma once
#include <vector>
#include <string_view>
#include <string>
#include <cstdint>
#include <cstring>
#include <memory>
#include <absl/container/flat_hash_map.h>  // ✨ Abseil flat_hash_map for better performance

namespace module_log {

// ✅ 枚举类型：节点类型 / Enum type: Node type
enum class NodeType : uint8_t {
    Standard = 0,
    Array = 1,
    DualArray = 2
};

// ✅ 工具函数：FP32 浮点数与 bit pattern 转换 / Utility function: FP32 float to bit pattern conversion
inline uint32_t float_to_bits(float f) {
    union { float f; uint32_t u; } converter;
    converter.f = f;
    return converter.u;
}

inline float bits_to_float(uint32_t u) {
    union { uint32_t u; float f; } converter;
    converter.u = u;
    return converter.f;
}

class LogTree {
    
    // ✨ V8.1 极致优化：所有字符串都用固定缓冲区 / Ultra optimization: All strings use fixed buffers
    struct alignas(64) Node {
        // ===== 基本信息 / Basic Information =====
        char name_buf[48];          // ✨ 固定大小名称缓冲区（替代std::string）/ Fixed name buffer
        uint8_t depth;              // 深度 / Depth
        NodeType node_type;         // ✅ 类型：Standard, Array, DualArray
        bool has_children;          // ✨ 是否有子节点（避免 hash 查找）/ Has children flag
        uint32_t parent_idx;        // ✨ 父节点索引（避免 add 时 hash 查找）/ Parent index
        uint64_t insert_order;      // 插入顺序 / Insertion order
        
        // ===== 第一组数据（主数据）/ First group data (main data) =====
        uint64_t raw_value;         // 原始值 / Raw value
        int bit_width;              // 位宽 / Bit width
        uint32_t float_bits;        // FP32 浮点数的 bit pattern / FP32 float bit pattern
        
        // ===== 第二组数据（仅双数组使用）/ Second group data (dual array only) =====
        uint64_t raw_value2;        // 第二个数组的原始值 / Second array raw value
        int bit_width2;             // 第二个数组的位宽 / Second array bit width
        uint32_t float2_bits;       // FP32 浮点数的 bit pattern / FP32 float bit pattern
        char array2_name_buf[48];   // ✨ 固定大小缓冲区 / Fixed buffer
        
        // ===== 第三组数据（仅 128 位节点使用）/ Third group data (128-bit node only) =====
        unsigned __int128 value128;   // 128 位无符号整数值 / 128-bit unsigned integer value
        
        // ===== 解码信息（可选）/ Decoding information (optional) =====
        uint64_t decoded_value;     // 解码值（0 表示无）/ Decoded value (0 means none)
        int decoded_bit_width;      // 解码位宽 / Decoded bit width
        char decoded_str_buf[48];   // ✨ 固定大小缓冲区 / Fixed buffer
        
        Node() : name_buf{}, depth(0), node_type(NodeType::Standard),
                 has_children(false), parent_idx(0), insert_order(0),
                 raw_value(0), bit_width(0), float_bits(0),
                 raw_value2(0), bit_width2(0), float2_bits(0),
                 array2_name_buf{},
                 value128(0),
                 decoded_value(0), decoded_bit_width(0),
                 decoded_str_buf{} {
            name_buf[0] = '\0';
            array2_name_buf[0] = '\0';
            decoded_str_buf[0] = '\0';
        }
    };
    
    // V8.1: 极致优化 - 连续存储 + 预分配 / Ultimate optimization - contiguous storage + pre-allocation
    std::vector<Node> nodes;
    absl::flat_hash_map<std::string, size_t> name_to_index;              // ✨ Abseil flat_hash_map (2-3x faster)
    absl::flat_hash_map<std::string, std::vector<size_t>> children;      // ✨ Abseil flat_hash_map
    absl::flat_hash_map<std::string, size_t> array_counters;             // ✨ Abseil flat_hash_map
    
    // ✨ OPTIMIZATION: Fixed-size string buffer for root_name (avoids heap allocation)
    char root_name_buf[48];
    std::string_view root_name;  // Points to root_name_buf
    std::string root_name_str;   // ✨ Cached std::string for map key (avoids repeated construction)
    
    size_t root_index = 0;          // ✨ Cache root index to avoid hash lookups
    mutable std::vector<size_t>* cached_root_children = nullptr;  // ✨ Cache root children pointer (mutable for add)
    uint64_t next_order = 0;
    
    // ✨ NEW: Parent name -> index cache for fast lookup (avoid linear scan)
    mutable absl::flat_hash_map<std::string, size_t> parent_cache_map;
    
    static constexpr size_t INITIAL_CAPACITY = 1200000;  // ✨ Pre-allocate for ~1M nodes
    static constexpr uint8_t MAX_DEPTH = 64;
    
    // ✅ Helper functions for fixed-size buffers
    inline static void set_node_name(Node& node, std::string_view name) {
        size_t len = std::min(name.size(), sizeof(node.name_buf) - 1);
        std::memcpy(node.name_buf, name.data(), len);
        node.name_buf[len] = '\0';
    }
    
    inline static const char* get_node_name(const Node& node) {
        return node.name_buf;
    }
    
    inline static void set_decoded_str(Node& node, std::string_view str) {
        size_t len = std::min(str.size(), sizeof(node.decoded_str_buf) - 1);
        std::memcpy(node.decoded_str_buf, str.data(), len);
        node.decoded_str_buf[len] = '\0';
    }
    
    inline static const char* get_decoded_str(const Node& node) {
        return node.decoded_str_buf;
    }
    
    inline static bool has_decoded_str(const Node& node) {
        return node.decoded_str_buf[0] != '\0';
    }
    
    inline static void set_array2_name(Node& node, std::string_view name) {
        size_t len = std::min(name.size(), sizeof(node.array2_name_buf) - 1);
        std::memcpy(node.array2_name_buf, name.data(), len);
        node.array2_name_buf[len] = '\0';
    }
    
    inline static const char* get_array2_name(const Node& node) {
        return node.array2_name_buf;
    }
    
    // ✅ 十六进制缓冲区大小：32 字符 + null terminator / Hex buffer size: 32 chars + null terminator
    static constexpr size_t HEX_BUFFER_SIZE = 33;
    
    // ✨ 极简父节点缓存（内部优化，用户无感知）/ Minimal parent cache (internal optimization, transparent to users)
    mutable std::string cached_parent_name;  // 缓存的父节点名称 / Cached parent name
    mutable size_t cached_parent_idx = 0;        // 缓存的父节点索引 / Cached parent index
    mutable uint8_t cached_parent_depth = 0;     // ✨ 缓存的父节点深度 / Cached parent depth
    mutable bool parent_cache_valid = false;         // 缓存是否有效 / Cache validity flag
    

public:
    // ===== Create 接口 / Create Interface =====
    
    /**
     * @brief 创建根节点（带原始值和位宽）/ Create root node (with raw value and bit width)
     * @param name 根节点名称 / Root node name
     * @param raw_value 原始值 / Raw value
     * @param raw_bit_width 原始值的位宽 / Bit width of raw value
     * @return LogTree 新创建的树对象 / Newly created tree object
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("System", 0x100, 9);
     * 
     * // Output:
     * // [System]: raw=0x100 (d:256/b:1'00000000)
     * ```
     */
    [[nodiscard]] static LogTree create(std::string_view name, uint64_t raw_value, int raw_bit_width) {
        LogTree t(name);
        t.create_root_node(raw_value, raw_bit_width);
        return t;
    }
    
    /**
     * @brief 创建根节点（简化版，默认值为 0）/ Create root node (simplified version, default value is 0)
     * @param name 根节点名称 / Root node name
     * @return LogTree 新创建的树对象 / Newly created tree object
     * @note 默认 raw=0, width=0，适用于仅需要结构的场景 / Default raw=0, width=0, suitable for structure-only scenarios
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Root");
     * 
     * // Output:
     * // [Root]
     * ```
     */
    [[nodiscard]] static LogTree create(std::string_view name) {
        LogTree t(name);
        t.create_root_node(0, 0);
        return t;
    }
    
    /**
     * @brief 创建根节点（带字符串解码）/ Create root node (with string decoding)
     * @param name 根节点名称 / Root node name
     * @param raw_value 原始值 / Raw value
     * @param raw_bit_width 原始值的位宽 / Bit width of raw value
     * @param decoded_str 解码后的字符串 / Decoded string
     * @return LogTree 新创建的树对象 / Newly created tree object
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Status", 0x41, 8, "OK");
     * 
     * // Output:
     * // [Status]: raw=0x41 (d:65/b:01000001) → decoded=OK
     * ```
     */
    [[nodiscard]] static LogTree create(std::string_view name, uint64_t raw_value, int raw_bit_width, std::string_view decoded_str) {
        LogTree t(name);
        size_t idx = t.create_root_node(raw_value, raw_bit_width);
        auto& node = t.nodes[idx];
        set_decoded_str(node, decoded_str);  // ✨ Use fixed buffer
        return t;
    }
    
    /**
     * @brief 创建根节点（带数值解码）/ Create root node (with numeric decoding)
     * @param name 根节点名称 / Root node name
     * @param raw_value 原始值 / Raw value
     * @param raw_bit_width 原始值位宽 / Raw value bit width
     * @param decoded_value 解码后的数值 / Decoded numeric value
     * @param decoded_bit_width 解码值位宽 / Decoded value bit width
     * @return LogTree 新创建的树对象 / Newly created tree object
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Sensor", 0x64, 8, 100, 7);
     * 
     * // Output:
     * // [Sensor]: raw=0x64 (d:100/b:01100100) → decoded=0x64 (d:100/b:1100100)
     * ```
     */
    [[nodiscard]] static LogTree create(std::string_view name, uint64_t raw_value, int raw_bit_width, uint64_t decoded_value, int decoded_bit_width) {
        LogTree t(name);
        size_t idx = t.create_root_node(raw_value, raw_bit_width);
        auto& node = t.nodes[idx];
        node.decoded_value = decoded_value;
        node.decoded_bit_width = decoded_bit_width;
        return t;
    }
    
    /**
     * @brief 创建根节点（双值模式：raw_value 作为主节点，decoded_name+decoded_value 作为解码值）/ Create root node (dual value mode: raw_value as main node, decoded_name+decoded_value as decoded value)
     * @param name 节点名称 / Node name
     * @param raw_value 原始值 / Raw value
     * @param decoded_name 解码值名称 / Decoded value name
     * @param decoded_value 解码后的数值 / Decoded numeric value
     * @return LogTree 新创建的树对象 / Newly created tree object
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Main", 0x10, "Decoded", 0x20);
     * 
     * // Output:
     * // [Main]: raw=0x10 (d:16/b:10000) → Decoded=0x20 (d:32/b:100000)
     * ```
     */
    [[nodiscard]] static LogTree create(std::string_view name, uint64_t raw_value, std::string_view decoded_name, uint64_t decoded_value) {
        LogTree t(name);
        
        // ✅ 自动计算最小 bit_width / Auto-calculate minimum bit_width
        int raw_width = raw_value == 0 ? 1 : 64 - __builtin_clzll(raw_value);
        int decoded_width = decoded_value == 0 ? 1 : 64 - __builtin_clzll(decoded_value);
        
        size_t idx = t.create_root_node(raw_value, raw_width);
        auto& node = t.nodes[idx];
        node.decoded_value = decoded_value;
        node.decoded_bit_width = decoded_width;
        set_decoded_str(node, decoded_name);  // ✨ Store decoded_name in fixed buffer
        return t;
    }
    
    /**
     * @brief 创建根节点（128 位无符号整数）/ Create root node (128-bit unsigned integer)
     * @param name 根节点名称 / Root node name
     * @param value128 128 位无符号整数值 / 128-bit unsigned integer value
     * @return LogTree 新创建的树对象 / Newly created tree object
     * @note 只显示十六进制和二进制格式，不显示十进制 / Only displays hexadecimal and binary format, no decimal
     * 
     * @example
     * ```cpp
     * unsigned __int128 big_val = 0;
     * big_val = ((unsigned __int128)0x1234567890ABCDEFULL << 64) | 0xFEDCBA0987654321ULL;
     * auto tree = LogTree::create("BigInt", big_val);
     * 
     * // Output:
     * // [BigInt]: 0x1234567890ABCDEFFEDCBA0987654321 
     * //           (b:00010010'00110100'...01000011'00100001)
     * ```
     */
    [[nodiscard]] static LogTree create(std::string_view name, unsigned __int128 value128) {
        LogTree t(name);
        size_t idx = t.create_root_node(0, 128);  // bit_width=128 表示 128 位值 / bit_width=128 indicates 128-bit value
        auto& node = t.nodes[idx];
        
        // ✅ 直接存储 128 位无符号整数值 / Directly store 128-bit unsigned integer value
        node.value128 = value128;
        
        return t;
    }

private:
    explicit LogTree(std::string_view name) {
        // ✨ OPTIMIZATION: Store root name in fixed buffer
        size_t len = std::min(name.size(), sizeof(root_name_buf) - 1);
        std::memcpy(root_name_buf, name.data(), len);
        root_name_buf[len] = '\0';
        root_name = std::string_view(root_name_buf, len);
        root_name_str = std::string(root_name);  // ✨ Cache for map key
        
        nodes.reserve(INITIAL_CAPACITY);
        // ✨ OPTIMIZED: Reserve enough for all nodes (every node goes into name_to_index)
        name_to_index.reserve(INITIAL_CAPACITY);
        children.reserve(INITIAL_CAPACITY / 10);
        parent_cache_map.reserve(INITIAL_CAPACITY / 10);  // ✨ NEW: Reserve parent cache
        // ✨ Pre-create and reserve root's children vector (avoids rehashing)
        children[root_name_str].reserve(INITIAL_CAPACITY / 2);
        // ✨ Cache pointer to root's children vector
        cached_root_children = const_cast<std::vector<size_t>*>(&children[root_name_str]);
    }
    
    // ✨ 提取公共函数：创建根节点 / Extract common function: create root node
    inline size_t create_root_node(uint64_t raw, int width) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        set_node_name(node, root_name);
        node.raw_value = raw;
        node.bit_width = width;
        node.depth = 0;
        node.insert_order = next_order++;
        node.node_type = NodeType::Standard;
        node.parent_idx = 0;  // Root has no parent
        
        root_index = idx;
        return idx;
    }

public:
    // ✨ ULTRA-FAST: 生成数组 key（避免 std::string 分配）/ Ultra-fast: generate array key (avoid std::string allocation)
    inline std::string make_array_key(std::string_view this_array_name, 
                                     std::string_view parent_name) const {
        std::string_view parent_str = parent_name.empty() ? root_name : parent_name;
        
        // ✨ OPTIMIZATION: Small string optimization for short keys
        char buf[128];
        size_t total = parent_str.size() + 1 + this_array_name.size();
        if (total > sizeof(buf) - 1) total = sizeof(buf) - 1;
        
        size_t pos = 0;
        size_t copy1 = std::min(parent_str.size(), total);
        std::memcpy(buf, parent_str.data(), copy1);
        pos += copy1;
        if (pos < total) { buf[pos++] = '|'; }
        if (pos < total) {
            size_t copy2 = std::min(this_array_name.size(), total - pos);
            std::memcpy(buf + pos, this_array_name.data(), copy2);
            pos += copy2;
        }
        buf[pos] = '\0';
        return std::string(buf, pos);
    }
    
    // ===== Add 接口 / Add Interface =====
    
    /**
     * @brief 添加简单节点（无数据，仅结构）/ Add simple node (no data, structure only)
     * @param parent_name 父节点名称 / Parent node name
     * @param this_node_name 当前节点名称 / Current node name
     * @return LogTree& 引用自身，支持链式调用 / Reference to self, supports chain calls
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Root");
     * tree.add("Root", "Child1")
     *     .add("Root", "Child2");
     * 
     * // Output:
     * // [Root]
     * // ├─[Child1]
     * // └─[Child2]
     * ```
     */
    LogTree& add(std::string_view parent_name, std::string_view this_node_name) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        set_node_name(node, this_node_name);
        node.raw_value = 0;
        node.bit_width = 0;
        node.node_type = NodeType::Standard;
        
        add_node_common(node, idx, this_node_name, parent_name);
        return *this;  // ✅ 支持链式调用 / Supports chain calls
    }
    
    /**
     * @brief 添加标准节点（带原始值和位宽）/ Add standard node (with raw value and bit width)
     * @param parent_name 父节点名称 / Parent node name
     * @param this_node_name 当前节点名称 / Current node name
     * @param raw_value 原始值 / Raw value
     * @param raw_bit_width 原始值的位宽 / Bit width of raw value
     * @return LogTree& 引用自身，支持链式调用 / Reference to self, supports chain calls
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("System");
     * tree.add("System", "CPU", 0xFF, 8)
     *     .add("System", "Memory", 0x100, 9);
     * 
     * // Output:
     * // [System]
     * // ├─[CPU]: raw=0xFF (d:255/b:11111111)
     * // └─[Memory]: raw=0x100 (d:256/b:1'00000000)
     * ```
     */
    LogTree& add(std::string_view parent_name, std::string_view this_node_name, uint64_t raw_value, int raw_bit_width) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        set_node_name(node, this_node_name);
        node.raw_value = raw_value;
        node.bit_width = raw_bit_width;
        node.node_type = NodeType::Standard;
        
        add_node_common(node, idx, this_node_name, parent_name);
        return *this;  // ✅ 支持链式调用 / Supports chain calls
    }
    
    /**
     * @brief 添加带字符串解码的节点 / Add node with string decoding
     * @param parent_name 父节点名称 / Parent node name
     * @param this_node_name 当前节点名称 / Current node name
     * @param raw_value 原始值 / Raw value
     * @param raw_bit_width 原始值的位宽 / Bit width of raw value
     * @param decoded_str 解码后的字符串 / Decoded string
     * @return LogTree& 引用自身，支持链式调用 / Reference to self, supports chain calls
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Status");
     * tree.add("Status", "Code", 0x01, 8, "Success");
     * 
     * // Output:
     * // [Status]
     * // └─[Code]: raw=0x1 (d:1/b:00000001) → decoded=Success
     * ```
     */
    LogTree& add(std::string_view parent_name, std::string_view this_node_name, uint64_t raw_value, int raw_bit_width, std::string_view decoded_str) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        set_node_name(node, this_node_name);
        node.raw_value = raw_value;
        node.bit_width = raw_bit_width;
        set_decoded_str(node, decoded_str);  // ✨ Use fixed buffer
        node.node_type = NodeType::Standard;
        
        add_node_common(node, idx, this_node_name, parent_name);
        return *this;  // ✅ 支持链式调用 / Supports chain calls
    }
    
    /**
     * @brief 添加带数值解码的节点 / Add node with numeric decoding
     * @param parent_name 父节点名称 / Parent node name
     * @param this_node_name 当前节点名称 / Current node name
     * @param raw_value 原始值 / Raw value
     * @param raw_bit_width 原始值位宽 / Raw value bit width
     * @param decoded_value 解码后的数值 / Decoded numeric value
     * @param decoded_bit_width 解码值位宽 / Decoded value bit width
     * @return LogTree& 引用自身，支持链式调用 / Reference to self, supports chain calls
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Sensor");
     * tree.add("Sensor", "Temp", 0x64, 8, 100, 7);
     * 
     * // Output:
     * // [Sensor]
     * // └─[Temp]: raw=0x64 (d:100/b:01100100) → decoded=0x64 (d:100/b:1100100)
     * ```
     */
    LogTree& add(std::string_view parent_name, std::string_view this_node_name, uint64_t raw_value, int raw_bit_width, uint64_t decoded_value, int decoded_bit_width) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        set_node_name(node, this_node_name);
        node.raw_value = raw_value;
        node.bit_width = raw_bit_width;
        node.decoded_value = decoded_value;
        node.decoded_bit_width = decoded_bit_width;
        node.node_type = NodeType::Standard;
        
        add_node_common(node, idx, this_node_name, parent_name);
        return *this;  // ✅ 支持链式调用 / Supports chain calls
    }
    
    /**
     * @brief 添加双值节点（raw_value 作为主值，decoded_name+decoded_value 作为解码值）/ Add dual value node (raw_value as main value, decoded_name+decoded_value as decoded value)
     * @param parent_name 父节点名称 / Parent node name
     * @param this_node_name 当前节点名称 / Current node name
     * @param raw_value 原始值 / Raw value
     * @param decoded_name 解码值名称 / Decoded value name
     * @param decoded_value 解码后的数值 / Decoded numeric value
     * @return LogTree& 引用自身，支持链式调用 / Reference to self, supports chain calls
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Sensor");
     * tree.add("Sensor", "Reading", 0x10, "Calibrated", 0x20);
     * 
     * // Output:
     * // [Sensor]
     * // └─[Reading]: raw=0x10 (d:16/b:10000) → Calibrated=0x20 (d:32/b:100000)
     * ```
     */
    LogTree& add(std::string_view parent_name, std::string_view this_node_name, uint64_t raw_value, std::string_view decoded_name, uint64_t decoded_value) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        // ✅ 自动计算最小 bit_width / Auto-calculate minimum bit_width
        int raw_width = raw_value == 0 ? 1 : 64 - __builtin_clzll(raw_value);
        int decoded_width = decoded_value == 0 ? 1 : 64 - __builtin_clzll(decoded_value);
        
        set_node_name(node, this_node_name);
        node.raw_value = raw_value;
        node.bit_width = raw_width;
        node.decoded_value = decoded_value;
        node.decoded_bit_width = decoded_width;
        set_decoded_str(node, decoded_name);  // ✨ Store decoded_name in fixed buffer
        node.node_type = NodeType::Standard;
        
        add_node_common(node, idx, this_node_name, parent_name);
        return *this;  // ✅ 支持链式调用 / Supports chain calls
    }

    // ===== 数组接口 / Array Interface =====
    
    /**
     * @brief 添加数组元素（指定索引）/ Add array element (specified index)
     * @param parent_name 父节点名称 / Parent node name
     * @param this_array_name 当前数组名称 / Current array name
     * @param node_index 节点在数组中的索引 / Node's index in array
     * @param node_float_value 节点的FP32浮点数值 / Node's FP32 float value
     * @param node_hex_value 节点的十六进制值 / Node's hexadecimal value
     * @param node_bit_width 节点二进制位宽 / Node's binary bit width
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Data");
     * tree.add_array_element("Data", "input", 0, 12.5f, 0xABCD, 16);
     * tree.add_array_element("Data", "input", 1, 15.0f, 0xDCBA, 16);
     * 
     * // Output:
     * // [Data]
     * // ├─input[0]=12.5 (0xABCD, b:10101011'11001101)
     * // └─input[1]=15.0 (0xDCBA, b:11011100'10111010)
     * ```
     */
    void add_array_element(std::string_view parent_name, std::string_view this_array_name, size_t node_index, 
                          float node_float_value, uint64_t node_hex_value, int node_bit_width) {
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        // ✨ Generate array name without fmt (e.g., "input[0]")
        char name_buf[64];
        int len = snprintf(name_buf, sizeof(name_buf), "%.*s[%zu]", 
                          (int)this_array_name.size(), this_array_name.data(), node_index);
        set_node_name(node, std::string_view(name_buf, len));
        node.raw_value = node_hex_value;
        node.bit_width = node_bit_width;
        node.float_bits = float_to_bits(node_float_value);  // ✅ 使用工具函数 / Use utility function
        node.node_type = NodeType::Array;
        
        add_node_common(node, idx, this_array_name, parent_name);
    }
    
    /**
     * @brief 添加数组元素（自动索引）/ Add array element (auto index)
     * @param parent_name 父节点名称 / Parent node name
     * @param this_array_name 当前数组名称 / Current array name
     * @param node_float_value 节点的FP32浮点数值 / Node's FP32 float value
     * @param node_hex_value 节点的十六进制值 / Node's hexadecimal value
     * @param node_bit_width 节点二进制位宽 / Node's binary bit width
     * @note 索引从 0 开始自动递增 / Index auto-increments from 0
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Metrics");
     * tree.add_array_element_auto("Metrics", "values", 1.5f, 0x3F00, 16);
     * tree.add_array_element_auto("Metrics", "values", 2.5f, 0x4000, 16);
     * tree.add_array_element_auto("Metrics", "values", 3.5f, 0x4060, 16);
     * 
     * // Output:
     * // [Metrics]
     * // ├─values[0]=1.5 (0x3F00, b:00111111'00000000)
     * // ├─values[1]=2.5 (0x4000, b:01000000'00000000)
     * // └─values[2]=3.5 (0x4060, b:01000000'01100000)
     * ```
     */
    void add_array_element_auto(std::string_view parent_name, std::string_view this_array_name,
                               float node_float_value, uint64_t node_hex_value, int node_bit_width) {
        std::string key = make_array_key(this_array_name, parent_name);
        size_t node_index = array_counters[key]++;
        
        add_array_element(parent_name, this_array_name, node_index, node_float_value, node_hex_value, node_bit_width);
    }
    
    // ===== 双数组接口 / Dual Array Interface =====
    
    /**
     * @brief 添加双数组元素（同步的输入输出对）/ Add dual array elements (synchronized input-output pairs)
     * @param parent_name 父节点名称 / Parent node name
     * @param this_array1_name 第一个数组名称（如 input）/ First array name (e.g., input)
     * @param node1_float_value 第一个节点的FP32浮点数值 / First node's FP32 float value
     * @param node1_hex_value 第一个节点的十六进制值 / First node's hexadecimal value
     * @param node1_bit_width 第一个节点的二进制位宽 / First node's binary bit width
     * @param this_array2_name 第二个数组名称（如 output）/ Second array name (e.g., output)
     * @param node2_float_value 第二个节点的FP32浮点数值 / Second node's FP32 float value
     * @param node2_hex_value 第二个节点的十六进制值 / Second node's hexadecimal value
     * @param node2_bit_width 第二个节点的二进制位宽 / Second node's binary bit width
     * @note 两个数组的索引会自动同步递增 / Both array indices auto-increment synchronously
     * 
     * @example
     * ```cpp
     * auto tree = LogTree::create("Network");
     * tree.add_dual_array_elements(
     *     "Network",
     *     "input", 12.5f, 0xABCD, 16,
     *     "output", 25.0f, 0x1234, 16
     * );
     * tree.add_dual_array_elements(
     *     "Network",
     *     "input", 15.0f, 0xDCBA, 16,
     *     "output", 30.0f, 0x4321, 16
     * );
     * 
     * // Output:
     * // [Network]
     * // ├─input[0]=12.5 (0xABCD, b:10101011'11001101), output[0]=25.0 (0x1234, b:00010010'00110100)
     * // └─input[1]=15.0 (0xDCBA, b:11011100'10111010), output[1]=30.0 (0x4321, b:01000011'00100001)
     * ```
     */
    void add_dual_array_elements(std::string_view parent_name,
                                 std::string_view this_array1_name, float node1_float_value, uint64_t node1_hex_value, int node1_bit_width,
                                 std::string_view this_array2_name, float node2_float_value, uint64_t node2_hex_value, int node2_bit_width) {
        std::string key1 = make_array_key(this_array1_name, parent_name);
        std::string key2 = make_array_key(this_array2_name, parent_name);
        
        size_t index1 = array_counters[key1]++;
        (void)array_counters[key2]++;  // ✨ Increment but don't store (index2 not used)
        
        // ✅ 创建节点并存储两组数据 / Create node and store two sets of data
        size_t idx = nodes.size();
        nodes.emplace_back();
        auto& node = nodes.back();
        
        // 第一组数据 / First group data
        char name_buf[64];
        int len = snprintf(name_buf, sizeof(name_buf), "%.*s[%zu]", 
                          (int)this_array1_name.size(), this_array1_name.data(), index1);
        set_node_name(node, std::string_view(name_buf, len));
        node.raw_value = node1_hex_value;
        node.bit_width = node1_bit_width;
        node.float_bits = float_to_bits(node1_float_value);  // ✅ 使用工具函数
        
        // ✨ 第二组数据（新增）/ Second group data (new)
        node.raw_value2 = node2_hex_value;
        node.bit_width2 = node2_bit_width;
        node.float2_bits = float_to_bits(node2_float_value);  // ✅ 使用工具函数 / Use utility function
        set_array2_name(node, this_array2_name);  // ✨ Store in fixed buffer
        
        node.node_type = NodeType::DualArray;  // ✅ 使用枚举 / Use enum
        
        add_node_common(node, idx, this_array1_name, parent_name);
    }

private:
    // ✨ ULTRA-OPTIMIZED: Store parent_idx on node, use hash map for parent lookup
    inline void add_node_common(Node& node, size_t idx, std::string_view this_node_name, std::string_view parent_name) {
        // ✨ FAST PATH: root name check (string_view comparison)
        if (this_node_name == root_name) {
            nodes.pop_back();
            return;
        }
        
        // Common operations
        node.insert_order = next_order++;
        
        // ✨ ULTRA-FAST PATH: Parent is root (most common case, ~90%)
        uint8_t depth;
        size_t p_idx = root_index;
        
        if (parent_name.empty() || parent_name == root_name) {
            // Fastest path: parent is root - no hash lookup
            depth = 1;
        } else {
            // ✨ OPTIMIZED: Use hash map instead of linear scan
            auto it = parent_cache_map.find(std::string(parent_name));
            if (it != parent_cache_map.end()) {
                // Cache hit
                p_idx = it->second;
                depth = nodes[p_idx].depth + 1;
            } else {
                // Slow path: linear scan through nodes to find parent
                depth = 1;
                p_idx = root_index;
                for (size_t i = nodes.size(); i > 0; --i) {
                    if (std::string_view(nodes[i-1].name_buf) == parent_name) {
                        depth = nodes[i-1].depth + 1;
                        p_idx = i - 1;
                        parent_cache_map[std::string(parent_name)] = p_idx;  // ✨ Cache for future lookups
                        break;
                    }
                }
            }
        }
        
        if (depth >= MAX_DEPTH) depth = 1;
        node.depth = depth;
        node.parent_idx = static_cast<uint32_t>(p_idx);
        
        // ✨ Mark parent as having children
        nodes[p_idx].has_children = true;
        
        // ✨ Add to children list (fast path for root, map for others)
        if (p_idx == root_index) {
            cached_root_children->push_back(idx);
        } else {
            // For non-root parents, use name to find children key
            children[std::string(nodes[p_idx].name_buf)].push_back(idx);
        }
    }
    


public:
    // ✨ ULTRA-OPTIMIZED: Use cached children during format
    std::string format() const {
        if (nodes.empty()) return "";
        
        std::string result;
        result.reserve(nodes.size() * 64);
        char is_last[MAX_DEPTH];
        
        build_tree_ultra_fast(root_index, is_last, result);
        return result;
    }
    
    std::string format_from(std::string_view start) const {
        // ✨ Linear scan to find node by name (format_from is rarely called)
        size_t start_idx = 0;
        bool found = false;
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (std::string_view(nodes[i].name_buf) == start) {
                start_idx = i;
                found = true;
                break;
            }
        }
        if (!found) return "";
        
        std::string result;
        result.reserve(4096);
        char is_last[MAX_DEPTH];
        build_subtree_ultra_fast(start_idx, nodes[start_idx].depth, is_last, result);
        return result;
    }

private:
    // ✨ ULTRA-FAST: Build tree from pre-computed child lists
    void build_tree_from_idx(size_t node_idx, char* is_last, std::string& result, 
                              const std::vector<std::vector<size_t>>& child_lists) const {
        const auto& node = nodes[node_idx];
        uint8_t depth = node.depth;
        
        append_prefix_ultra_fast(result, depth, is_last);
        
        if (is_simple_node(node)) {
            result += '[';
            result += node.name_buf;
            result += ']';
        } else {
            format_node_to(result, node);
        }
        result.push_back('\n');
        
        if (node.has_children && node_idx < child_lists.size()) {
            const auto& list = child_lists[node_idx];
            const size_t count = list.size();
            for (size_t i = 0; i < count; ++i) {
                is_last[depth] = (i == count - 1);
                build_tree_from_idx(list[i], is_last, result, child_lists);
            }
        }
    }
    
    // ✨ ULTRA-FAST: 使用 char 数组代替 vector, 避免 resize 开销
    void build_tree_ultra_fast(size_t node_idx, char* is_last, std::string& result) const {
        const auto& node = nodes[node_idx];
        uint8_t depth = node.depth;
        
        append_prefix_ultra_fast(result, depth, is_last);
        
        if (is_simple_node(node)) {
            result += '[';
            result += node.name_buf;
            result += ']';
        } else {
            format_node_to(result, node);
        }
        result.push_back('\n');
        
        const std::vector<size_t>* child_list = nullptr;
        if (node_idx == root_index) {
            child_list = cached_root_children;
        } else if (node.has_children) {
            auto it = children.find(std::string(node.name_buf));
            if (it != children.end()) {
                child_list = &it->second;
            }
        }
        
        if (child_list) {
            const size_t count = child_list->size();
            for (size_t i = 0; i < count; ++i) {
                is_last[depth] = (i == count - 1);
                build_tree_ultra_fast((*child_list)[i], is_last, result);
            }
        }
    }
    

    
    // ✨ ULTRA-FAST: Subtree with char array
    void build_subtree_ultra_fast(size_t node_idx, uint8_t start_depth, char* is_last, std::string& result) const {
        const auto& node = nodes[node_idx];
        uint8_t rel_depth = node.depth - start_depth;
        
        append_prefix_ultra_fast(result, rel_depth, is_last);
        
        if (is_simple_node(node)) {
            result += '[';
            result += node.name_buf;
            result += ']';
        } else {
            format_node_to(result, node);
        }
        result.push_back('\n');
        
        auto it = children.find(std::string(node.name_buf));
        if (it != children.end()) {
            const auto& list = it->second;
            const size_t count = list.size();
            
            for (size_t i = 0; i < count; ++i) {
                is_last[rel_depth] = (i == count - 1);
                build_subtree_ultra_fast(list[i], start_depth, is_last, result);
            }
        }
    }
    

    
    // ✨ V8.1 ULTRA: 直接写入 result，零 std::string 返回 / Direct write to result, zero std::string returns
    inline void format_node_to(std::string& result, const Node& node) const {
        if (is_simple_node(node)) {
            result += '[';
            result += node.name_buf;
            result += ']';
            return;
        }
        
        if (node.node_type == NodeType::Array) {
            format_array_node_to(result, node);
        }
        else if (node.node_type == NodeType::DualArray) {
            format_dual_array_node_to(result, node);
        }
        else if (node.bit_width == 128) {
            format_128bit_node_to(result, node);
        }
        else if (has_named_numeric_decoding(node)) {
            format_named_numeric_decoded_node_to(result, node);
        }
        else if (has_numeric_decoding(node)) {
            format_numeric_decoded_node_to(result, node);
        }
        else if (has_string_decoding(node)) {
            format_string_decoded_node_to(result, node);
        }
        else {
            format_standard_node_to(result, node);
        }
    }
    

    
private:
    // ✨ 辅助函数：判断是否为简单节点（无数据）/ Helper: check if simple node (no data)
    inline bool is_simple_node(const Node& node) const {
        return node.raw_value == 0 && node.bit_width == 0 && 
               !has_decoded_str(node) && node.decoded_value == 0 &&
               node.node_type != NodeType::Array && node.node_type != NodeType::DualArray &&
               node.bit_width != 128;
    }
    
    // ✨ 辅助函数：判断是否有数值解码 / Helper: check if has numeric decoding
    inline bool has_numeric_decoding(const Node& node) const {
        return node.decoded_value != 0 && node.decoded_bit_width != 0 && !has_decoded_str(node);
    }
    
    // ✨ 辅助函数：判断是否有命名数值解码 / Helper: check if has named numeric decoding
    inline bool has_named_numeric_decoding(const Node& node) const {
        return node.decoded_value != 0 && node.decoded_bit_width != 0 && has_decoded_str(node);
    }
    
    // ✨ 辅助函数：判断是否有字符串解码 / Helper: check if has string decoding
    inline bool has_string_decoding(const Node& node) const {
        return has_decoded_str(node);
    }
    
    // ✨ 格式化数组节点 / Format array node
    inline void format_array_node_to(std::string& result, const Node& node) const {
        result += node.name_buf;
        result += '=';
        float float_val = bits_to_float(node.float_bits);
        char buf[64];
        int len = snprintf(buf, sizeof(buf), "%.1f (0x%lX, b:", (double)float_val, (unsigned long)node.raw_value);
        result.append(buf, len);
        format_binary_to(result, node.raw_value, node.bit_width);
        result += ')';
    }
    

    
    // ✨ 格式化双数组节点 / Format dual array node
    inline void format_dual_array_node_to(std::string& result, const Node& node) const {
        result += node.name_buf;
        result += '=';
        float float1 = bits_to_float(node.float_bits);
        char buf[64];
        int len = snprintf(buf, sizeof(buf), "%.1f (0x%lX, b:", (double)float1, (unsigned long)node.raw_value);
        result.append(buf, len);
        format_binary_to(result, node.raw_value, node.bit_width);
        result += "), ";
        
        const char* node_name = node.name_buf;
        const char* bracket = std::strchr(node_name, '[');
        result += get_array2_name(node);
        if (bracket) {
            result += bracket;
        }
        result += '=';
        float float2 = bits_to_float(node.float2_bits);
        len = snprintf(buf, sizeof(buf), "%.1f (0x%lX, b:", (double)float2, (unsigned long)node.raw_value2);
        result.append(buf, len);
        format_binary_to(result, node.raw_value2, node.bit_width2);
        result += ')';
    }
    

    
    // ✨ 格式化 128 位节点 / Format 128-bit node
    inline void format_128bit_node_to(std::string& result, const Node& node) const {
        uint64_t high = static_cast<uint64_t>(node.value128 >> 64);
        uint64_t low = static_cast<uint64_t>(node.value128 & 0xFFFFFFFFFFFFFFFFULL);
        
        result += '[';
        result += node.name_buf;
        result += "]: 0x";
        char hex_buf[33];
        snprintf(hex_buf, sizeof(hex_buf), "%016llX%016llX", 
                (unsigned long long)high, (unsigned long long)low);
        result += hex_buf;
        result += " (b:";
        format_binary_to(result, high, 64);
        result += '_';
        format_binary_to(result, low, 64);
        result += ')';
    }
    

    
    // ✨ 格式化命名数值解码节点 / Format named numeric decoded node
    inline void format_named_numeric_decoded_node_to(std::string& result, const Node& node) const {
        result += '[';
        result += node.name_buf;
        result += "]: raw=0x";
        char buf[128];
        int len = snprintf(buf, sizeof(buf), "%lX (d:%lu/b:", (unsigned long)node.raw_value, (unsigned long)node.raw_value);
        result.append(buf, len);
        format_binary_to(result, node.raw_value, node.bit_width);
        result += ") → ";
        result += get_decoded_str(node);
        result += "=0x";
        len = snprintf(buf, sizeof(buf), "%lX (d:%lu/b:", (unsigned long)node.decoded_value, (unsigned long)node.decoded_value);
        result.append(buf, len);
        format_binary_to(result, node.decoded_value, node.decoded_bit_width);
        result += ')';
    }
    

    
    // ✨ 格式化数值解码节点 / Format numeric decoded node
    inline void format_numeric_decoded_node_to(std::string& result, const Node& node) const {
        result += '[';
        result += node.name_buf;
        result += "]: raw=0x";
        char buf[128];
        int len = snprintf(buf, sizeof(buf), "%lX (d:%lu/b:", (unsigned long)node.raw_value, (unsigned long)node.raw_value);
        result.append(buf, len);
        format_binary_to(result, node.raw_value, node.bit_width);
        result += ") → decoded=0x";
        len = snprintf(buf, sizeof(buf), "%lX (d:%lu/b:", (unsigned long)node.decoded_value, (unsigned long)node.decoded_value);
        result.append(buf, len);
        format_binary_to(result, node.decoded_value, node.decoded_bit_width);
        result += ')';
    }
    

    
    // ✨ 格式化字符串解码节点 / Format string decoded node
    inline void format_string_decoded_node_to(std::string& result, const Node& node) const {
        result += '[';
        result += node.name_buf;
        result += "]: raw=0x";
        char buf[128];
        int len = snprintf(buf, sizeof(buf), "%lX (d:%lu/b:", (unsigned long)node.raw_value, (unsigned long)node.raw_value);
        result.append(buf, len);
        format_binary_to(result, node.raw_value, node.bit_width);
        result += ") → decoded=";
        result += get_decoded_str(node);
    }
    

    
    // ✨ 格式化标准节点（无解码）/ Format standard node (no decoding)
    inline void format_standard_node_to(std::string& result, const Node& node) const {
        result += '[';
        result += node.name_buf;
        result += "]: raw=0x";
        char buf[128];
        int len = snprintf(buf, sizeof(buf), "%lX (d:%lu/b:", (unsigned long)node.raw_value, (unsigned long)node.raw_value);
        result.append(buf, len);
        format_binary_to(result, node.raw_value, node.bit_width);
        result += ')';
    }
    

    
    // ✨ ULTRA-FAST: Direct char buffer prefix (avoid vector lookup)
    inline void append_prefix_ultra_fast(std::string& result, uint8_t depth, const char* is_last) const {
        // UTF-8 encoded tree drawing characters
        static const char* branch_last = "\xe2\x94\x94\xe2\x94\x80";  // └─
        static const char* branch_mid  = "\xe2\x94\x9c\xe2\x94\x80";  // ├─
        static const char* indent_last = "  ";
        static const char* indent_mid  = "\xe2\x94\x82 ";             // │ 
        
        for (uint8_t i = 0; i < depth; ++i) {
            bool is_last_flag = is_last[i];
            bool is_branch = (i == depth - 1);
            
            if (is_branch) {
                result += is_last_flag ? branch_last : branch_mid;
            } else {
                result += is_last_flag ? indent_last : indent_mid;
            }
        }
    }
    

    
    // ✨ V8.1 ULTRA: 直接写入 result，零分配 / Direct write to result, zero allocation
    inline void format_binary_to(std::string& result, uint64_t value, int bit_width) const {
        if (bit_width <= 0) { result += '0'; return; }
        if (bit_width > 64) bit_width = 64;
        
        // ≤8 位：不需要分隔
        if (bit_width <= 8) {
            size_t old_size = result.size();
            result.resize(old_size + bit_width);
            char* ptr = &result[old_size];
            for (int i = bit_width - 1; i >= 0; --i) {
                ptr[bit_width - 1 - i] = (value & (1ULL << i)) ? '1' : '0';
            }
            return;
        }
        
        // >8 位：计算大小并直接写入
        int full_groups = bit_width / 8;
        int remaining_bits = bit_width % 8;
        int separator_count = (remaining_bits > 0) ? full_groups : (full_groups - 1);
        size_t total_size = bit_width + separator_count;
        
        size_t old_size = result.size();
        result.resize(old_size + total_size);
        char* ptr = &result[old_size];
        int pos = 0;
        int current_bit = bit_width;
        
        if (remaining_bits > 0) {
            int shift = current_bit - remaining_bits;
            uint64_t mask = (1ULL << remaining_bits) - 1;
            uint64_t group_value = (value >> shift) & mask;
            for (int i = remaining_bits - 1; i >= 0; --i) {
                ptr[pos++] = (group_value & (1ULL << i)) ? '1' : '0';
            }
            current_bit -= remaining_bits;
        }
        
        for (int g = 0; g < full_groups; ++g) {
            if (pos > 0) {
                ptr[pos++] = '\'';
            }
            current_bit -= 8;
            uint64_t group_value = (value >> current_bit) & 0xFF;
            ptr[pos++] = (group_value & 0x80) ? '1' : '0';
            ptr[pos++] = (group_value & 0x40) ? '1' : '0';
            ptr[pos++] = (group_value & 0x20) ? '1' : '0';
            ptr[pos++] = (group_value & 0x10) ? '1' : '0';
            ptr[pos++] = (group_value & 0x08) ? '1' : '0';
            ptr[pos++] = (group_value & 0x04) ? '1' : '0';
            ptr[pos++] = (group_value & 0x02) ? '1' : '0';
            ptr[pos++] = (group_value & 0x01) ? '1' : '0';
        }
    }
    
    // ✨ V8 新增：带分隔符的二进制格式化（每 8 位分隔）- 极致性能版本
    inline std::string format_binary_with_separator(uint64_t value, int bit_width) const {
        std::string result;
        if (bit_width <= 0) return "0";
        if (bit_width > 64) bit_width = 64;
        
        if (bit_width <= 8) {
            result.resize(bit_width);
            char* ptr = &result[0];
            for (int i = bit_width - 1; i >= 0; --i) {
                ptr[bit_width - 1 - i] = (value & (1ULL << i)) ? '1' : '0';
            }
            return result;
        }
        
        int full_groups = bit_width / 8;
        int remaining_bits = bit_width % 8;
        int separator_count = (remaining_bits > 0) ? full_groups : (full_groups - 1);
        size_t total_size = bit_width + separator_count;
        result.resize(total_size);
        
        char* ptr = &result[0];
        int pos = 0;
        int current_bit = bit_width;
        
        if (remaining_bits > 0) {
            int shift = current_bit - remaining_bits;
            uint64_t mask = (1ULL << remaining_bits) - 1;
            uint64_t group_value = (value >> shift) & mask;
            for (int i = remaining_bits - 1; i >= 0; --i) {
                ptr[pos++] = (group_value & (1ULL << i)) ? '1' : '0';
            }
            current_bit -= remaining_bits;
        }
        
        for (int g = 0; g < full_groups; ++g) {
            if (pos > 0) {
                ptr[pos++] = '\'';
            }
            current_bit -= 8;
            uint64_t group_value = (value >> current_bit) & 0xFF;
            ptr[pos++] = (group_value & 0x80) ? '1' : '0';
            ptr[pos++] = (group_value & 0x40) ? '1' : '0';
            ptr[pos++] = (group_value & 0x20) ? '1' : '0';
            ptr[pos++] = (group_value & 0x10) ? '1' : '0';
            ptr[pos++] = (group_value & 0x08) ? '1' : '0';
            ptr[pos++] = (group_value & 0x04) ? '1' : '0';
            ptr[pos++] = (group_value & 0x02) ? '1' : '0';
            ptr[pos++] = (group_value & 0x01) ? '1' : '0';
        }
        
        return result;
    }
};

} // namespace module_log