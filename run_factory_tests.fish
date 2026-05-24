#!/usr/bin/env fish

set -g ROOT (pwd)
set -g BUILD_DIR "$ROOT/cmake-build-debug"
set -g APP "$BUILD_DIR/ProiectOOP"
set -g TMP_DIR "$ROOT/teste_factory"
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

function write_input
    set -l file "$argv[1]"
    set -e argv[1]
    printf '%s\n' $argv > "$file"
end

function test_factory_dedicat_a
    write_input "$INPUT_DIR/1.in" "7" "1" "5" "14" "1" "3" "0" "0"
    "$APP" < "$INPUT_DIR/1.in" > "$OUTPUT_DIR/1.out" 2>&1
    require_pattern "$OUTPUT_DIR/1.out" "EPYC" "Creare ServerDedicat via ProviderA (procesor EPYC)"
    require_pattern "$OUTPUT_DIR/1.out" "32" "Creare ServerDedicat via ProviderA (32 cores)"
end

function test_factory_virtual_a
    write_input "$INPUT_DIR/2.in" "7" "1" "5" "14" "2" "3" "0" "0"
    "$APP" < "$INPUT_DIR/2.in" > "$OUTPUT_DIR/2.out" 2>&1
    require_pattern "$OUTPUT_DIR/2.out" "KVM" "Creare ServerVirtual via ProviderA (virtualizare KVM)"
    require_pattern "$OUTPUT_DIR/2.out" "80" "Creare ServerVirtual via ProviderA (80% resurse)"
end

function test_factory_dedicat_b
    write_input "$INPUT_DIR/3.in" "7" "2" "5" "14" "1" "3" "0" "0"
    "$APP" < "$INPUT_DIR/3.in" > "$OUTPUT_DIR/3.out" 2>&1
    require_pattern "$OUTPUT_DIR/3.out" "Xeon" "Creare ServerDedicat via ProviderB (procesor Xeon)"
    require_pattern "$OUTPUT_DIR/3.out" "16" "Creare ServerDedicat via ProviderB (16 cores)"
end

function test_factory_virtual_b
    write_input "$INPUT_DIR/4.in" "7" "2" "5" "14" "2" "3" "0" "0"
    "$APP" < "$INPUT_DIR/4.in" > "$OUTPUT_DIR/4.out" 2>&1
    require_pattern "$OUTPUT_DIR/4.out" "VMware" "Creare ServerVirtual via ProviderB (virtualizare VMware)"
    require_pattern "$OUTPUT_DIR/4.out" "50" "Creare ServerVirtual via ProviderB (50% resurse)"
end

function test_schimbare_provider
    write_input "$INPUT_DIR/5.in" "7" "1" "5" "14" "1" "0" "7" "2" "5" "14" "1" "3" "0" "0"
    "$APP" < "$INPUT_DIR/5.in" > "$OUTPUT_DIR/5.out" 2>&1
    require_pattern "$OUTPUT_DIR/5.out" "Provider schimbat cu succes pe B" "Schimbare provider la runtime"
    require_pattern "$OUTPUT_DIR/5.out" "Xeon" "Clientul foloseste noul provider fara alte modificari"
end

function test_client_agnostic
    set -g TOTAL (math $TOTAL + 1)
    if grep -Eq "FabricaProviderA\b" "$ROOT/ClientCloud.cpp"
        echo "  [✗] Clientul NU ar trebui sa cunoasca tipul concret (gasit FabricaProviderA)"
    else if grep -Eq "FabricaProviderB\b" "$ROOT/ClientCloud.cpp"
        echo "  [✗] Clientul NU ar trebui sa cunoasca tipul concret (gasit FabricaProviderB)"
    else
        echo "  [✓] Clientul NU ar trebui sa cunoasca tipul concret"
        set -g PASS (math $PASS + 1)
    end
end

function test_provider_name_in_menu
    write_input "$INPUT_DIR/7.in" "7" "2" "5" "14" "1" "0" "0"
    "$APP" < "$INPUT_DIR/7.in" > "$OUTPUT_DIR/7.out" 2>&1
    require_pattern "$OUTPUT_DIR/7.out" "Provider B" "Provider name displayed in menu"
end

echo "--- Rulare teste Factory ---"
test_factory_dedicat_a
test_factory_virtual_a
test_factory_dedicat_b
test_factory_virtual_b
test_schimbare_provider
test_client_agnostic
test_provider_name_in_menu

echo ""
echo "PASSED: $PASS / TOTAL: $TOTAL"
