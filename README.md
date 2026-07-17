# Daily Markdown Logger
Lightweight native Windows application written in C using the Win32 API
Used for saving daily reflections as date-stamped Markdown files
No third-party GUI frameworks involved

## Current Features

- Native Win32 interface
- Multiline text entry
- Date-based Markdown filenames
- File-operation error reporting
- Modular UI config and persistence

## Screenshot

_TBA

## Build steps

```
bat
make
```

## Roadmap

### UI Structure & Layout

- [x] Create STATIC label to display current date
- [x] Create STATIC label to show a generic prompt (e.g., "What did you learn today?")
- [x] Create EDIT control for multiline user input
- [x] Create BUTTON control labeled "Save"
- [x] Arrange controls neatly in the window

### Core Functionality

- [x] Use GetLocalTime() to retrieve the current date
- [x] Generate a markdown filename based on the date (e.g., 2025-05-29.md)
- [x] On button click, retrieve text from EDIT control
- [x] Write the text to the markdown file
- [x] Handle file I/O errors gracefully (e.g., show MessageBox on failure)

### Modularity

- [x] Create header and source files to migrate functions into
- [x] Update Makefile to address new source files and architecture
- [x] Migrate constants, macros, control IDs, functionality from central main C file

### Style (Optional, Later Stages)

- [ ] Choose consistent fonts, spacing, and colors
- [ ] Define a basic internal style guide (e.g., padding, alignment)
- [ ] Replace STATIC/EDIT/BUTTON with custom drawn controls (optional upgrade)