// Compacted EduNexus backend (functionality preserved)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <queue>
using namespace std;
const string DATA_FILE = "students.dat", UNDO_FILE = "undo_stack.dat",
             QUEUE_FILE = "admission_queue.dat", COURSES_FILE = "courses.dat",
             SESSIONS_FILE = "study_sessions.dat";
struct Student {
  int id;
  string name;
  float gpa;
};
vector<Student> studentList;

// Helper function to join command line arguments into a single string
// The 'start' index allows skipping command name arguments.
string joinArgs(int start, int argc, char **argv) {
  string r;
  for (int i = start; i < argc; ++i) {
    if (!r.empty())
      r += ' ';
    r += argv[i];
  }
  return r;
}
// Escapes double quotes in strings to ensure valid JSON output
string escapeJson(const string &s) {
  string o;
  for (char c : s)
    o += (c == '"' ? string("\\\"") : string(1, c));
  return o;
}
// Serializes a single Student struct into a JSON format string
string studentToJson(const Student &s) {
  ostringstream os;
  os << fixed << setprecision(2);
  os << "{\"id\":" << s.id << ",\"name\":\"" << escapeJson(s.name)
     << "\",\"gpa\":" << s.gpa << "}";
  return os.str();
}
// Serializes a vector of Student structs into a JSON array string
string studentsToJsonArray(const vector<Student> &a) {
  string o = "[";
  for (size_t i = 0; i < a.size(); ++i) {
    if (i)
      o += ",";
    o += studentToJson(a[i]);
  }
  o += "]";
  return o;
}

