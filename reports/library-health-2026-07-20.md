# CoreTestament — Library Health Report

Datum: 2026-07-20
Branch: `maintenance/build-and-test-reliability`
Untersucht gegen: aktueller Arbeitsstand inkl. der noch unkommittierten Änderungen
(`CoreTestament/CMakeLists.txt`: `BUILD_SHARED_LIBS`-Option; `CoreTestament/examples/`).

Alle Zahlen in diesem Report stammen aus echten Builds/Läufen im Podman-Container
(`./scripts/run-container.sh`), nicht aus Schätzungen. Alle Scratch-Build-Verzeichnisse
(`build/analysis-*`, `build/bench*`, `build/gcov-*`) sind nach der Untersuchung entfernt
worden; nichts davon ist committet.

---

## 1. Größe der Shared Library

Release-Build (`-DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON`), GCC 16:

| Variante | Größe |
|---|---|
| `libCoreTestamentFramework.so`, ungestrippt | 520.920 Bytes (~509 KiB) |
| `libCoreTestamentFramework.so`, gestrippt (`strip`) | 418.296 Bytes (~408 KiB) |

Zum Vergleich: die statische `.a` (Debug, `BUILD_TESTING=ON`, mit Test-Objektdateien
im selben Verzeichnis) lag bei ca. 4,1 MB — kein fairer direkter Vergleich (andere
Build-Konfiguration), aber als Hausnummer festgehalten.

**Einschätzung:** Für eine Bibliothek dieser Funktionsbreite (DSL-Runtime, JUnit-Reporter,
Parallel-Runner, Skip/Trace/Environment-Subsysteme) ist ~400 KB gestrippt unauffällig klein.
Kein Handlungsbedarf.

## 2. Sind alle exportierten Symbole notwendig?

**Nein — spürbar nicht.** Es gibt aktuell keine `-fvisibility=hidden` / `__declspec`-Steuerung
irgendwo im Projekt (bereits in einem früheren Review als offener Punkt notiert). Auf ELF/GCC
bedeutet das: jedes Symbol mit externer Bindung landet automatisch in der dynamischen
Symboltabelle — auch rein interne Implementierungsklassen.

```
nm -D --defined-only -C libCoreTestamentFramework.so | wc -l
→ 445 exportierte Symbole
```

Kategorisierung (grobe, aber belastbare Zählung per `grep` auf die demanglten Namen):

| Kategorie | Anzahl | Beispiel |
|---|---|---|
| `Testament::Internal*`-Klassen (`InternalSuite`, `InternalTest`, `InternalRegistry`) | 70 | `Testament::InternalSuite::getName() const` |
| `Testament::*::detail::*`-Namespace | 106 | `Testament::Asserts::detail::failAssertion(...)` |
| `*::Impl::*` (private PImpl-Klassen) | 6 | `Testament::AssertionFailure::Impl::Impl(...)` |
| STL-Template-Instanziierungen (`std::...<...>`) | 175 | `std::vector<Testament::detail::TestHandle>::...` |

Rund 180 der 445 Symbole (~40%) gehören zu Klassen, die **nirgends in einem öffentlichen
Header (`include/Testament/*.hpp`) deklariert sind** — `InternalSuite`, `InternalRegistry`,
`HookManager`, `TestManager`, `ConfigurationDiagnostics`, `OptionsStorage`, alle `*::Impl`-Typen
und alles unter `Testament::detail::` (mit Ausnahme der bewusst öffentlichen Typen, die zufällig
auch `detail` im Namen tragen). Kein einziges davon wird von einem Consumer benötigt oder ist
über einen öffentlichen Header überhaupt erreichbar — sie sind heute nur exportiert, weil
niemand die Sichtbarkeit eingeschränkt hat.

**Risiko, nicht nur Kosmetik:** Ungewollt exportierte interne Symbole sind ein ABI-Versprechen,
das nie beabsichtigt war — jede Umbenennung von `InternalSuite::getName()` ist heute technisch
eine Breaking Change für jeden, der (warum auch immer) gegen dieses Symbol linkt.

**Empfehlung:**
1. `CMAKE_CXX_VISIBILITY_PRESET hidden` + `CMAKE_VISIBILITY_INLINES_HIDDEN ON` auf dem
   `CoreTestamentFramework`-Target setzen.
