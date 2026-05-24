#!/usr/bin/env fish

set -g ROOT (pwd)
set -g BUILD_DIR "$ROOT/cmake-build-debug"
set -g APP "$BUILD_DIR/ProiectOOP"
set -g TMP_DIR "$ROOT/teste_polimorfism"
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
    if grep -Eq "$pattern" "$file"
        echo "  [✗] $label (pattern found but should NOT be present)"
    else
        echo "  [✓] $label"
        set -g PASS (math $PASS + 1)
    end
end

function write_input
    set -l file "$argv[1]"
    set -e argv[1]
    printf '%s\n' $argv > "$file"
end

function test_afisare_polimorfica
    # Create SD, create SV, show all
    write_input "$INPUT_DIR/1.in" "5" "1" "polyDed" "2048" "1" "3.0" "true" "ID_DED_POLY" "30.0" "Xeon" "true" "12" \
    "2" "polyVirt" "1024" "1" "2.0" "true" "ID_VIRT_POLY" "15.0" "VMware" "75" "8000" \
    "3" "0" "0"
    "$APP" < "$INPUT_DIR/1.in" > "$OUTPUT_DIR/1.out" 2>&1
    require_pattern "$OUTPUT_DIR/1.out" "ServerDedicat" "Afisare polimorfica ServerDedicat"
    require_pattern "$OUTPUT_DIR/1.out" "ServerVirtual" "Afisare polimorfica ServerVirtual"
end

function test_tostring
    write_input "$INPUT_DIR/3.in" "0"
    "$APP" < "$INPUT_DIR/3.in" > "$OUTPUT_DIR/3.out" 2>&1
    require_pattern "$OUTPUT_DIR/3.out" "procesor=Xeon" "toString() ServerDedicat la startup"
    require_pattern "$OUTPUT_DIR/3.out" "virtualizare=KVM" "toString() ServerVirtual la startup"
    require_pattern "$OUTPUT_DIR/3.out" "BaseSrv" "toString() Server la startup"
end

function test_dynamic_cast_succes
    write_input "$INPUT_DIR/6.in" "5" "1" "castDed" "4096" "1" "3.5" "true" "ID_DED_CAST" "100.0" "Threadripper" "true" "48" \
    "2" "castVirt" "2048" "1" "2.5" "true" "ID_VIRT_CAST" "20.0" "KVM" "60" "10000" \
    "4" "0" "4" "1" "0" "0"
    "$APP" < "$INPUT_DIR/6.in" > "$OUTPUT_DIR/6.out" 2>&1
    require_pattern "$OUTPUT_DIR/6.out" "Procesor: Threadripper" "dynamic_cast ServerDedicat succes"
    require_pattern "$OUTPUT_DIR/6.out" "Virtualizare: KVM" "dynamic_cast ServerVirtual succes"
end

function test_dynamic_cast_esuat
    write_input "$INPUT_DIR/8.in" "5" "2" "castVirt" "2048" "1" "2.5" "true" "ID_VIRT_CAST" "20.0" "KVM" "60" "10000" \
    "4" "0" "0" "0"
    "$APP" < "$INPUT_DIR/8.in" > "$OUTPUT_DIR/8.out" 2>&1
    require_no_pattern "$OUTPUT_DIR/8.out" "Segmentation fault" "Fara crash la dynamic_cast esuat"
    require_no_pattern "$OUTPUT_DIR/8.out" "Procesor:" "Fail to read Processor -> output expected generic or skip. Acesta nu e ServerDedicat"
end

function test_destructor_chain
    write_input "$INPUT_DIR/9.in" "5" "1" "delDed" "1024" "1" "2.0" "true" "ID_DEL" "10.0" "Xeon" "false" "12" \
    "7" "0" "0" "0"
    "$APP" < "$INPUT_DIR/9.in" > "$OUTPUT_DIR/9.out" 2>&1
    require_pattern "$OUTPUT_DIR/9.out" "Destructor ServerDedicat" "Destructor chain 1"
    require_pattern "$OUTPUT_DIR/9.out" "Destructor ResursaCloud" "Destructor chain 2"
end

function test_calculeaza_cost
    write_input "$INPUT_DIR/10.in" "5" "1" "costDed" "4096" "2" "3.0" "3.2" "true" "ID_COST1" "20.0" "Xeon" "true" "12" \
    "2" "costVirt" "2048" "1" "2.5" "true" "ID_COST2" "10.0" "KVM" "50" "5000" \
    "5" "0" "0"
    "$APP" < "$INPUT_DIR/10.in" > "$OUTPUT_DIR/10.out" 2>&1
    require_pattern "$OUTPUT_DIR/10.out" "Cost total:.*" "calculeazaCost polimorfic"
end

echo "--- Rulare teste Polimorfism ---"
test_afisare_polimorfica
test_tostring
test_dynamic_cast_succes
test_dynamic_cast_esuat
test_destructor_chain
test_calculeaza_cost

echo ""
echo "PASSED: $PASS / TOTAL: $TOTAL"
