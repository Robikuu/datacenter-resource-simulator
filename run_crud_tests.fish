#!/usr/bin/env fish

# End-to-end CRUD smoke tests for ProiectOOP (Proiect 1 + Proiect 2).
# Run with: fish run_crud_tests.fish

set -l ROOT /home/robert/ProiectOOP
set -l BUILD_DIR "$ROOT/cmake-build-debug"
set -l APP "$BUILD_DIR/ProiectOOP"
set -l TMP_DIR "$ROOT/teste"
set -l INPUT_DIR "$TMP_DIR/input"
set -l OUTPUT_DIR "$TMP_DIR/output"
set -g PASS 0
set -g TOTAL 0

mkdir -p "$INPUT_DIR"
mkdir -p "$OUTPUT_DIR"

function fail
    echo "[FAIL] $argv"
    exit 1
end

function require_pattern
    set -l file "$argv[1]"
    set -l pattern "$argv[2]"
    set -l label "$argv[3]"
    set -g TOTAL (math $TOTAL + 1)
    if grep -Eq "$pattern" "$file"
        echo "  [OK] $label"
        set -g PASS (math $PASS + 1)
    else
        echo "  [MISSING] $label"
        echo "  pattern: $pattern"
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
        echo "  [OK] $label (count=$count)"
        set -g PASS (math $PASS + 1)
    else
        echo "  [MISSING] $label (count=$count, expected>=$expected_min)"
    end
end

function require_no_pattern
    set -l file "$argv[1]"
    set -l pattern "$argv[2]"
    set -l label "$argv[3]"
    set -g TOTAL (math $TOTAL + 1)
    if grep -Eq "$pattern" "$file"
        echo "  [MISSING] $label (pattern found but should NOT be present)"
    else
        echo "  [OK] $label"
        set -g PASS (math $PASS + 1)
    end
end

function write_input
    set -l file "$argv[1]"
    set -e argv[1]
    printf '%s\n' $argv > "$file"
end

echo "== Build =="
cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug || fail "CMake configure failed"
cmake --build "$BUILD_DIR" -j || fail "Build failed"

if not test -x "$APP"
    fail "Binary not found: $APP"
end

# ============================================================
#  PROIECT 1 SCENARIOS (1-9) — kept from original
# ============================================================

# --- Scenario 1: Server + IP CRUD ---
write_input "$INPUT_DIR/01.in" \
"1" "1" "srvA" "1024" "2" "2.5" "2.7" "true" \
"1" "srvB" "2048" "1" "3.1" "false" \
"2" "3" "1" "srvB_upd" "3072" "2" "3.2" "3.3" "true" \
"4" "0" "2" "0" \
"2" "1" "10.0.0.1" "ipv4" "true" \
"1" "2001:db8:85a3:0:0:8a2e:370:7334" "ipv6" "false" \
"3" "0" "10.0.0.10" "ipv4" "false" \
"4" "1" "2" "0" \
"0"
"$APP" < "$INPUT_DIR/01.in" > "$OUTPUT_DIR/01.out" 2>&1

echo "== 01: Server + IP CRUD =="
require_pattern "$OUTPUT_DIR/01.out" "Server creat cu succes" "server create"
require_pattern "$OUTPUT_DIR/01.out" "Server actualizat cu succes" "server update"
require_pattern "$OUTPUT_DIR/01.out" "IP actualizat cu succes" "ip update"
require_pattern "$OUTPUT_DIR/01.out" "IP sters cu succes" "ip delete"