2. Für die öffentlichen Klassen ein Export-Makro einführen (`generate_export_header()` oder
   manuell `TESTAMENT_API`), analog zum bereits empfohlenen Schritt für eine MSVC-taugliche
   Shared-Library.
3. Nach der Umstellung erneut `nm -D` prüfen — Zielbild: nur noch Symbole von Klassen aus
   `include/Testament/*.hpp` (keine `Internal*`, kein `detail::`, kein `Impl::`).

Das ist derselbe Befund, der auch schon in der `SHARED`-Machbarkeitsdiskussion als Vorbehalt
für MSVC genannt wurde — hier zeigt sich: er betrifft nicht nur Windows, sondern die
ABI-Sauberkeit grundsätzlich.

## 3. Statische Codeanalyse

Ja, vorhanden und im Container installiert: `cppcheck` 2.19.0, `clang-tidy` (LLVM 21).
Kein `.clang-tidy` im Repo — Standard-Checks wurden manuell gewählt.

### cppcheck (`--enable=warning,style,performance,portability`, 18 Quelldateien)

18 Fundstellen, alle **style/performance**, keine **error**-Klasse. Eine davon zunächst
verdächtig, bei Prüfung aber ein Fehlalarm:

- **Falsch-Positiv, verifiziert:** `CoreTestament/src/core/Asserts.cpp:84–88` — cppcheck meldet
  „Access of moved variable 'assertion'/'expected'“. Bei Prüfung des Codes: Die beiden
  `std::move(...)`-Aufrufe liegen in zwei **sich gegenseitig ausschließenden** `catch`-Zweigen
  (`catch (const std::exception&)` vs. `catch (...)`) — pro Aufruf wird höchstens einer davon
  ausgeführt. cppcheck erkennt die Alternativität der Catch-Zweige nicht. Kein Bug.
- Rest: durchgehend kosmetisch (`useStlAlgorithm`, `constParameterReference`,
  `constVariableReference`, `functionStatic`, ein `passedByValue`-Perf-Hinweis in `Runner.cpp:344`).
  Kein Handlungsbedarf, optional beim nächsten Aufräumen mitnehmbar.

### clang-tidy (`bugprone-*,performance-*,clang-analyzer-*`, Kernbibliothek)

4 Fundstellen, alle real und je 5 Minuten wert:

1. **`AssertionFailure.cpp:43`** und **`SkipRequest.cpp:24`** (`performance-move-constructor-init`):
   ```cpp
   AssertionFailure::AssertionFailure(AssertionFailure&& other) noexcept : impl(other.impl) {}
   SkipRequest::SkipRequest(SkipRequest&& other) noexcept : impl(other.impl) {}
   ```
   `impl` ist ein `shared_ptr<const Impl>`. Der „Move“-Konstruktor ruft faktisch den
   Copy-Konstruktor des `shared_ptr` auf (unnötiger atomarer Refcount-Increment statt reinem
   Zeiger-Transfer). Korrekt, aber suboptimal — `impl(std::move(other.impl))` wäre der
   eigentliche Move. Betrifft auch `operator=(SkipRequest&&)` (Zeile 26: `impl = other.impl;`).
   Gefahrlos zu fixen, kein Verhaltensunterschied.
2. **`Asserts.cpp:82`** (`performance-unnecessary-value-param`): `exception_ptr exception` in
   `failUnexpectedException` wird per Wert übernommen und nur einmal kopiert — könnte gemoved
   werden. Marginal, da `exception_ptr`-Kopien billig sind (Refcount), aber korrekt erkannt.
3. **`JUnitTestEventHandler.cpp:141`** (`bugprone-inc-dec-in-conditions`):
   ```cpp
   if (++index >= arguments.size() || arguments[index].empty()) { ... }
   ```
   Kein UB, `||` ist kurzschlussausgewertet, also sicher — aber schwerer zu lesen. Stilfrage,
   kein Bug.

**Fazit Statik:** Keine Korrektheitsbugs gefunden. Zwei echte, risikolose Mikro-Optimierungen
(Move-statt-Copy in zwei Exception-Typen).

## 4. Speicherlecks

**Ergebnis: keine gefunden**, aber mit einer wichtigen Einschränkung zur Werkzeug-Verfügbarkeit:

