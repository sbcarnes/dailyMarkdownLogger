# Daily Markdown Logger - MVP Checklist
*Date started: 2025-05-29*

## 🧱 UI Structure & Layout

- [x] Create STATIC label to display current date
- [x] Create STATIC label to show a generic prompt (e.g., "What did you learn today?")
- [x] Create EDIT control for multiline user input
- [x] Create BUTTON control labeled "Save"
- [x] Arrange controls neatly in the window

## 🔧 Core Functionality

- [x] Use GetLocalTime() to retrieve the current date
- [x] Generate a markdown filename based on the date (e.g., 2025-05-29.md)
- [x] On button click, retrieve text from EDIT control
- [x] Write the text to the markdown file
- [ ] Handle file I/O errors gracefully (e.g., show MessageBox on failure)

## 🎨 Style (Optional, Later Stages)

- [ ] Choose consistent fonts, spacing, and colors
- [ ] Define a basic internal style guide (e.g., padding, alignment)
- [ ] Replace STATIC/EDIT/BUTTON with custom drawn controls (optional upgrade)