# --- Scenario 2: VM linking ---
write_input "$INPUT_DIR/02.in" \
"1" "1" "srvOff" "4096" "1" "2.0" "false" \
"1" "srvOn" "8192" "1" "3.0" "true" \
"1" "srvOn2" "4096" "1" "2.9" "true" \
"0" \
"3" "1" "vm_no_link" "2" "1024" "10.1.1.1" "ipv4" "false" "true" "20" "30" "0" \
"1" "vm_try_offline" "1" "1024" "10.1.1.2" "ipv4" "false" "false" "10" "1" "0" \
"1" "vm_link_prompt" "1" "1024" "10.1.1.3" "ipv4" "true" "true" "50" "1" "1" \
"1" "vm_link_direct" "1" "1024" "10.1.1.4" "ipv4" "true" "false" "15" "2" "2" \
"0" "0"
"$APP" < "$INPUT_DIR/02.in" > "$OUTPUT_DIR/02.out" 2>&1

echo "== 02: VM linking =="
require_pattern "$OUTPUT_DIR/02.out" "VM creat cu succes" "vm create"
require_pattern "$OUTPUT_DIR/02.out" "VM legat de serverul" "vm linked"
require_pattern "$OUTPUT_DIR/02.out" "offline.*fara server|fara server.*offline" "offline warning"

# --- Scenario 3: Delete Server decouples VM ---
write_input "$INPUT_DIR/03.in" \
"1" "1" "srvKeep" "2048" "1" "2.4" "true" \
"1" "srvDel" "2048" "1" "2.6" "true" \
"0" \
"3" "1" "vmBound" "1" "1024" "10.2.2.2" "ipv4" "true" "true" "40" "1" "1" "2" "0" \
"1" "4" "1" "0" \
"3" "2" "0" \
"0"
"$APP" < "$INPUT_DIR/03.in" > "$OUTPUT_DIR/03.out" 2>&1

echo "== 03: Delete server decouples VM =="
require_pattern "$OUTPUT_DIR/03.out" "Server sters cu succes" "server delete"
require_pattern "$OUTPUT_DIR/03.out" "Server gazda VM: N/A" "vm decoupled"

# --- Scenario 4: Delete VM cleanup ---
write_input "$INPUT_DIR/04.in" \
"3" "1" "vm1" "2" "1024" "10.3.3.1" "ipv4" "false" "true" "11" "12" \
"1" "vm2" "1" "1024" "10.3.3.2" "ipv4" "true" "false" "22" \
"2" "4" "0" "2" "0" \
"0"
"$APP" < "$INPUT_DIR/04.in" > "$OUTPUT_DIR/04.out" 2>&1

echo "== 04: Delete VM cleanup =="
require_pattern "$OUTPUT_DIR/04.out" "VM sters cu succes" "vm delete"
require_pattern "$OUTPUT_DIR/04.out" "vm2" "remaining vm shown"

# --- Scenario 5: Datacenter CRUD ---
write_input "$INPUT_DIR/05.in" \
"1" "1" "srvDC0" "4096" "1" "2.2" "true" \
"1" "srvDC1" "8192" "1" "3.0" "true" \
"0" \
"4" "1" "DC1" "Bucuresti" "2" "0" "1" "2" \
"3" "0" "DC1_UPD" "Cluj" "1" "1" "2" \
"4" "0" "2" "0" \
"0"
"$APP" < "$INPUT_DIR/05.in" > "$OUTPUT_DIR/05.out" 2>&1

echo "== 05: Datacenter CRUD =="
require_pattern "$OUTPUT_DIR/05.out" "Datacenter creat cu succes" "datacenter create"
require_pattern "$OUTPUT_DIR/05.out" "Datacenter actualizat cu succes" "datacenter update"
require_pattern "$OUTPUT_DIR/05.out" "Datacenter sters cu succes" "datacenter delete"

# --- Scenario 6: Empty-collection guards ---
write_input "$INPUT_DIR/06.in" \
"1" "2" "3" "4" "0" \
"2" "2" "3" "4" "0" \
"3" "2" "3" "4" "0" \
"4" "2" "3" "4" "0" \
"5" "3" "4" "0" "5" "6" "0" "7" "0" "0" \
"0"
"$APP" < "$INPUT_DIR/06.in" > "$OUTPUT_DIR/06.out" 2>&1