- **GCC 16 (Container-Standard) + `-fsanitize=address,undefined`:** Build bricht mit einem
  internen Compiler-Absturz (ICE) beim Kompilieren von `ParallelTestValidation.cpp` ab
  (`internal compiler error: Segmentation fault` in `gt_ggc_mx_lang_tree_node`, GCC-16-Trunk-Bug,
  nicht unser Code — passt zum bereits früher in diesem Projekt beobachteten Muster von
  GCC-16-ICEs unter Sonderflags). ASan/UBSan+GCC-16 ist damit aktuell **nicht nutzbar**.
- **Clang 21 (einziger im Container verfügbarer Clang) + Sanitizer:** Linker findet
  `libclang_rt.asan*.a` nicht — das Sanitizer-Runtime-Paket für Clang 21 ist im Containerfile
  nicht installiert. Ebenfalls nicht nutzbar, ohne das Image zu ändern.
- **Ausweichlösung: Valgrind (im Container vorhanden, 3.26.0).** `--leak-check=full
  --show-leak-kinds=definite,indirect` auf fünf repräsentativen Binaries (`TestRunner` —
  das große interne Aggregat, `ConcurrentRunnerValidation`, `ParallelSuiteValidation`,
  `ParallelTestValidation`, `TestamentRunner` — die neuen Beispiele):

  ```
  TestRunner: HEAP SUMMARY: in use at exit: 0 bytes in 0 blocks
              total heap usage: 117 allocs, 117 frees, 185.736 bytes allocated
              All heap blocks were freed -- no leaks are possible
              ERROR SUMMARY: 0 errors from 0 contexts
  ```
  Alle fünf Binaries: 0 definite/indirect leaks, Exit-Code 0.

**Empfehlung:** `libclang-rt-21-dev` (oder das passende Paket für den installierten
Clang) ins `Containerfile` aufnehmen, damit ASan/UBSan+LeakSanitizer künftig auch mit Clang
als Fallback für die GCC-16-Instabilität nutzbar ist. Das vorhandene `sanitizers`-CMake-Preset
setzt aktuell hart auf `gcc-toolchain.cmake` und deaktiviert `detect_leaks` im zugehörigen
Test-Preset (`ASAN_OPTIONS: detect_leaks=0`) — vermutlich genau wegen dieses ICE-Problems.
Sobald der GCC-16-Bug behoben oder Clang nutzbar ist, sollte `detect_leaks=1` der Standard
werden, nicht die Ausnahme.

## 5. Code Coverage

Kein `gcovr`/`lcov` im Container — nur `gcov` direkt verfügbar. Damit lässt sich für
Header, die von mehreren `.cpp`-Dateien inkludiert werden, keine korrekt vereinigte
Coverage berechnen (jede TU liefert nur ihre eigene Teilsicht). Für die 18 eindeutigen
`.cpp`-Implementierungsdateien der Kernbibliothek ist die Zahl dagegen unzweideutig
(jede Datei wird genau einmal übersetzt):

```
Build: Debug + --coverage (GCC 16), alle 46 Tests grün
Aggregiert über 18 .cpp-Dateien: 936 / 1062 ausführbare Zeilen = 88,14 %
```

Schwächste Stellen:

| Datei | Abdeckung | Bemerkung |
|---|---|---|
| `SkipRequest.cpp` | 50,0 % (8/16) | Copy-Ctor/-Assign, Move-Assign, Destruktor, `what()` nie ausgeführt — nur `.reason()` wird in Tests genutzt |
| `LifecycleSuite.cpp` | 66,7 % (6/9) | |
| `Reporters.cpp` | 66,7 % (4/6) | |
| `AssertionFailure.cpp` | 82,8 % (24/29) | |

Stärkste/vollständige Abdeckung: `Trace.cpp`, `ConfigurationDiagnostics.cpp`,
`SuiteAssembler.cpp`, `DefaultRun.cpp` (je 100 %).

**Empfehlung:**
1. `gcovr` ins `Containerfile` aufnehmen (`pip install gcovr` oder Paket, falls vorhanden) —
   ohne sauberes Merge-Tool ist eine korrekte Gesamt-Coverage inkl. Header nicht seriös zu
   ermitteln, nur eine Annäherung wie hier.
