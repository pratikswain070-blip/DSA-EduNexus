# EduNexus: C++ backend + REST API

This folder includes a small C++ backend executable and a Node.js Express server that exposes a REST API to the frontend `code.html` page.

Quick start (macOS / Linux):

1. Build the C++ backend:

```bash
g++ backend.cpp -o backend -std=c++17
```

2. Install Node deps and start server:

```bash
npm install
npm start
```

3. Open the UI in your browser:

http://localhost:3000/code.html

Notes:
- The Node server calls the `backend` executable with a simple command protocol: `add`, `list`, `search`, `sort`, `saveIndex`.
- The frontend uses fetch to call `/api/students` and `/api/students` POST to add.
