// index page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Login Page</title>
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
</head>
<body>
  <div class="container">
    <div class="row">
      <div class="col-md-6 offset-md-3">
        <h2 class="text-center">Login</h2>
          <div class="mb-3">
            <label for="userId" class="form-label">Your User ID</label>
            <input type="text" class="form-control" id="userId" name="userId" required>
          </div>
          <div class="mb-3">
            <label for="userPassword" class="form-label">Your Password</label>
            <input type="password" class="form-control" id="userPassword" name="userPassword" required>
          </div>
          <button onClick="sendText()" class="btn btn-primary">Submit</button>        
      </div>
    </div>
  </div>
  <script>
  function sendText() {
    var xhr = new XMLHttpRequest();

    let userId = document.getElementById("userId").value;
    let userPassword = document.getElementById("userPassword").value;
	  xhr.open("GET", `/data?userId=${userId}&userPassword=${userPassword}`, true); 
    xhr.send();
  }
  </script>
</body>
</html>
)rawliteral";