#!/usr/bin/env fish

if not test -f ./cmake-build-debug/ProiectOOP
    echo "EROARE: Binaryul nu exista. Ruleaza cmake --build cmake-build-debug -j intai."
    exit 1
end

echo "========================================"
echo "    PROIECT 3 — TEST SUITE COMPLET"
echo "========================================"
echo ""

set total_passed 0
set total_tests 0

for suite in run_crud_tests.fish run_exceptii_tests.fish run_polimorfism_tests.fish run_stl_tests.fish run_singleton_tests.fish run_factory_tests.fish run_template_tests.fish
    echo "--- $suite ---"
    set result (fish $suite 2>&1)
    echo "$result"

    # Parse "PASSED: N / TOTAL: M" from last line
    set last_line (echo "$result" | tail -1)
    if string match -q -- "PASSED:*" "$last_line"
        set p (string replace -r "PASSED: ([0-9]+) / TOTAL: ([0-9]+)" '$1' -- "$last_line")
        set t (string replace -r "PASSED: ([0-9]+) / TOTAL: ([0-9]+)" '$2' -- "$last_line")
        if test -n "$p" -a -n "$t"
            set total_passed (math $total_passed + $p)
            set total_tests (math $total_tests + $t)
        end
    else if string match -q -- "Results:*" "$last_line"
        set p (string replace -r "Results: ([0-9]+) / ([0-9]+).*" '$1' -- "$last_line")
        set t (string replace -r "Results: ([0-9]+) / ([0-9]+).*" '$2' -- "$last_line")
        if test -n "$p" -a -n "$t"
            set total_passed (math $total_passed + $p)
            set total_tests (math $total_tests + $t)
        end
    end
    echo ""
end

echo "========================================"
echo "SUMAR FINAL: $total_passed / $total_tests teste trecute"
echo "========================================"
