#!/usr/bin/env fish

set -g ROOT (pwd)
set -g BUILD_DIR "$ROOT/cmake-build-debug"
set -g APP "$BUILD_DIR/ProiectOOP"
set -g TMP_DIR "$ROOT/teste_singleton"
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

function require_pattern_count_at_least
    set -l file "$argv[1]"
    set -l pattern "$argv[2]"
    set -l expected_min "$argv[3]"
    set -l label "$argv[4]"
    set -l count (grep -Ec "$pattern" "$file")
    set -g TOTAL (math $TOTAL + 1)
    if test "$count" -ge "$expected_min"
        echo "  [✓] $label (count=$count)"
        set -g PASS (math $PASS + 1)
    else
        echo "  [✗] $label (count=$count, expected>=$expected_min)"
    end
end

function write_input
    set -l file "$argv[1]"
    set -e argv[1]
    printf '%s\n' $argv > "$file"
end

function test_logger_acumuleaza
    write_input "$INPUT_DIR/1.in" "1" "1" "LoggerSrv" "1024" "1" "1.0" "true" "0" \
    "5" "1" "LoggerDed" "1024" "1" "1.0" "true" "ID" "10.0" "A" "false" "12" "0" \
    "6" "0"
    "$APP" < "$INPUT_DIR/1.in" > "$OUTPUT_DIR/1.out" 2>&1
    require_pattern_count_at_least "$OUTPUT_DIR/1.out" "\[EVENT" 2 "Logger acumuleaza intrari"
end

function test_constructor_server
    write_input "$INPUT_DIR/2.in" "1" "1" "LogCtorSrv" "1024" "1" "1.0" "true" "0" "6" "0"
    "$APP" < "$INPUT_DIR/2.in" > "$OUTPUT_DIR/2.out" 2>&1
    require_pattern "$OUTPUT_DIR/2.out" "Server creat: nume=DEFAULT" "Constructor Server logat"
end

function test_destructor_server
    write_input "$INPUT_DIR/3.in" "1" "1" "LogDtorSrv" "1024" "1" "1.0" "true" "4" "0" "0" "6" "0"
    "$APP" < "$INPUT_DIR/3.in" > "$OUTPUT_DIR/3.out" 2>&1
    require_pattern "$OUTPUT_DIR/3.out" "Server distrus: nume=LogDtorSrv" "Destructor Server logat"
end

function test_constructor_resursa
    write_input "$INPUT_DIR/4.in" "5" "1" "LogCtorDed" "1024" "1" "1.0" "true" "ID_RES_CTOR" "10.0" "Xeon" "false" "12" "0" "6" "0"
    "$APP" < "$INPUT_DIR/4.in" > "$OUTPUT_DIR/4.out" 2>&1
    require_pattern "$OUTPUT_DIR/4.out" "ResursaCloud creata: id=SRV_DEFAULT" "Constructor ResursaCloud logat"
end

function test_crud_logged
    write_input "$INPUT_DIR/5.in" "5" "1" "LogCrud" "1024" "1" "1.0" "true" "ID_CRUD" "10.0" "A" "false" "12" \
    "7" "0" "13" "0" "0"
    "$APP" < "$INPUT_DIR/5.in" > "$OUTPUT_DIR/5.out" 2>&1
    require_pattern "$OUTPUT_DIR/5.out" "CRUD: ServerDedicat creat" "CRUD create logat"
    require_pattern "$OUTPUT_DIR/5.out" "CRUD: Resursa stearsa" "CRUD delete logat"
end

function test_contor_evenimente
    write_input "$INPUT_DIR/7.in" "1" "1" "Srv1" "1024" "1" "1.0" "true" "1" "Srv2" "1024" "1" "1.0" "true" "0" \
    "5" "1" "Ded1" "1024" "1" "1.0" "true" "ID" "10.0" "A" "false" "12" "0" "6" "0"
    "$APP" < "$INPUT_DIR/7.in" > "$OUTPUT_DIR/7.out" 2>&1
    require_pattern "$OUTPUT_DIR/7.out" "EVENT 1" "Contor evenimente incepe corect"
    require_pattern "$OUTPUT_DIR/7.out" "EVENT 2" "Contor evenimente creste corect"
    require_pattern "$OUTPUT_DIR/7.out" "EVENT 3" "Contor evenimente creste in continuare"
end

echo "--- Rulare teste Singleton Logger ---"
test_logger_acumuleaza
test_constructor_server
test_destructor_server
test_constructor_resursa
test_crud_logged
test_contor_evenimente

echo ""
echo "PASSED: $PASS / TOTAL: $TOTAL"