// Writes all students currently in memory to the text data file
void saveStudents() {
  ofstream f(DATA_FILE);
  for (auto &s : studentList)
    f << s.id << " \"" << s.name << "\" " << fixed << setprecision(2) << s.gpa
      << "\n";
}
// Reads students from the data file into the memory vector on startup
void loadStudents() {
  ifstream f(DATA_FILE);
  if (!f)
    return;
  studentList.clear();
  string line;
  while (getline(f, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    Student s;
    ss >> s.id >> ws;
    if (ss.peek() == '\"') {
      ss.get();
      getline(ss, s.name, '\"');
    } else
      ss >> s.name;
    ss >> s.gpa;
    studentList.push_back(s);
  }
}

// Performs O(N) linear search to find a student by ID. Returns index or -1 if not found.
int linearSearch(int id) {
  for (size_t i = 0; i < studentList.size(); ++i)
    if (studentList[i].id == id)
      return (int)i;
  return -1;
}
// Performs O(log N) binary search on sorted array to find a student by ID. Returns index or -1 if not found.
int binarySearchId(int id) {
  int l = 0, r = (int)studentList.size() - 1;
  while (l <= r) {
    int m = (l + r) / 2;
    if (studentList[m].id == id)
      return m;
    if (studentList[m].id < id)
      l = m + 1;
    else
      r = m - 1;
  }
  return -1;
}

// Sorts the student list in ascending order based on student ID (O(N log N))
void sortById() {
  sort(studentList.begin(), studentList.end(),
       [](const Student &a, const Student &b) { return a.id < b.id; });
}
// Sorts the student list in descending order based on GPA (O(N log N))
void sortByGpaDesc() {
  sort(studentList.begin(), studentList.end(),
       [](const Student &a, const Student &b) { return a.gpa > b.gpa; });
}
// Sorts the student list alphabetically based on student name (O(N log N))
void sortByName() {
  sort(studentList.begin(), studentList.end(),
       [](const Student &a, const Student &b) { return a.name < b.name; });
}

// Pushes an undo action to the undo stack file for persistence
void pushUndo(const string &s) {
  ofstream o(UNDO_FILE, ios::app);
  o << s << "\n";
}
// Pops the most recent undo action from the undo stack file
string popUndo() {
  ifstream in(UNDO_FILE);
  vector<string> L;
  string ln;
  while (getline(in, ln))
    if (!ln.empty())
      L.push_back(ln);
  if (L.empty())
    return string();
  string last = L.back();
  L.pop_back();
  ofstream o(UNDO_FILE);
  for (auto &r : L)
    o << r << "\n";
  return last;
}

// Loads the admission queue from the queue file
vector<string> loadQueue() {
  vector<string> q;
  ifstream f(QUEUE_FILE);
  string l;
  while (getline(f, l))
    if (!l.empty())
      q.push_back(l);
  return q;
}
// Saves the current admission queue to the queue file
void saveQueue(const vector<string> &q) {
  ofstream o(QUEUE_FILE);
  for (auto &s : q)
    o << s << "\n";
}

// Graph structure to represent course prerequisites using an Adjacency List
struct CourseGraph {
  unordered_map<string, vector<string>> adj;
  void load() {
    ifstream f(COURSES_FILE);
    string l;
    while (getline(f, l)) {
      if (l.empty())
        continue;
      stringstream ss(l);
      string a, b;
      ss >> a >> b;
      adj[a].push_back(b);
      if (!adj.count(b))
        adj[b] = {};
    }
  }
  string bfs(const string &start) {
    if (!adj.count(start))
      return "[]";
    unordered_map<string, bool> vis;
    queue<string> q;
    q.push(start);
    vis[start] = 1;
    string o = "[";
    bool first = 1;
    while (!q.empty()) {
      auto u = q.front();
      q.pop();
      if (!first)
        o += ",";
      o += "\"" + escapeJson(u) + "\"";
      first = 0;
      for (auto &v : adj[u])
        if (!vis[v]) {
          vis[v] = 1;
          q.push(v);
        }
    }
    o += "]";
    return o;
  }
  void dfsRec(const string &u, unordered_map<string, bool> &vis, string &out,
              bool &first) {
    vis[u] = 1;
    if (!first)
      out += ",";
    out += "\"" + escapeJson(u) + "\"";
    first = 0;
    for (auto &v : adj[u])
      if (!vis[v])
        dfsRec(v, vis, out, first);
  }
  string dfs(const string &start) {
    if (!adj.count(start))
      return "[]";
    unordered_map<string, bool> vis;
    string o = "[";
    bool first = 1;
    dfsRec(start, vis, o, first);
    o += "]";
    return o;
  }
  string allJson() {
    string o = "{";
    bool fn = 1;
    for (auto &p : adj) {
      if (!fn)
        o += ",";
      o += "\"" + escapeJson(p.first) + "\":[";
      bool fv = 1;
      for (const string &v : p.second) {
        if (!fv) o += ",";
        o += "\"" + escapeJson(v) + "\"";
        fv = 0;
      }
      o += "]";
      fn = 0;
    }
    o += "}";
    return o;
  }
};

// Calculates the Longest Increasing Subsequence (LIS) of GPAs using Dynamic Programming.
// Returns a JSON string containing the length of the trend, the GPAs, and the actual path of indices. Time Complexity: O(N^2).
string gpaTrendDP() {
  int n = studentList.size();
  if (!n)
    return "{\"length\":0,\"allGpas\":[],\"path\":[]}";
  vector<float> g;
  for (auto &s : studentList)
    g.push_back(s.gpa);
  
  vector<int> dp(n, 1);
  vector<int> parent(n, -1);
  int mx = 0;
  int last_idx = -1;
  
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      if (g[i] > g[j] && dp[j] + 1 > dp[i]) {
        dp[i] = dp[j] + 1;
        parent[i] = j;
      }
    }
    if (dp[i] > mx) {
      mx = dp[i];
      last_idx = i;
    }
  }
  
  vector<int> path;
  int curr = last_idx;
  while (curr != -1) {
    path.push_back(curr);
    curr = parent[curr];
  }
  reverse(path.begin(), path.end());

  ostringstream os;
  os << fixed << setprecision(2);
  os << "{\"length\":" << mx << ",\"allGpas\":[";
  for (int i = 0; i < n; ++i) {
    if (i)
      os << ",";
    os << g[i];
  }
  os << "],\"path\":[";
  for (size_t i = 0; i < path.size(); ++i) {
    if (i)
      os << ",";
    os << "{\"index\":" << path[i] << ",\"gpa\":" << g[path[i]] << ",\"id\":" << studentList[path[i]].id << "}";
  }
  os << "]}";
  return os.str();
}

// Implements a Greedy Algorithm for the Activity Selection Problem.
// Selects the maximum number of non-overlapping study sessions by sorting them by end times.
string greedyScheduler() {
  ifstream f(SESSIONS_FILE);
  struct S {
    string n;
    int s, e;
  };
  vector<S> v;
  string line;
  while (getline(f, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    S s;
    getline(ss, s.n, '|');
    ss >> s.s >> s.e;
    v.push_back(s);
  }
  if (v.empty())
    return "{\"selected\":[],\"all\":[]}";
  sort(v.begin(), v.end(), [](const S &a, const S &b) { return a.e < b.e; });
  vector<S> sel;
  sel.push_back(v[0]);
  int last = v[0].e;
  for (size_t i = 1; i < v.size(); ++i)
    if (v[i].s >= last) {
      sel.push_back(v[i]);
      last = v[i].e;
    }
  string out = "{\"selected\":[";
  for (size_t i = 0; i < sel.size(); ++i) {
    if (i)
      out += ",";
    out += "{\"name\":\"" + escapeJson(sel[i].n) +
           "\",\"start\":" + to_string(sel[i].s) +
           ",\"end\":" + to_string(sel[i].e) + "}";
  }
  out += "],\"all\":[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i)
      out += ",";
    out += "{\"name\":\"" + escapeJson(v[i].n) +
           "\",\"start\":" + to_string(v[i].s) +
           ",\"end\":" + to_string(v[i].e) + "}";
  }
  out += "]}";
  return out;
}