echo "== 06: Empty-collection guards =="
require_pattern_count_at_least "$OUTPUT_DIR/06.out" "Nu exista servere\." 3 "server empty guards"
require_pattern_count_at_least "$OUTPUT_DIR/06.out" "Nu exista IP-uri\." 3 "ip empty guards"
require_pattern_count_at_least "$OUTPUT_DIR/06.out" "Nu exista VM-uri\." 3 "vm empty guards"
require_pattern_count_at_least "$OUTPUT_DIR/06.out" "Nu exista datacentere\." 3 "datacenter empty guards"
require_pattern_count_at_least "$OUTPUT_DIR/06.out" "Nu exista resurse cloud\." 3 "resurse cloud empty guards"

# --- Scenario 7: Invalid indexes ---
write_input "$INPUT_DIR/07.in" \
"1" "1" "srvIDX" "1024" "1" "2.0" "true" "3" "-1" "4" "9" "0" \
"2" "1" "10.7.7.7" "ipv4" "true" "3" "7" "4" "-1" "0" \
"3" "1" "vmIDX" "1" "1024" "10.7.7.8" "ipv4" "false" "true" "20" "0" "3" "99" "4" "-1" "0" \
"4" "1" "dcIDX" "Bucuresti" "0" "3" "5" "4" "-1" "0" \
"0"
"$APP" < "$INPUT_DIR/07.in" > "$OUTPUT_DIR/07.out" 2>&1

echo "== 07: Invalid indexes =="
require_pattern_count_at_least "$OUTPUT_DIR/07.out" "Index invalid\." 8 "invalid index guards"

# --- Scenario 8: Max server capacity ---
write_input "$INPUT_DIR/08.in" \
"1" \
"1" "srv0" "1024" "1" "2.0" "true" \
"1" "srv1" "1024" "1" "2.0" "true" \
"1" "srv2" "1024" "1" "2.0" "true" \
"1" "srv3" "1024" "1" "2.0" "true" \
"1" "srv4" "1024" "1" "2.0" "true" \
"1" "srv5" "1024" "1" "2.0" "true" \
"1" "srv6" "1024" "1" "2.0" "true" \
"1" "srv7" "1024" "1" "2.0" "true" \
"1" "srv8" "1024" "1" "2.0" "true" \
"1" "srv9" "1024" "1" "2.0" "true" \
"0" \
"0"
"$APP" < "$INPUT_DIR/08.in" > "$OUTPUT_DIR/08.out" 2>&1

echo "== 08: Max server capacity (Adaptat STL: fara limita fixa) =="
require_pattern_count_at_least "$OUTPUT_DIR/08.out" "Server creat cu succes" 10 "ten creates"

# --- Scenario 9: DC invalid server refs ---
write_input "$INPUT_DIR/09.in" \
"1" "1" "srvA" "2048" "1" "2.1" "true" "1" "srvB" "2048" "1" "2.2" "true" "0" \
"4" "1" "dc_invalid_idx" "Iasi" "3" "99" "-1" "1" "2" "0" \
"0"
"$APP" < "$INPUT_DIR/09.in" > "$OUTPUT_DIR/09.out" 2>&1

echo "== 09: DC invalid server refs =="
require_pattern "$OUTPUT_DIR/09.out" "Datacenter creat cu succes" "dc create with mixed idx"
require_pattern_count_at_least "$OUTPUT_DIR/09.out" "Index invalid\. Server ignorat\." 2 "invalid server idx ignored"

# ============================================================
#  PROIECT 2 SCENARIOS (10-16)
# ============================================================

# --- Scenario 10: Create ServerDedicat + ServerVirtual ---
write_input "$INPUT_DIR/10.in" \
"5" \
"1" "srvDed1" "4096" "1" "3.5" "true" "RES_DED1" "50.0" "Xeon" "true" "24" \
"1" "srvDed2" "8192" "2" "2.8" "3.0" "true" "RES_DED2" "75.0" "EPYC" "false" "36" \
"2" "srvVirt1" "2048" "1" "2.5" "true" "RES_VIRT1" "10.0" "KVM" "50" "5000" \
"2" "srvVirt2" "1024" "1" "2.0" "false" "RES_VIRT2" "5.0" "Xen" "25" "3000" \
"3" \
"0" \
"0"
"$APP" < "$INPUT_DIR/10.in" > "$OUTPUT_DIR/10.out" 2>&1

