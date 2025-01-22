<?php
// session_start();

$cookie_env = getenv('COOKIE');
$token_id = null;

if ($cookie_env) {
    parse_str(str_replace('; ', '&', $cookie_env), $cookies);
    if (isset($cookies['token_id'])) {
        $token_id = $cookies['token_id'];
    }
}

$file1 = fopen('data.csv', 'r');
$users1 = [];
while (($data = fgetcsv($file1, 0, ",", '"', "\\")) !== FALSE) {
    $users1[] = $data;
}
fclose($file1);

$valid_token = false;
foreach ($users1 as $index => $user) {
    if (isset($user[2]) && $user[2] == $token_id) {
        $valid_token = true;
        break;
    }
}

if (!empty($token_id) && $valid_token) {
    header("Location: welcome.php");
    exit();
} else {
    setcookie('token_id', '', time() - 3600, '/');
}

$error_message = "";

// Handle form submission
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST['username'];
    $password = $_POST['password'];

    $file = fopen('data.csv', 'r');
    $users = [];
    while (($data = fgetcsv($file, 0, ",", '"', "\\")) !== FALSE) {
        $users[] = $data;
    }
    fclose($file);

    $user_found = false;
    foreach ($users as $index => $user) {
        if ($user[0] == $username) {
            $user_found = true;
            if ($password === $user[1]) {
                $token_id = bin2hex(random_bytes(16));
                $users[$index][2] = $token_id;

                $file = fopen('data.csv', 'w');
                foreach ($users as $user) {
                    fputcsv($file, $user, ',', '"', '\\');
                }
                fclose($file);

                setcookie('token_id', $token_id, time() + 3600, "/");
                header("Location: welcome.php");
                exit();
            } else {
                header("Location: login.php?msg=Invalid_password!");
            }
        }
    }
    
    if (!$user_found) {
        header("Location: login.php?msg=No_such_user_found!");
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(to right, #4facfe, #00f2fe);
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }
        h2 {
            text-align: center;
            color: #fff;
        }
        form {
            background-color: rgba(255, 255, 255, 0.9);
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
        }
        label {
            display: block;
            margin-bottom: 8px;
            font-weight: bold;
        }
        input[type="text"], input[type="password"] {
            width: calc(100% - 20px);
            padding: 10px;
            margin-bottom: 15px;
            border: 1px solid #ccc;
            border-radius: 5px;
            font-size: 14px;
        }
        .error {
            color: red;
            font-size: 12px;
            margin-bottom: 10px;
        }
        button {
            width: 100%;
            background-color: #4facfe;
            color: #fff;
            padding: 10px;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }
        button:hover {
            background-color: #00c9fe;
        }
        p {
            text-align: center;
            margin-top: 15px;
        }
        p a {
            color: #4facfe;
            text-decoration: none;
            font-weight: bold;
        }
        p a:hover {
            text-decoration: underline;
        }
        .container {
            display: block;
            width: 20%;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Login Page</h2>
        <form action="login.php" method="POST">
            <label for="username">Username:</label>
            <input type="text" id="username" name="username" required><br><br>

            <label for="password">Password:</label>
            <input type="password" id="password" name="password" required>
            <br><br>

            <button type="submit">Login</button>
        </form>
        <p>Don't have an account? <a href="signup.php">Sign Up</a></p>
    </div>
</body>
</html>
