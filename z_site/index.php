<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <h1>
        <?php echo "hello brahim" ?>
    </h1>

    <h1>
        <?php echo "hello world" ?>
    </h1>
    <p>
    <?php echo "Request Method: " . $_SERVER['REQUEST_METHOD'];?>
    </p><br>
    <p>
    <?php
    echo "Request Method: " . $_SERVER['_'];
    ?>
    </p><br>
    <h1>
    <?php
    echo "Request Method: " . $_SERVER['USER'] . "<br>"; 
    echo "CONTENT_LENGTH : " . $_SERVER['CONTENT_LENGTH'];
    ?>
    </h1>
</body>
</html>