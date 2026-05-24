#!/usr/bin/env fish

set -g ROOT (pwd)
set -g BUILD_DIR "$ROOT/cmake-build-debug"
set -g APP "$BUILD_DIR/ProiectOOP"
set -g TMP_DIR "$ROOT/teste_stl"
set -g INPUT_DIR "$TMP_DIR/input"
set -g OUTPUT_DIR "$TMP_DIR/output"
set -g PASS 0
set -g TOTAL 0

mkdir -p "$INPUT_DIR"
mkdir -p "$OUTPUT_DIR"

function require_pattern
    set -l file "$argv[1]"
    set -l pattern "$argv[2]"
    set -l label "$argv[3]"
    set -g TOTAL (math $TOTAL + 1)
    if grep -Eq "$pattern" "$file"
        echo "  [✓] $label"
        set -g PASS (math $PASS + 1)
    else
        echo "  [✗] $label (missing pattern: $pattern)"
    end
end
function require_no_pattern
    set -l file "$argv[1]"
    set -l pattern "$argv[2]"
    set -l label "$argv[3]"
    set -g TOTAL (math $TOTAL + 1)
    if not grep -Eq "$pattern" "$file"
        echo "  [✓] $label"
        set -g PASS (math $PASS + 1)
    else
        echo "  [✗] $label (found unwanted pattern: $pattern)"
    end
end
function write_input
    set -l file "$argv[1]"
    set -e argv[1]
    printf '%s\n' $argv > "$file"
end

function test_sort
    write_input "$INPUT_DIR/1.in" "5" "1" "HighCost" "1024" "1" "1.0" "true" "ID1" "100.0" "Xeon" "false" "12" \
    "2" "LowCost" "1024" "1" "1.0" "true" "ID2" "10.0" "KVM" "50" "100" \
    "12" "1" "0" "0" "0"
    "$APP" < "$INPUT_DIR/1.in" > "$OUTPUT_DIR/1.out" 2>&1
    require_pattern "$OUTPUT_DIR/1.out" "nume=HighCost" "Sort descending by cost (HighCost)"
    require_pattern "$OUTPUT_DIR/1.out" "nume=LowCost" "Sort descending by cost (LowCost)"
end

function test_find_if_gasit
    write_input "$INPUT_DIR/2.in" "5" "1" "Cost200" "1024" "1" "1.0" "true" "ID" "200.0" "Xeon" "false" "12" \
    "12" "2" "100" "0" "0" "0"
    "$APP" < "$INPUT_DIR/2.in" > "$OUTPUT_DIR/2.out" 2>&1
    require_pattern "$OUTPUT_DIR/2.out" "Cost200" "find_if gasit (threshold=100)"
end

function test_find_if_negasit
    write_input "$INPUT_DIR/3.in" "5" "1" "Cost10" "1024" "1" "1.0" "true" "ID" "10.0" "Xeon" "false" "12" \
    "12" "2" "9999" "0" "0" "0"
    "$APP" < "$INPUT_DIR/3.in" > "$OUTPUT_DIR/3.out" 2>&1
    require_pattern "$OUTPUT_DIR/3.out" "Nicio resursa cu cost" "find_if nu s-a gasit (threshold=9999)"
end

function test_count_if
    write_input "$INPUT_DIR/4.in" "5" "1" "On1" "1024" "1" "1.0" "true" "ID" "10.0" "Xeon" "false" "12" \
    "1" "On2" "1024" "1" "1.0" "true" "ID2" "10.0" "Xeon" "false" "12" \
    "2" "Off" "1024" "1" "1.0" "false" "ID3" "10.0" "KVM" "50" "100" \
    "12" "3" "0" "0" "0"
    "$APP" < "$INPUT_DIR/4.in" > "$OUTPUT_DIR/4.out" 2>&1
    require_pattern "$OUTPUT_DIR/4.out" "Resurse online:" "count_if online servers"
end

function test_for_each
    write_input "$INPUT_DIR/5.in" "5" "1" "S1" "1024" "1" "1.0" "true" "ID1" "10.0" "Xeon" "false" "12" \
    "2" "S2" "1024" "1" "1.0" "true" "ID2" "10.0" "KVM" "50" "100" \
    "12" "4" "0" "0" "0"
    "$APP" < "$INPUT_DIR/5.in" > "$OUTPUT_DIR/5.out" 2>&1
    require_pattern "$OUTPUT_DIR/5.out" "nume=S1" "for_each summary display (1)"
    require_pattern "$OUTPUT_DIR/5.out" "nume=S2" "for_each summary display (2)"
end

function test_any_of_da
    write_input "$INPUT_DIR/6.in" "3" "1" "VM_On" "2" "1024" "10.0.0.1" "ipv4" "true" "true" "10" "20" \
    "5" "0" "0"
    "$APP" < "$INPUT_DIR/6.in" > "$OUTPUT_DIR/6.out" 2>&1
    require_pattern "$OUTPUT_DIR/6.out" "da" "any_of VM pornita = da"
end

function test_any_of_nu
    write_input "$INPUT_DIR/7.in" "3" "1" "VM_Off" "2" "1024" "10.0.0.1" "ipv4" "true" "false" "10" "20" \
    "5" "0" "0"
    "$APP" < "$INPUT_DIR/7.in" > "$OUTPUT_DIR/7.out" 2>&1
    require_pattern "$OUTPUT_DIR/7.out" "Niciun VM nu este pornit" "any_of VM pornita = nu"
end

function test_transform
    write_input "$INPUT_DIR/8.in" "1" "1" "TransformSrvA" "1024" "1" "1.0" "true" "1" "TransformSrvB" "1024" "1" "1.0" "true" "0" \
    "1" "9" "0" "0"
    "$APP" < "$INPUT_DIR/8.in" > "$OUTPUT_DIR/8.out" 2>&1
    require_pattern "$OUTPUT_DIR/8.out" "TransformSrvA" "transform name list 1"
    require_pattern "$OUTPUT_DIR/8.out" "TransformSrvB" "transform name list 2"
end

function test_erase_remove_if
    write_input "$INPUT_DIR/9.in" "1" "1" "KeepMe" "1024" "1" "1.0" "true" "1" "RemoveMe" "1024" "1" "1.0" "false" "0" \
    "1" "6" "0" "0"
    "$APP" < "$INPUT_DIR/9.in" > "$OUTPUT_DIR/9.out" 2>&1
    require_pattern "$OUTPUT_DIR/9.out" "KeepMe" "erase-remove_if keeps online"
    require_pattern "$OUTPUT_DIR/9.out" "Servere online \(1 din 2\)" "erase-remove_if drops offline"
end

function test_structured_binding
    write_input "$INPUT_DIR/10.in" "5" "1" "Ch1" "1024" "1" "1.0" "true" "ID1" "50.0" "Xeon" "false" "12" \
    "2" "Ch2" "1024" "1" "1.0" "true" "ID2" "10.0" "KVM" "50" "100" \
    "12" "5" "0" "0" "0"
    "$APP" < "$INPUT_DIR/10.in" > "$OUTPUT_DIR/10.out" 2>&1
    require_pattern "$OUTPUT_DIR/10.out" "\[0\] Server" "std::pair structured binding result 1"
    require_pattern "$OUTPUT_DIR/10.out" "cost:" "std::pair structured binding result 2"
end


echo "--- Rulare teste STL Algorithms ---"
test_sort
test_find_if_gasit
test_find_if_negasit
test_count_if
test_for_each
test_any_of_da
test_any_of_nu
test_transform
test_erase_remove_if
test_structured_binding

echo ""
echo "PASSED: $PASS / TOTAL: $TOTAL"
