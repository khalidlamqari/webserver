<?php
// Ensure the upload directory exists


if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['image'])) {
    $file = $_FILES['image'];

    // Check for upload errors
    if ($file['error'] === UPLOAD_ERR_OK) {
        $tmpName = $file['tmp_name']; // Temporary file path
        $fileName = basename($file['name']); // Original file name
        $targetFile = $uploadDir . $fileName;

        // Move the file to the upload directory
        if (move_uploaded_file($tmpName, $targetFile)) {
            echo "Image uploaded successfully: <a href=\"$targetFile\">$fileName</a>";
        } else {
            echo "Error: Failed to move the uploaded file.";
        }
    } else {
        echo "Error: Upload failed with error code " . $file['error'];
    }
} else {
    echo "No file uploaded.";
}
?>