echo "== 10: Create ServerDedicat + ServerVirtual =="
require_pattern_count_at_least "$OUTPUT_DIR/10.out" "ServerDedicat creat cu succes" 2 "dedicat creates"
require_pattern_count_at_least "$OUTPUT_DIR/10.out" "ServerVirtual creat cu succes" 2 "virtual creates"
require_pattern "$OUTPUT_DIR/10.out" "Tip Procesor: Xeon" "dedicat Xeon shown"
require_pattern "$OUTPUT_DIR/10.out" "Tip Procesor: EPYC" "dedicat EPYC shown"
require_pattern "$OUTPUT_DIR/10.out" "Tip Virtualizare: KVM" "virtual KVM shown"
require_pattern "$OUTPUT_DIR/10.out" "Tip Virtualizare: Xen" "virtual Xen shown"

# --- Scenario 11: Polymorphic display (afiseazaDetalii via base ptr) ---
write_input "$INPUT_DIR/11.in" \
"5" \
"1" "polyDed" "2048" "1" "3.0" "true" "ID_DED_POLY" "30.0" "Xeon" "true" "12" \
"2" "polyVirt" "1024" "1" "2.0" "true" "ID_VIRT_POLY" "15.0" "VMware" "75" "8000" \
"3" \
"0" \
"0"
"$APP" < "$INPUT_DIR/11.in" > "$OUTPUT_DIR/11.out" 2>&1

echo "== 11: Polymorphic display =="
require_pattern "$OUTPUT_DIR/11.out" "Resursa \[0\].*tip real:.*ServerDedicat" "typeid dedicat"
require_pattern "$OUTPUT_DIR/11.out" "Resursa \[1\].*tip real:.*ServerVirtual" "typeid virtual"
require_pattern "$OUTPUT_DIR/11.out" "Detalii Server" "base server details"
require_pattern "$OUTPUT_DIR/11.out" "Detalii Dedicat" "derived dedicat details"
require_pattern "$OUTPUT_DIR/11.out" "Detalii Virtual" "derived virtual details"

# --- Scenario 12: dynamic_cast on ServerDedicat vs ServerVirtual ---
write_input "$INPUT_DIR/12.in" \
"5" \
"1" "castDed" "4096" "1" "3.5" "true" "ID_DED_CAST" "100.0" "Threadripper" "true" "48" \
"2" "castVirt" "2048" "1" "2.5" "true" "ID_VIRT_CAST" "20.0" "KVM" "60" "10000" \
"4" "0" \
"4" "1" \
"0" \
"0"
"$APP" < "$INPUT_DIR/12.in" > "$OUTPUT_DIR/12.out" 2>&1

echo "== 12: dynamic_cast =="
require_pattern "$OUTPUT_DIR/12.out" "Aceasta resursa este un ServerDedicat" "cast to dedicat"
require_pattern "$OUTPUT_DIR/12.out" "Procesor: Threadripper" "dedicat-specific field"
require_pattern "$OUTPUT_DIR/12.out" "Aceasta resursa este un ServerVirtual" "cast to virtual"
require_pattern "$OUTPUT_DIR/12.out" "Virtualizare: KVM" "virtual-specific field"
require_pattern "$OUTPUT_DIR/12.out" "Limite IOPS: 10000" "virtual IOPS field"

# --- Scenario 13: Construction/destruction order ---
write_input "$INPUT_DIR/13.in" \
"5" \
"1" "orderDed" "1024" "1" "2.0" "true" "ID_ORDER" "10.0" "Xeon" "false" "12" \
"7" "0" \
"0" \
"0"
"$APP" < "$INPUT_DIR/13.in" > "$OUTPUT_DIR/13.out" 2>&1