// Main entry point for the backend executable.
// Acts as a command-line interface (CLI) to execute specific operations based on arguments.
// The Python server calls this executable with arguments and parses the JSON output.
int main(int argc, char **argv) {
  loadStudents();
  if (argc < 2) {
    cout << "{\"error\":\"no command provided\"}" << endl;
    return 0;
  }
  string cmd = argv[1];
  if (cmd == "add") {
    int id = atoi(argv[2]);
    string name = argv[3];
    float g = atof(argv[4]);
    if (linearSearch(id) != -1) {
      cout << "{\"error\":\"id already exists\"}" << endl;
      return 1;
    }
    studentList.push_back({id, name, g});
    saveStudents();
    pushUndo(string("add ") + to_string(id));
    cout << "{\"status\":\"ok\",\"student\":" << studentToJson({id, name, g})
         << "}" << endl;
  } else if (cmd == "list") {
    cout << "{\"students\":" << studentsToJsonArray(studentList)
         << ",\"count\":" << studentList.size() << "}" << endl;
  } else if (cmd == "delete") {
    int id = atoi(argv[2]);
    auto it = find_if(studentList.begin(), studentList.end(),
                      [&](const Student &s) { return s.id == id; });
    if (it == studentList.end()) {
      cout << "{\"error\":\"student not found\"}" << endl;
      return 1;
    }
    Student rem = *it;
    studentList.erase(it);
    saveStudents();
    ostringstream u;
    u << fixed << setprecision(2) << "delete " << rem.id << " \"" << rem.name
      << "\" " << rem.gpa;
    pushUndo(u.str());
    cout << "{\"status\":\"ok\",\"student\":" << studentToJson(rem) << "}"
         << endl;
  } else if (cmd == "search") {
    int id = atoi(argv[2]);
    int idx = linearSearch(id);
    if (idx == -1)
      cout << "{\"found\":false}" << endl;
    else
      cout << "{\"found\":true,\"student\":" << studentToJson(studentList[idx])
           << "}" << endl;
  } else if (cmd == "bsearch") {
    int id = atoi(argv[2]);
    sortById();
    saveStudents();
    int idx = binarySearchId(id);
    if (idx == -1)
      cout << "{\"found\":false}" << endl;
    else
      cout << "{\"found\":true,\"student\":" << studentToJson(studentList[idx])
           << "}" << endl;
  } else if (cmd == "sort") {
    sortById();
    saveStudents();
    cout << "{\"status\":\"ok\",\"students\":"
         << studentsToJsonArray(studentList) << "}" << endl;
  } else if (cmd == "bubbleSort") {
    sortById();
    saveStudents();
    cout << "{\"status\":\"ok\",\"students\":"
         << studentsToJsonArray(studentList) << "}" << endl;
  } else if (cmd == "mergeSort") {
    sortByGpaDesc();
    saveStudents();
    cout << "{\"status\":\"ok\",\"students\":"
         << studentsToJsonArray(studentList) << "}" << endl;
  } else if (cmd == "quickSort") {
    sortByName();
    saveStudents();
    cout << "{\"status\":\"ok\",\"students\":"
         << studentsToJsonArray(studentList) << "}" << endl;
  } else if (cmd == "undo") {
    string a = popUndo();
    if (a.empty())
      cout << "{\"status\":\"empty\"}" << endl;
    else {
      stringstream ss(a);
      string t;
      ss >> t;
      if (t == "add") {
        int id;
        ss >> id;
        studentList.erase(
            remove_if(studentList.begin(), studentList.end(),
                      [&](const Student &s) { return s.id == id; }),
            studentList.end());
        saveStudents();
        cout << "{\"status\":\"ok\",\"message\":\"Undid Add\"}" << endl;
      } else if (t == "delete") {
        int id;
        float g;
        string name;
        ss >> id >> ws;
        if (ss.peek() == '\"') {
          ss.get();
          getline(ss, name, '\"');
        } else
          ss >> name;
        ss >> g;
        studentList.push_back({id, name, g});
        saveStudents();
        cout << "{\"status\":\"ok\",\"message\":\"Undid Delete\"}" << endl;
      }
    }
  } else if (cmd == "enqueue") {
    string a = joinArgs(2, argc, argv);
    auto q = loadQueue();
    q.push_back(a);
    saveQueue(q);
    cout << "{\"status\":\"ok\",\"applicant\":\"" << escapeJson(a) << "\"}"
         << endl;
  } else if (cmd == "dequeue") {
    auto q = loadQueue();
    if (q.empty())
      cout << "{\"status\":\"empty\"}" << endl;
    else {
      string p = q.front();
      q.erase(q.begin());
      saveQueue(q);
      cout << "{\"status\":\"ok\",\"applicant\":\"" << escapeJson(p) << "\"}"
           << endl;
    }
  } else if (cmd == "queuelist") {
    auto q = loadQueue();
    string o = "{\"queue\":[] ,\"count\":0}";
    if (!q.empty()) {
      o = "{\"queue\":[";
      for (size_t i = 0; i < q.size(); ++i) {
        if (i)
          o += ",";
        o += "{\"position\":" + to_string(i + 1) + ",\"applicant\":\"" +
             escapeJson(q[i]) + "\"}";
      }
      o += "],\"count\":" + to_string(q.size()) + "}";
    }
    cout << o << endl;
  } else if (cmd == "gpatrend") {
    cout << gpaTrendDP() << endl;
  } else if (cmd == "greedy") {
    cout << greedyScheduler() << endl;
  } else if (cmd == "bst") { // emulate BST operations using sorted array
    sortById();
    string action = "traverse";
    if (argc >= 3)
      action = argv[2];
    if (action == "traverse")
      cout << "{\"students\":" << studentsToJsonArray(studentList) << "}"
           << endl;
    else if (action == "search") {
      int id = atoi(argv[3]);
      int idx = binarySearchId(id);
      if (idx != -1)
        cout << "{\"found\":true,\"student\":"
             << studentToJson(studentList[idx]) << "}" << endl;
      else
        cout << "{\"found\":false}" << endl;
    }
  } else if (cmd == "prereqs") {
    CourseGraph g;
    g.load();
    if (argc < 3)
      cout << "{\"graph\":" << g.allJson() << "}" << endl;
    else {
      string course = argv[2];
      string mode = "bfs";
      if (argc >= 4)
        mode = argv[3];
      if (mode == "bfs")
        cout << "{\"path\":" << g.bfs(course) << "}" << endl;
      else
        cout << "{\"path\":" << g.dfs(course) << "}" << endl;
    }
  } else if (cmd == "searchname") {
    string q = joinArgs(2, argc, argv);
    transform(q.begin(), q.end(), q.begin(), ::tolower);
    vector<Student> res;
    for (auto &s : studentList) {
      string n = s.name;
      replace(n.begin(), n.end(), '_', ' ');
      string ln = n;
      transform(ln.begin(), ln.end(), ln.begin(), ::tolower);
      if (ln.find(q) != string::npos)
        res.push_back(s);
    }
    cout << "{\"results\":" << studentsToJsonArray(res) << "}" << endl;
  } else if (cmd == "orgtree") {
    cout << "{\"name\":\"EduNexus "
            "University\",\"type\":\"university\",\"children\":[{\"name\":"
            "\"Computer "
            "Science\",\"type\":\"department\",\"children\":[{\"name\":\"Dr. "
            "Alan Turing\",\"type\":\"faculty\",\"role\":\"Department "
            "Head\",\"children\":[]},{\"name\":\"Dr. Grace "
            "Hopper\",\"type\":\"faculty\",\"role\":\"Professor\",\"children\":"
            "[]},{\"name\":\"Dr. Ada "
            "Lovelace\",\"type\":\"faculty\",\"role\":\"Associate "
            "Professor\",\"children\":[]}]},{\"name\":\"Electrical "
            "Engineering\",\"type\":\"department\",\"children\":[{\"name\":"
            "\"Dr. Nikola Tesla\",\"type\":\"faculty\",\"role\":\"Department "
            "Head\",\"children\":[]},{\"name\":\"Dr. James "
            "Maxwell\",\"type\":\"faculty\",\"role\":\"Professor\","
            "\"children\":[]}]},{\"name\":\"Mathematics\",\"type\":"
            "\"department\",\"children\":[{\"name\":\"Dr. Srinivasa "
            "Ramanujan\",\"type\":\"faculty\",\"role\":\"Department "
            "Head\",\"children\":[]},{\"name\":\"Dr. Emmy "
            "Noether\",\"type\":\"faculty\",\"role\":\"Professor\","
            "\"children\":[]}]}]}"
         << endl;
  } else if (cmd == "arrays") {
    cout << "{\"semesters\":[\"Fall\",\"Spring\",\"Summer\"],\"departments\":["
            "\"Computer Science\",\"Electrical Engineering\",\"Mathematics\"]}"
         << endl;
  } else {
    cout << "{\"error\":\"unknown command\"}" << endl;
  }
  return 0;
}