2. `SkipRequest`s Copy/Move/`what()`-Pfade sind ungetestet — passend zum unter Punkt 3
   gefundenen Move-Ctor-Bug: ein Test, der eine `SkipRequest` tatsächlich kopiert/moved und
   `.what()` aufruft, hätte diesen (harmlosen) Move-Bug vermutlich nicht gefangen (da beide
   Varianten funktional gleich sind), würde aber die Lücke schließen.

## 6. Performance

### Laufzeit-Overhead pro Test (Release, GCC 16 `-O2`, Konsolen-Reporter aktiv)

Generierte Suiten mit ausschließlich leeren `Test("name", [] {})`-Einträgen (reiner
Framework-Overhead, kein Testkörper-Anteil), je 5 Läufe, Gesamtzeit per `date +%s%N`:

| Testanzahl | Ø Gesamtzeit | Ø pro Test |
|---|---|---|
| 200 | 7.722 µs | ~38,6 µs |
| 2.000 | 32.175 µs | ~16,1 µs |

Der Unterschied erklärt sich durch einen festen Overhead pro Prozess (Registry-Setup,
Reporter-Initialisierung, Prozessstart — überschlägig ~5 ms), der sich bei mehr Tests
besser amortisiert. Reiner Grenzkosten-Anteil pro zusätzlichem Test: ~13,6 µs.

**Einordnung:** Für einen Reporter, der pro Test eine formatierte, emoji-versehene
Konsolenzeile schreibt, ist das ein plausibler, unauffälliger Wert — nicht mit reinem
Dispatch-Overhead verwechseln, das ist Ende-zu-Ende inklusive I/O.

### Compile-Zeit-Skalierung (ein `Suite(...)`-Aufruf mit N trivialen `Test(...)`-Einträgen)

| Testanzahl (eine Datei, ein `Suite(...)`) | Compile-Zeit (G++-16, `-O2`) |
|---|---|
| 200 | 8,5 s |
| 2.000 | 91,6 s |

Skaliert näherungsweise linear (10× Tests → 10,8× Zeit), keine erkennbare quadratische
Explosion trotz variadischem Template-Pack mit 2000 Definitionen. **Aber:** 2000
`Test(...)`-Aufrufe in einem einzigen `Suite(...)`-Aufruf in einer Datei ist ein
Extremfall — reale Suiten liegen typischerweise im ein- bis niedrigen zweistelligen
Bereich. Kein akuter Handlungsbedarf, aber relevant, falls jemand versucht, sehr große
tabellarische Testmengen in einem einzigen `Suite(...)`-Aufruf statt über
`ParameterizedTest`/`Cases(...)` abzubilden.

---

## Zusammenfassung — was ggf. behoben werden sollte

| # | Befund | Schwere | Aufwand |
|---|---|---|---|
| 1 | Keine Symbol-Sichtbarkeitssteuerung — ~40 % der exportierten Symbole sind rein interne Implementierung | Mittel (ABI-Sauberkeit, kein akuter Bug) | Mittel (`visibility=hidden` + Export-Makro für öffentliche Klassen) |
| 2 | `AssertionFailure`/`SkipRequest`-Move-Konstruktoren rufen Copy statt Move auf `shared_ptr`-Member | Niedrig (Performance, kein Korrektheitsproblem) | Trivial (`std::move(other.impl)`) |
| 3 | GCC-16-ICE blockiert ASan/UBSan-Builds; Clang-Sanitizer-Runtime fehlt im Container | Mittel (blockiert Tooling, nicht das Produkt) | Containerfile-Änderung (Clang-Runtime-Paket) oder auf GCC-16-Fix warten |
| 4 | `gcovr`/`lcov` fehlen — Coverage-Zahlen für Header nicht seriös ermittelbar | Niedrig (Tooling-Lücke) | Containerfile-Änderung |
| 5 | `SkipRequest`: Copy/Move/`what()` ungetestet | Niedrig | Ein zusätzlicher Test |
| 6 | cppcheck-Fehlalarm zu „moved variable“ in `Asserts.cpp` | Kein Fix nötig | — (dokumentiert, damit niemand das später fälschlich „behebt“) |

Keine Speicherlecks, keine Korrektheitsbugs, keine Absturzfunde. Die Bibliothek ist in
gutem Zustand; die offenen Punkte sind alle Härtungs-/Hygiene-Themen, keine funktionalen
Defekte.
