# Prompt: Library-Health-Report erneut ausführen

## Ausgangslage

Am 2026-07-20 wurde eine sechsteilige Untersuchung von `CoreTestament` durchgeführt und
unter `reports/library-health-2026-07-20.md` festgehalten: Shared-Lib-Größe,
Symbol-Export-Audit, statische Analyse, Speicherlecks, Code Coverage, Performance. Ergebnis
damals: keine Bugs/Leaks, aber sechs offene Punkte (siehe Tabelle im alten Report), u.a.
fehlende Symbol-Sichtbarkeitssteuerung, zwei Move-statt-Copy-Stellen in
`AssertionFailure`/`SkipRequest`, ein GCC-16-ICE der ASan/UBSan blockiert, fehlendes
`gcovr`/`lcov` im Container, ungetestete `SkipRequest`-Pfade.

Diese Untersuchung soll **erneut** durchgeführt werden — entweder auf Anfrage oder nachdem
sich der Code seither spürbar weiterentwickelt hat — um zu prüfen, was sich verändert hat
und ob die damals offenen Punkte behoben wurden.

## Aufgabe

Wiederhole exakt die sechs Untersuchungen aus dem alten Report, jeweils mit echten
Builds/Läufen im Podman-Container (`./scripts/run-container.sh`), nicht aus Schätzungen:

1. **Shared-Lib-Größe:** Release-Build mit `-DBUILD_SHARED_LIBS=ON`, Größe ungestrippt und
   gestrippt (`strip`) messen.
2. **Symbol-Export-Audit:** `nm -D --defined-only -C libCoreTestamentFramework.so`, Anzahl
   und Kategorisierung (Internal*/detail::/Impl::/STL-Rauschen). Prüfen, ob Punkt 1 aus der
   alten Zusammenfassungstabelle (Sichtbarkeitssteuerung) inzwischen umgesetzt wurde — falls
   ja, sollte die Symbolzahl deutlich unter 445 liegen.
3. **Statische Analyse:** `cppcheck --enable=warning,style,performance,portability` und
   `clang-tidy --checks="-*,bugprone-*,performance-*,clang-analyzer-*"` über
   `CoreTestament/src`. Prüfen, ob die beiden Move-Ctor-Funde (`AssertionFailure.cpp`,
   `SkipRequest.cpp`) noch bestehen.
4. **Speicherlecks:** Erst versuchen, ob GCC-16 `-fsanitize=address,undefined` inzwischen
   ohne ICE durchläuft (Datei zum Reproduzieren: `ParallelTestValidation.cpp`); falls nicht,
   Valgrind (`--leak-check=full --show-leak-kinds=definite,indirect`) auf `TestRunner`,
   `ConcurrentRunnerValidation`, `ParallelSuiteValidation`, `ParallelTestValidation`,
   `TestamentRunner` (Beispiele) als Ausweichlösung.
5. **Code Coverage:** Debug-Build mit `--coverage`, `ctest`, dann `gcov` je `.cpp`-Datei
   auswerten (`grep -cE "^ *[0-9]+:"` vs. `"^ *#####:"` pro `*.cpp.gcov`, aufsummieren).
   Prüfen, ob `gcovr`/`lcov` inzwischen im Container verfügbar sind — falls ja, damit statt
   der manuellen gcov-Auswertung arbeiten (liefert auch für Header eine korrekte Zahl).
6. **Performance:** Generierte Suite mit ausschließlich leeren `Test("name", [] {})` (200 und
   2000 Stück), Release `-O2`, je 5 Läufe, Gesamtzeit via `date +%s%N`, sowie Compile-Zeit
   für dieselben Dateien.

## Wichtig — Nebenbedingungen

- Alle Scratch-Builds in eigene Unterverzeichnisse unter `build/` legen (z. B.
  `build/analysis-*`), niemals das bestehende `build/`-Verzeichnis selbst oder bereits
  committete Dateien anfassen.
- Nichts committen, sofern nicht explizit angewiesen.
- Nach Abschluss alle Scratch-Verzeichnisse/-Dateien wieder löschen — nur der fertige Report
  bleibt.
- Die Codebasis entwickelt sich schnell (Klassen-/Dateinamen aus dem alten Report können
  inzwischen umbenannt oder verschoben worden sein) — vor der Untersuchung kurz prüfen, ob
  referenzierte Dateien/Klassen noch unter denselben Namen existieren, und bei Bedarf die
  aktuellen Entsprechungen verwenden statt blind den alten Namen zu folgen.

## Erwartete Abschlussausgabe

Ein neuer, datierter Report unter `reports/library-health-<YYYY-MM-DD>.md`, im selben Aufbau
wie der vorherige (sechs nummerierte Abschnitte + Zusammenfassungstabelle am Ende). Am Anfang
kurz vermerken, welche der sechs Punkte aus der alten Zusammenfassungstabelle inzwischen
behoben wurden und welche weiterhin offen sind — nicht nur neue Rohzahlen, sondern ein
Diff zur letzten Untersuchung.
