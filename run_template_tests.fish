#!/usr/bin/env fish

set -g ROOT (pwd)
set -g BUILD_DIR "$ROOT/cmake-build-debug"
set -g APP "$BUILD_DIR/ProiectOOP"
set -g TMP_DIR "$ROOT/teste_template"
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

function require_pattern_count
    set -l file "$argv[1]"
    set -l pattern "$argv[2]"
    set -l expected "$argv[3]"
    set -l label "$argv[4]"
    set -l count (grep -Ec "$pattern" "$file")
    set -g TOTAL (math $TOTAL + 1)
    if test "$count" -eq "$expected"
        echo "  [✓] $label (count=$count)"
        set -g PASS (math $PASS + 1)
    else
        echo "  [✗] $label (count=$count, expected=$expected)"
    end
end

function write_input
    set -l file "$argv[1]"
    set -e argv[1]
    printf '%s\n' $argv > "$file"
end

function test_depozitar_adauga_get
    write_input "$INPUT_DIR/1.in" "5" "1" "DepSrv" "1024" "1" "1.0" "true" "ID_DEP1" "10.0" "A" "false" "12" \
    "3" "0" "0"
    "$APP" < "$INPUT_DIR/1.in" > "$OUTPUT_DIR/1.out" 2>&1
    require_pattern "$OUTPUT_DIR/1.out" "Resursa \[0\]" "Depozitar adauga si get: index 0 exista"
    require_pattern "$OUTPUT_DIR/1.out" "ID_DEP1" "Depozitar adauga si get: ID corect"
end

function test_depozitar_elimina
    write_input "$INPUT_DIR/2.in" "5" "1" "Dep1" "1024" "1" "1.0" "true" "ID_DEP_1" "10.0" "A" "false" "12" \
    "1" "Dep2" "1024" "1" "1.0" "true" "ID_DEP_2" "10.0" "A" "false" "12" \
    "7" "0" "3" "0" "0"
    "$APP" < "$INPUT_DIR/2.in" > "$OUTPUT_DIR/2.out" 2>&1
    require_pattern_count "$OUTPUT_DIR/2.out" "Resursa \[0\]" 1 "Depozitar elimina (doar resursa noua ramasa)"
    require_pattern "$OUTPUT_DIR/2.out" "ID_DEP_2" "Depozitar elimina: ID_DEP_2 a ramas la index 0"
end

function test_depozitar_get_bounds
    write_input "$INPUT_DIR/3.in" "5" "1" "Dep1" "1024" "1" "1.0" "true" "ID" "10.0" "A" "false" "12" \
    "4" "999" "0" "0"
    "$APP" < "$INPUT_DIR/3.in" > "$OUTPUT_DIR/3.out" 2>&1
    require_pattern "$OUTPUT_DIR/3.out" "Index invalid" "Depozitar bounds check la get (ExceptieValidare)"
end

function test_cautaLiniar_gasit
    write_input "$INPUT_DIR/5.in" "5" "1" "DepCost" "1024" "1" "1.0" "true" "ID_COST" "100.0" "Xeon" "false" "12" \
    "8" "5" "0" "0" "0"
    "$APP" < "$INPUT_DIR/5.in" > "$OUTPUT_DIR/5.out" 2>&1
    require_pattern "$OUTPUT_DIR/5.out" "Resursa gasita" "cautaLiniar - gasit"
end

function test_cautaLiniar_negasit
    write_input "$INPUT_DIR/6.in" "5" "1" "DepCost" "1024" "1" "1.0" "true" "ID_COST" "100.0" "Xeon" "false" "12" \
    "8" "9999" "0" "0" "0"
    "$APP" < "$INPUT_DIR/6.in" > "$OUTPUT_DIR/6.out" 2>&1
    require_pattern "$OUTPUT_DIR/6.out" "Nicio resursa cu ID=9999" "cautaLiniar - negasit"
end

function test_sorteaza
    write_input "$INPUT_DIR/7.in" "5" "1" "C1" "1024" "1" "1.0" "true" "ID1" "50.0" "Xeon" "false" "12" \
    "1" "C2" "1024" "1" "1.0" "true" "ID2" "100.0" "Xeon" "false" "12" \
    "9" "0" "0"
    "$APP" < "$INPUT_DIR/7.in" > "$OUTPUT_DIR/7.out" 2>&1
    require_pattern "$OUTPUT_DIR/7.out" "nume=C1" "sorteaza crescator 1"
    require_pattern "$OUTPUT_DIR/7.out" "nume=C2" "sorteaza crescator 2"
end

function test_numara
    write_input "$INPUT_DIR/8.in" "5" "1" "C1" "1024" "1" "1.0" "true" "ID1" "50.0" "A" "false" "12" \
    "1" "C2" "1024" "1" "1.0" "true" "ID2" "10.0" "A" "false" "12" \
    "1" "C3" "1024" "1" "1.0" "true" "ID3" "70.0" "A" "false" "12" \
    "10" "0" "0"
    "$APP" < "$INPUT_DIR/8.in" > "$OUTPUT_DIR/8.out" 2>&1
    require_pattern "$OUTPUT_DIR/8.out" "Servere online:" "numara with predicate"
end

function test_cautaLiniar_specializare
    write_input "$INPUT_DIR/9.in" "5" "1" "Spec1" "1024" "1" "1.0" "true" "ID_FIND_ME" "10.0" "A" "false" "12" \
    "8" "ID_FIND_ME" "0" "0"
    "$APP" < "$INPUT_DIR/9.in" > "$OUTPUT_DIR/9.out" 2>&1
    require_pattern "$OUTPUT_DIR/9.out" "Spec1" "Specializare totala cautaLiniar<ResursaCloud*>"
end

echo "--- Rulare teste Template ---"
test_depozitar_adauga_get
test_depozitar_elimina
test_depozitar_get_bounds
test_cautaLiniar_gasit
test_cautaLiniar_negasit
test_sorteaza
test_numara
test_cautaLiniar_specializare

echo ""
echo "PASSED: $PASS / TOTAL: $TOTAL"
