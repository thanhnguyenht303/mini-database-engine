# Mini DB Architecture

## MVP
- Single table
- Key: uint64_t id
- Commands later:
  - insert
  - select
  - find

## Storage plan
- Page size: 4096 bytes
- Root page starts at page 0
- First version: single leaf page only
- Later:
  - leaf split
  - internal nodes
  - persistence