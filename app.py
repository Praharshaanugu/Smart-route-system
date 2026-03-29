from flask import Flask, request, render_template
import subprocess

app = Flask(__name__)
@app.route('/')
def home():
    return render_template('index.html')
@app.route('/find', methods=['POST'])
def find():
    src = request.form['source']
    dest = request.form['destination']
    choice = request.form['choice']
    budget = request.form['budget']
    avoid = request.form['avoid']

    result = subprocess.run(
        [".\\travel.exe", src, dest, choice, budget, avoid],
        capture_output=True,
        text=True,
        timeout=10
    )

    output = result.stdout.split("\n")

    routes = []
    routes = []

    for line in output:
      if "Route" in line and "|" in line:
        try:
            parts = line.split("|")
            path = parts[0].split(":")[1].strip()
            cost = parts[1].replace("Cost:", "").strip()
            routes.append((path, cost))
        except:
            pass
      if not routes:
          return f"<pre>{result.stdout}</pre>"

    return render_template("result.html", routes=routes)
if __name__ == '__main__':
    app.run(debug=True)