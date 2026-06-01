(function() {
  // --- DEFAULT MOCK DATA ---
  const defaultStudents = [
    { id: 1, name: "Pratik_Swain", gpa: 3.85 },
    { id: 2, name: "Aditi_Sharma", gpa: 3.92 },
    { id: 3, name: "Rahul_Verma", gpa: 3.45 },
    { id: 4, name: "Sneha_Reddy", gpa: 3.78 },
    { id: 5, name: "Vikram_Malhotra", gpa: 3.60 },
    { id: 6, name: "Priya_Nair", gpa: 3.95 },
    { id: 7, name: "Arjun_Mehta", gpa: 3.20 }
  ];

  const defaultQueue = [
    { applicant: "Karan Johar", position: 1 },
    { applicant: "Rohan Nanda", position: 2 },
    { applicant: "Shanaya Singhania", position: 3 }
  ];

  const courseGraph = {
    'Machine_Learning': ['Advanced_Algorithms', 'Linear_Algebra'],
    'Advanced_Algorithms': ['Data_Structures'],
    'Linear_Algebra': [],
    'Data_Structures': ['Programming_Fundamentals'],
    'Discrete_Math': [],
    'Database_Systems': ['Data_Structures'],
    'Operating_Systems': ['Computer_Architecture'],
    'Programming_Fundamentals': [],
    'Calculus': [],
    'Computer_Architecture': ['Digital_Logic'],
    'Digital_Logic': []
  };

  const defaultSessions = [
    { name: "Algorithms_Group", start: 540, end: 660 },
    { name: "Database_Systems", start: 600, end: 720 },
    { name: "Web_Development", start: 780, end: 900 },
    { name: "Computer_Networks", start: 840, end: 960 },
    { name: "Software_Engineering", start: 990, end: 1110 }
  ];

  // --- INITIALIZE LOCAL STORAGE ---
  function getStored(key, fallback) {
    const val = localStorage.getItem(key);
    if (!val) {
      localStorage.setItem(key, JSON.stringify(fallback));
      return fallback;
    }
    return JSON.parse(val);
  }

  function setStored(key, val) {
    localStorage.setItem(key, JSON.stringify(val));
  }

  // Intercept window.fetch
  const originalFetch = window.fetch;
  window.fetch = async function(resource, init) {
    if (typeof resource !== 'string' || !resource.startsWith('/api/')) {
      return originalFetch(resource, init);
    }

    try {
      // Attempt to hit the real Flask backend server
      const response = await originalFetch(resource, init);
      const contentType = response.headers.get('content-type') || '';
      
      // If server is not running, Vercel/static servers return a 404 HTML file instead of JSON
      if (response.ok && contentType.includes('application/json')) {
        return response;
      }
      if (response.status === 404 || contentType.includes('text/html')) {
        throw new Error('Endpoint not found or returned HTML page');
      }
      return response;
    } catch (err) {
      // Flask Backend Server is not running -> Fallback to client-side localStorage Mock DB!
      return handleMock(resource, init);
    }
  };

  // --- MOCK API ROUTER ---
  function handleMock(url, init) {
    const method = (init && init.method || 'GET').toUpperCase();
    const students = getStored('mock_students', defaultStudents);
    const queue = getStored('mock_queue', defaultQueue);
    const undoStack = getStored('mock_undo_stack', []);

    // 1. GET /api/students
    if (url === '/api/students' && method === 'GET') {
      return jsonResponse({ count: students.length, students });
    }

    // 2. POST /api/students
    if (url === '/api/students' && method === 'POST') {
      const data = JSON.parse(init.body);
      const newStudent = { id: parseInt(data.id), name: data.name.replace(/ /g, '_'), gpa: parseFloat(data.gpa) };
      students.push(newStudent);
      setStored('mock_students', students);
      return jsonResponse({ status: "ok", student: newStudent });
    }

    // 3. DELETE /api/students/<id>
    if (url.startsWith('/api/students/') && method === 'DELETE') {
      const id = parseInt(url.split('/').pop());
      const idx = students.findIndex(s => s.id === id);
      if (idx !== -1) {
        const deleted = students.splice(idx, 1)[0];
        setStored('mock_students', students);
        
        // Push to undo stack
        undoStack.push({ type: 'delete', student: deleted });
        setStored('mock_undo_stack', undoStack);
        
        return jsonResponse({ status: "ok", student: deleted });
      }
      return jsonResponse({ error: "Student not found" }, 404);
    }

    // 4. POST /api/students/sort
    if (url === '/api/students/sort' && method === 'POST') {
      const data = JSON.parse(init.body);
      const algo = data.algorithm || 'sort';
      
      if (algo === 'insertion' || algo === 'bubble') {
        students.sort((a, b) => a.id - b.id);
      } else if (algo === 'merge') {
        students.sort((a, b) => b.gpa - a.gpa); // sort by GPA descending
      } else if (algo === 'quick') {
        students.sort((a, b) => a.name.localeCompare(b.name));
      }
      setStored('mock_students', students);
      return jsonResponse({ status: "ok", message: `Students sorted using ${algo} sort successfully!` });
    }

    // 5. POST /api/students/undo
    if (url === '/api/students/undo' && method === 'POST') {
      if (undoStack.length > 0) {
        const lastAction = undoStack.pop();
        setStored('mock_undo_stack', undoStack);
        if (lastAction.type === 'delete') {
          students.push(lastAction.student);
          setStored('mock_students', students);
          return jsonResponse({ status: "ok", message: `Restored deleted student ${lastAction.student.name.replace(/_/g, ' ')}` });
        }
      }
      return jsonResponse({ status: "error", message: "Nothing to undo" });
    }

    // 6. GET /api/students/search/<id>
    if (url.startsWith('/api/students/search/') && method === 'GET') {
      const id = parseInt(url.split('/').pop());
      const s = students.find(s => s.id === id);
      return jsonResponse(s ? { found: true, student: s } : { found: false });
    }

    // 7. GET /api/students/bsearch/<id>
    if (url.startsWith('/api/students/bsearch/') && method === 'GET') {
      const id = parseInt(url.split('/').pop());
      const s = students.find(s => s.id === id);
      return jsonResponse(s ? { found: true, student: s } : { found: false });
    }

    // 8. GET /api/students/searchname/<name>
    if (url.startsWith('/api/students/searchname/') && method === 'GET') {
      const name = decodeURIComponent(url.split('/').pop()).toLowerCase();
      const results = students.filter(s => s.name.replace(/_/g, ' ').toLowerCase().includes(name));
      return jsonResponse({ results });
    }

    // 9. GET /api/admissions
    if (url === '/api/admissions' && method === 'GET') {
      return jsonResponse({ count: queue.length, queue });
    }

    // 10. POST /api/admissions/enqueue
    if (url === '/api/admissions/enqueue' && method === 'POST') {
      const data = JSON.parse(init.body);
      const newApplicant = { applicant: data.name, position: queue.length + 1 };
      queue.push(newApplicant);
      setStored('mock_queue', queue);
      return jsonResponse({ status: "ok", applicant: newApplicant });
    }

    // 11. POST /api/admissions/dequeue
    if (url === '/api/admissions/dequeue' && method === 'POST') {
      if (queue.length > 0) {
        const dequeued = queue.shift();
        
        // Update positions of remaining applicants
        queue.forEach((item, index) => item.position = index + 1);
        setStored('mock_queue', queue);
        
        // Enroll student automatically in directory
        const maxId = students.reduce((max, s) => s.id > max ? s.id : max, 0);
        const nextId = maxId + 1;
        const newStudent = { id: nextId, name: dequeued.applicant.replace(/ /g, '_'), gpa: 0.00 };
        students.push(newStudent);
        setStored('mock_students', students);

        return jsonResponse({
          status: "ok",
          applicant: dequeued.applicant,
          enrolled: true,
          studentId: nextId,
          enrollMessage: `${dequeued.applicant} enrolled as student #${nextId}`,
          remaining: queue.length
        });
      }
      return jsonResponse({ status: "error", message: "Queue is empty" });
    }

    // 12. GET /api/analytics/gpatrend
    if (url === '/api/analytics/gpatrend' && method === 'GET') {
      const trend = getLIS(students);
      return jsonResponse(trend);
    }

    // 13. GET /api/analytics/greedy
    if (url === '/api/analytics/greedy' && method === 'GET') {
      const greedy = getGreedy(defaultSessions);
      return jsonResponse(greedy);
    }

    // 14. GET /api/curriculum/prereqs
    if (url === '/api/curriculum/prereqs' && method === 'GET') {
      return jsonResponse({ graph: courseGraph });
    }

    // 15. GET /api/curriculum/prereqs/<course>
    if (url.startsWith('/api/curriculum/prereqs/') && method === 'GET') {
      const parts = url.split('?')[0].split('/');
      const course = parts.pop();
      const params = new URLSearchParams(url.split('?')[1] || '');
      const mode = params.get('mode') || 'bfs';
      
      const path = mode === 'bfs' ? bfs(course) : dfs(course);
      return jsonResponse({ path });
    }

    // 16. GET /api/hierarchy/bst
    if (url === '/api/hierarchy/bst' && method === 'GET') {
      const sorted = [...students].sort((a, b) => a.id - b.id);
      return jsonResponse({ students: sorted });
    }

    // 17. GET /api/hierarchy/bst/search/<id>
    if (url.startsWith('/api/hierarchy/bst/search/') && method === 'GET') {
      const id = parseInt(url.split('/').pop());
      const s = students.find(s => s.id === id);
      return jsonResponse(s ? { found: true, student: s } : { found: false });
    }

    // 18. GET /api/hierarchy/orgtree
    if (url === '/api/hierarchy/orgtree' && method === 'GET') {
      return jsonResponse({
        name: "EduNexus University",
        type: "university",
        children: [
          {
            name: "College of Engineering",
            type: "department",
            children: [
              { name: "Computer Science Dept", type: "department", children: [
                { name: "Dr. Alan Turing", type: "faculty", role: "Department Head" },
                { name: "Dr. Grace Hopper", type: "faculty", role: "Professor" }
              ]},
              { name: "Electrical Engineering Dept", type: "department", children: [
                { name: "Dr. Nikola Tesla", type: "faculty", role: "Professor" }
              ]}
            ]
          },
          {
            name: "College of Science",
            type: "department",
            children: [
              { name: "Mathematics Dept", type: "department", children: [
                { name: "Dr. Carl Friedrich Gauss", type: "faculty", role: "Professor" }
              ]}
            ]
          }
        ]
      });
    }

    return jsonResponse({ error: "Endpoint mock not implemented" }, 404);
  }

  // --- HELPERS ---
  function jsonResponse(data, status = 200) {
    return new Response(JSON.stringify(data), {
      status,
      headers: { 'Content-Type': 'application/json' }
    });
  }

  // Dynamic LIS (Longest Increasing Subsequence)
  function getLIS(arr) {
    const sorted = [...arr].sort((a, b) => a.id - b.id);
    const n = sorted.length;
    if (n === 0) return { allGpas: [], path: [], length: 0 };
    
    const allGpas = sorted.map(s => s.gpa);
    const dp = Array(n).fill(1);
    const parent = Array(n).fill(-1);
    
    let maxLen = 0;
    let maxIdx = 0;
    
    for (let i = 0; i < n; i++) {
      for (let j = 0; j < i; j++) {
        if (sorted[i].gpa > sorted[j].gpa && dp[i] < dp[j] + 1) {
          dp[i] = dp[j] + 1;
          parent[i] = j;
        }
      }
      if (dp[i] > maxLen) {
        maxLen = dp[i];
        maxIdx = i;
      }
    }
    
    const path = [];
    let curr = maxIdx;
    while (curr !== -1) {
      path.push({
        index: curr,
        gpa: sorted[curr].gpa,
        id: sorted[curr].id
      });
      curr = parent[curr];
    }
    path.reverse();
    
    return { allGpas, path, length: maxLen };
  }

  // Greedy Activity Selection (Sort by End Time)
  function getGreedy(sessions) {
    const sorted = [...sessions].sort((a, b) => a.end - b.end);
    const selected = [];
    let lastEnd = 0;
    for (const s of sorted) {
      if (s.start >= lastEnd) {
        selected.push(s);
        lastEnd = s.end;
      }
    }
    return { all: sessions, selected };
  }

  // Graph Traversal Algorithms
  function bfs(start) {
    const visited = [];
    const queue = [start];
    const seen = new Set([start]);
    
    while (queue.length > 0) {
      const curr = queue.shift();
      visited.push(curr);
      const neighbors = courseGraph[curr] || [];
      for (const n of neighbors) {
        if (!seen.has(n)) {
          seen.add(n);
          queue.push(n);
        }
      }
    }
    return visited;
  }

  function dfs(start) {
    const visited = [];
    const seen = new Set();
    function explore(curr) {
      seen.add(curr);
      visited.push(curr);
      const neighbors = courseGraph[curr] || [];
      for (const n of neighbors) {
        if (!seen.has(n)) explore(n);
      }
    }
    explore(start);
    return visited;
  }
})();
