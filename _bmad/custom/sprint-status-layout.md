# Matrix-Control — sprint-status.yaml layout

Team convention for `development_status` in `_bmad-output/implementation-artifacts/sprint-status.yaml`.

## Epic blocks

For each epic, in epic-file order:

1. One **blank line** before the block (not before the first epic under `development_status:`).
2. One **YAML comment** with the epic title from the planning epics source (`planning-artifacts/epics.md` or sharded `*epic*.md` headers):
   - Format: `# Epic {id}: {title}`
   - `{id}` is the epic identifier from the header (`0`, `1`, `R`, `U`, …).
   - `{title}` is the heading text after the colon (trimmed).
3. The `epic-{slug}:` status line (`epic-0`, `epic-r`, `epic-u`, …).
4. All story keys for that epic, in epic-file order.
5. `epic-{slug}-retrospective: optional` (or `done`).

## Example

```yaml
development_status:

  # Epic 0: Development Foundation (P-001)
  epic-0: done
  0-1-gitignore-and-root-directory-renames: done
  epic-0-retrospective: optional

  # Epic 1: Patch & Master Memory Model
  epic-1: done
  1-1-patchmodel-packed-buffer: done
  epic-1-retrospective: optional
```

## Preservation

When updating an existing file (sprint-planning refresh, create-story, dev-story, code-review, quick-dev):

- Keep all comment lines, blank lines, STATUS DEFINITIONS, WORKFLOW NOTES, and `action_items`.
- Never downgrade story statuses.
- Add comment headers for newly discovered epics; update titles if epic headers changed.