echo "== 13: Construction/destruction order =="
require_pattern "$OUTPUT_DIR/13.out" "Constructor ResursaCloud" "ctor ResursaCloud"
require_pattern "$OUTPUT_DIR/13.out" "Constructor Server" "ctor Server"
require_pattern "$OUTPUT_DIR/13.out" "Constructor ServerDedicat" "ctor ServerDedicat"
require_pattern "$OUTPUT_DIR/13.out" "Destructor ServerDedicat" "dtor ServerDedicat"
require_pattern "$OUTPUT_DIR/13.out" "Destructor Server" "dtor Server"
require_pattern "$OUTPUT_DIR/13.out" "Destructor ResursaCloud" "dtor ResursaCloud"

# --- Scenario 14: Cost calculation ---
write_input "$INPUT_DIR/14.in" \
"5" \
"1" "costDed" "4096" "2" "3.0" "3.2" "true" "ID_COST1" "20.0" "Xeon" "true" "12" \
"2" "costVirt" "2048" "1" "2.5" "true" "ID_COST2" "10.0" "KVM" "50" "5000" \
"5" \
"0" \
"0"
"$APP" < "$INPUT_DIR/14.in" > "$OUTPUT_DIR/14.out" 2>&1

echo "== 14: Cost calculation =="
require_pattern "$OUTPUT_DIR/14.out" "Resursa \[0\]:.*RON/ora" "dedicat cost row"
require_pattern "$OUTPUT_DIR/14.out" "Resursa \[1\]:.*RON/ora" "virtual cost row"
require_pattern "$OUTPUT_DIR/14.out" "Cost total:.*RON/ora" "total cost"

# --- Scenario 15: afiseazaStatus extends base method ---
write_input "$INPUT_DIR/15.in" \
"5" \
"1" "statusDed" "4096" "1" "3.5" "true" "ID_STAT1" "10.0" "EPYC" "true" "24" \
"2" "statusVirt" "2048" "1" "2.5" "true" "ID_STAT2" "5.0" "Xen" "80" "7000" \
"6" "0" \
"6" "1" \
"0" \
"0"
"$APP" < "$INPUT_DIR/15.in" > "$OUTPUT_DIR/15.out" 2>&1

echo "== 15: afiseazaStatus chain =="
require_pattern "$OUTPUT_DIR/15.out" "Status Resursa Cloud" "base status called"
require_pattern "$OUTPUT_DIR/15.out" "Procesor: EPYC.*RAID:" "dedicat status extends"
require_pattern "$OUTPUT_DIR/15.out" "Virtualizare: Xen.*Resurse:" "virtual status extends"

# --- Scenario 16: Delete from polymorphic collection + invalid idx ---
write_input "$INPUT_DIR/16.in" \
"5" \
"1" "delDed" "1024" "1" "2.0" "true" "ID_DEL1" "10.0" "Xeon" "false" "12" \
"2" "delVirt" "1024" "1" "2.0" "true" "ID_DEL2" "5.0" "KVM" "50" "1000" \
"7" "99" \
"7" "-1" \
"7" "0" \
"3" \
"7" "0" \
"3" \
"0" \
"0"
"$APP" < "$INPUT_DIR/16.in" > "$OUTPUT_DIR/16.out" 2>&1

echo "== 16: Delete from polymorphic collection =="
require_pattern_count_at_least "$OUTPUT_DIR/16.out" "Resursa stearsa cu succes" 2 "two deletes"
require_pattern_count_at_least "$OUTPUT_DIR/16.out" "Index invalid\." 2 "invalid idx guards"
require_pattern "$OUTPUT_DIR/16.out" "Nu exista resurse cloud" "empty after delete all"

echo
echo "=============================="
echo "Results: $PASS / $TOTAL checks passed"
echo "Outputs in: $OUTPUT_DIR"
echo "=============================="
