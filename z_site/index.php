<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <?php
        $requestBody = file_get_contents('php://stdin');
        echo "Request Body:\n";
        echo $requestBody;
        sleep(10);
    ?>
</body>
</html>