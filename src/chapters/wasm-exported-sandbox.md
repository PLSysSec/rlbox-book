# Using a Wasm module with imported memory and imported tables

By default, RLBox operates on Wasm modules with an imported memory and imported
table. This allows RLBox to optimize the memory allocation's alignment for its
internal operations. However, you can also use modules with exported memory and
exported tables (albeit with some performance penalty).

To do this, adjust the flags during the compilation of your Wasm module to
export memory and tables. Specifically, remove the arguments
`-Wl,--import-memory -Wl,--import-table` if present, and use the arguments
`-Wl,--export-all -Wl,--export-table`