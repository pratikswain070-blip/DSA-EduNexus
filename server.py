import os
import json
import subprocess
from flask import Flask, request, jsonify, send_file, send_from_directory

app = Flask(__name__, static_folder='.', static_url_path='')

def run_backend(args):
    """
    Run the C++ backend with the given arguments and return the parsed JSON.
    """
    try:
        # Run the compiled C++ executable
        result = subprocess.run(
            ['./backend'] + args,
            capture_output=True,
            text=True,
            timeout=10
        )
        
        # If the backend wrote something to standard output, try to parse it as JSON
        if result.stdout:
            try:
                return json.loads(result.stdout)
            except json.JSONDecodeError:
                return {"raw": result.stdout.strip()}
        
        # If the command failed, return the error output
        if result.returncode != 0:
            return {"error": result.stderr or "Unknown error"}
            
        return {}
        
    except subprocess.TimeoutExpired:
        return {"error": "Timeout expired while waiting for C++ backend"}
    except Exception as e:
        return {"error": str(e)}

# ============================================================
# PAGE ROUTES
# ============================================================

@app.route('/')
def index():
    return send_file('index.html')

@app.route('/directory')
def directory():
    return send_file('directory.html')

@app.route('/admissions')
def admissions():
    return send_file('admissions.html')

@app.route('/hierarchy')
def hierarchy():
    return send_file('hierarchy.html')

# ============================================================
# API ROUTES
# ============================================================

@app.route('/api/students', methods=['GET', 'POST'])
def students():
    if request.method == 'GET':
        return jsonify(run_backend(['list']))
        
    if request.method == 'POST':
        data = request.json
        if 'id' not in data or 'name' not in data or 'gpa' not in data:
            return jsonify({"error": "Invalid payload"}), 400
            
        safe_name = data['name'].replace(' ', '_')
        return jsonify(run_backend(['add', str(data['id']), safe_name, str(data['gpa'])]))

@app.route('/api/students/<int:id>', methods=['DELETE'])
def delete_student(id):
    return jsonify(run_backend(['delete', str(id)]))

@app.route('/api/students/search/<int:id>', methods=['GET'])
def search_student(id):
    return jsonify(run_backend(['search', str(id)]))

@app.route('/api/students/bsearch/<int:id>', methods=['GET'])
def bsearch_student(id):
    return jsonify(run_backend(['bsearch', str(id)]))

@app.route('/api/students/searchname/<name>', methods=['GET'])
def search_name(name):
    return jsonify(run_backend(['searchname', name]))

@app.route('/api/students/sort', methods=['POST'])
def sort_students():
    data = request.json or {}
    algo = data.get('algorithm', 'sort')
    cmd_map = {
        'insertion': 'sort',
        'bubble': 'bubbleSort',
        'merge': 'mergeSort',
        'quick': 'quickSort'
    }
    cmd = cmd_map.get(algo, 'sort')
    return jsonify(run_backend([cmd]))

@app.route('/api/students/undo', methods=['POST'])
def undo():
    return jsonify(run_backend(['undo']))

@app.route('/api/admissions', methods=['GET'])
def list_queue():
    return jsonify(run_backend(['queuelist']))

@app.route('/api/admissions/enqueue', methods=['POST'])
def enqueue():
    data = request.json
    name = data.get('name')
    if not name:
        return jsonify({"error": "Name required"}), 400
    return jsonify(run_backend(['enqueue', name]))

@app.route('/api/admissions/dequeue', methods=['POST'])
def dequeue():
    # 1. Get next ID
    students_data = run_backend(['list'])
    all_students = students_data.get('students', [])
    max_id = max([s['id'] for s in all_students], default=0)
    next_id = max_id + 1
    
    # 2. Dequeue
    data = run_backend(['dequeue'])
    if data.get('status') != 'ok':
        return jsonify(data)
        
    # 3. Add to directory
    safe_name = data['applicant'].replace(' ', '_')
    add_result = run_backend(['add', str(next_id), safe_name, '0.00'])
    
    if add_result.get('status') == 'ok':
        data['enrolled'] = True
        data['studentId'] = next_id
        data['enrollMessage'] = f"{data['applicant']} enrolled as student #{next_id}"
    else:
        data['enrolled'] = False
        data['enrollMessage'] = "Failed to enroll: " + add_result.get('error', 'unknown error')
        
    return jsonify(data)

@app.route('/api/analytics/gpatrend', methods=['GET'])
def gpa_trend():
    return jsonify(run_backend(['gpatrend']))

@app.route('/api/analytics/greedy', methods=['GET'])
def greedy():
    return jsonify(run_backend(['greedy']))

@app.route('/api/hierarchy/bst', methods=['GET'])
def bst_traverse():
    return jsonify(run_backend(['bst', 'traverse']))

@app.route('/api/hierarchy/bst/search/<int:id>', methods=['GET'])
def bst_search(id):
    return jsonify(run_backend(['bst', 'search', str(id)]))

@app.route('/api/curriculum/prereqs', methods=['GET'])
def prereqs():
    return jsonify(run_backend(['prereqs']))

@app.route('/api/curriculum/prereqs/<course>', methods=['GET'])
def prereqs_course(course):
    mode = request.args.get('mode', 'bfs')
    return jsonify(run_backend(['prereqs', course, mode]))

@app.route('/api/hierarchy/orgtree', methods=['GET'])
def orgtree():
    return jsonify(run_backend(['orgtree']))

if __name__ == '__main__':
    print("  ╔═══════════════════════════════════════════════╗")
    print("  ║   EduNexus Command Center — Python Server     ║")
    print("  ║   http://localhost:5001                       ║")
    print("  ╚═══════════════════════════════════════════════╝")
    app.run(port=5001, debug=True)
